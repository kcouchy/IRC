/* ************************************************************************** */
/*                                                                            */
/*                                                              ++            */
/*   ClientParser.cpp                                          +**+   +*  *   */
/*                                                             ##%#*###*+++   */
/*   By: aboyreau <bnzlvosnb@mozmail.com>                     +**+ -- ##+     */
/*                                                            # *   *. #*     */
/*   Created: 2024/07/17 11:59:26 by aboyreau          **+*+  * -_._-   #+    */
/*   Updated: 2024/07/29 23:23:54 by aboyreau          +#-.-*  +         *    */
/*                                                     *-.. *   ++       #    */
/* ************************************************************************** */

#include "ClientParser.h"
#include "Messageable.h"
#include "utils.h"
#include <algorithm>
#include <iostream>
#include <stdexcept>

ClientParser::ClientParser() {}
ClientParser::~ClientParser() {}

void ClientParser::parse(std::string msg, Client &client)
{
	size_t i = 0;
	std::string prefix = "", command = "", args = "";

	std::cout << "msg : " << msg << std::endl;
	std::vector<std::string> actions = strsplit(msg, '\n');
	for (std::vector<std::string>::iterator it = actions.begin(); it != actions.end(); it++)
	{
		(*it).erase((*it).find_last_not_of("\r\n") + 1);
		if ((*it).length() == 0)
			continue ;
		i = 0;
		std::vector<std::string> splitted_command = strsplit(*it, ' ');
		if (splitted_command[i][0] == ':')
			prefix = splitted_command[i++];
		if (splitted_command.size() > i)
			command = splitted_command[i++];
		for (std::vector<std::string>::iterator it2 = splitted_command.begin() + i; it2 != splitted_command.end(); it2++)
			args += *it2 + ' ';
		if (args.size() > 0)
			args = args.substr(0, args.size() - 1);
		args.erase(args.find_last_not_of("\r\n") + 1);
		this->parse_command(prefix, command, args, client);
		command = "";
		prefix = "";
		args = "";
	}
}

void ClientParser::parse_command(std::string prefix, std::string command, std::string args, Client &client)
{
	std::vector<function> handlers;
	handlers.push_back(function("CAP", &ClientParser::cap));			// tested, OK
	handlers.push_back(function("PASS", &ClientParser::pass));			// tested, OK
	handlers.push_back(function("NICK", &ClientParser::nick));			// tested, KO
	handlers.push_back(function("USER", &ClientParser::user));			// tested, KO
	handlers.push_back(function("QUIT", &ClientParser::quit));			// untested, KO

	handlers.push_back(function("JOIN", &ClientParser::join));			// partially tested, KO
	handlers.push_back(function("PART", &ClientParser::part));			// partially tested, KO
	handlers.push_back(function("INVITE", &ClientParser::invite));		// partially tested, KO
	handlers.push_back(function("KICK", &ClientParser::kick));			// partially tested, KO
	handlers.push_back(function("TOPIC", &ClientParser::topic));		// partially tested, KO
	handlers.push_back(function("MODE", &ClientParser::mode));			// untested, KO

	handlers.push_back(function("PRIVMSG", &ClientParser::privmsg));	// untested, KO

	std::vector<function>::iterator it = std::find(handlers.begin(), handlers.end(), command);
	if (it != handlers.end())
	{
		std::string error;
		if (!client.is_registered() && it - handlers.begin() > 4)
		{
			client.send("", ":ft_irc " + ERR_PASSWDMISMATCH + " * :Please register");
			return ;
		}
		error = (this->*(*it).value)(prefix, args, client); // Calls this->function() based on function pointer to function but still on this instance
		if (error != "")
			client.send("", ":ft_irc " + error);
	}
}

std::string ClientParser::parse_postfix(std::string args)
{
	std::string postfix;

	size_t postfix_start = args.find_last_of(":");
	if (postfix_start == std::string::npos)
		postfix = "";
	else
		postfix = args.substr(args.find_last_of(":"), args.size() - 1);
	return postfix;
}

std::string ClientParser::cap(std::string prefix, std::string args, Client &client)
{
	if (args.size() == 0)
		return (ERR_NEEDMOREPARAMS);
	return client.capabilites(prefix, args);
}

std::string ClientParser::pass(std::string, std::string args, Client &client)
{
	std::string name = client.getName();
	name = " " + (name.size() == 0 ? "*" : name);
	if (args.size() == 0)
	{
		client.send("", ":ft_irc " + ERR_NEEDMOREPARAMS + name + " PASS :Not enough parameters");
		return "";
	}
	return client.auth(args);
}

std::string ClientParser::nick(std::string prefix, std::string args, Client &client)
{
	std::string name = client.getName();
	name = " " + (name.size() == 0 ? "*" : name);
	if (args == "")
		client.send("", ":ft_irc " + ERR_NONICKNAMEGIVEN + name + " :No nickname given");
	if (args.find_first_not_of(AUTHORISED_SET) != std::string::npos)
		client.send("", ":ft_irc " + ERR_ERRONEUSNICKNAME + name + " :Erroneous nickname");
	return client.changeNick(prefix, args);
}

std::string ClientParser::user(std::string prefix, std::string args, Client &client)
{
	std::string name = client.getName();
	name = " " + (name.size() == 0 ? "*" : name);
	if (args.size() == 0)
	{
		client.send("", ":ft_irc " + ERR_NEEDMOREPARAMS + name + " USER :Not enough parameters");
		return "";
	}
	return client.changeUser(prefix, args);
}

// TODO JOIN 0
std::string ClientParser::join(std::string, std::string args, Client &client)
{
	size_t index = 0;
	std::string channels, keys;

	if (args.size() == 0)
	{
		client.send("", ":ft_irc " + ERR_NEEDMOREPARAMS + client.getName() + " JOIN :Not enough parameters");
		return "";
	}
	std::vector<std::string> arguments = strsplit(args, ' ');
	if (arguments.size() >= 1)
		channels = arguments.at(0);
	if (arguments.size() >= 2)
		keys = arguments.at(1);
	std::vector<std::string> channel_list = strsplit(channels, ',');
	std::vector<std::string> key_list = strsplit(keys, ',');
	while (index < channel_list.size() || index < key_list.size())
	{
		std::string channel, key;
		try
		{
			channel = channel_list.at(index);
			key = key_list.at(index);
		}
		catch (const std::out_of_range &e)
		{}
		client.joinChannel(channel, key);
		index++;
	}
	return "";
}

// TODO PART
std::string ClientParser::part(std::string prefix, std::string args, Client &client)
{
	std::vector<std::string> channels;
	std::string reason = parse_postfix(args);
	if (reason != "")
	{
		args.erase(args.find_last_of(":"), reason.size());
		args = args.substr(0, args.size() - 1);
		reason = reason.substr(1, reason.size());
	}
	channels = strsplit(args, ',');
	(void) client;
	(void) prefix;
	return ERR_NEEDMOREPARAMS;
	// return client.removeChannel(prefix, args);
}

std::string ClientParser::invite(std::string, std::string params, Client &client)
{
	std::vector<std::string> args = strsplit(params, ' ');
	if (args.size() < 2)
		return (ERR_NEEDMOREPARAMS);
	return client.inviteToChannel(args[0], args[1]);
}

std::string ClientParser::kick(std::string, std::string args, Client &client)
{
	std::string channel;
	std::string message;
	std::vector<std::string> kicked_users;
	std::vector<std::string> temp;

	message = parse_postfix(args);
	args = args.substr(0, args.size() - message.size());
	if (message == "")
		message = "You have been kicked";
	temp = strsplit(args, ' ');
	if (temp.size() < 2)
		return (ERR_NEEDMOREPARAMS);
	channel = temp[0];
	kicked_users = strsplit(temp[1], ',');
	if (kicked_users.size() == 0)
		return (ERR_NEEDMOREPARAMS);
	std::vector<std::string>::iterator username;
	for (username = kicked_users.begin(); username != kicked_users.end(); username++)
		client.kickChannel(channel, *username, message);
	return "";
}

std::string ClientParser::mode(std::string prefix, std::string args, Client &client)
{
	(void) prefix;
	(void) args;
	(void) client;
	// throw std::logic_error("Unimplemented");
	return "";
}

std::string ClientParser::topic(std::string prefix, std::string args, Client &client)
{
	(void) prefix;
	(void) args;
	(void) client;
	throw std::logic_error("Unimplemented");
	return "";
}

std::string ClientParser::privmsg(std::string prefix, std::string args, Client &client)
{
	return client.sendMessage(prefix, args);
}

std::string ClientParser::quit(std::string prefix, std::string args, Client &client)
{
	return client.quit(prefix, args);
}
