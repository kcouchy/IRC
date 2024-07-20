/* ************************************************************************** */
/*                                                                            */
/*                                                              ++            */
/*   ClientParser.cpp                                          +**+   +*  *   */
/*                                                             ##%#*###*+++   */
/*   By: aboyreau <bnzlvosnb@mozmail.com>                     +**+ -- ##+     */
/*                                                            # *   *. #*     */
/*   Created: 2024/07/17 11:59:26 by aboyreau          **+*+  * -_._-   #+    */
/*   Updated: 2024/07/20 18:29:26 by aboyreau          +#-.-*  +         *    */
/*                                                     *-.. *   ++       #    */
/* ************************************************************************** */

#include "ClientParser.h"
#include "Messageable.h"
#include "utils.h"
#include <algorithm>
#include <iostream>

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

	handlers.push_back(function("JOIN", &ClientParser::join));			// untested, KO
	handlers.push_back(function("PART", &ClientParser::part));			// untested, KO
	handlers.push_back(function("INVITE", &ClientParser::invite));		// untested, KO
	handlers.push_back(function("KICK", &ClientParser::kick));			// untested, KO
	// handlers.push_back(function("TOPIC", &ClientParser::topic));		// untested, KO
	// handlers.push_back(function("MODE", &ClientParser::mode));			// untested, KO

	handlers.push_back(function("PRIVMSG", &ClientParser::privmsg));	// untested, KO
	handlers.push_back(function("QUIT", &ClientParser::quit));			// untested, KO

	std::vector<function>::iterator it = std::find(handlers.begin(), handlers.end(), command);
	if (it != handlers.end())
	{
		std::string error;
		error = (this->*(*it).value)(prefix, args, client); // Calls this->function() based on function pointer to function but still on this instance
		if (error != "")
		{
			client.send("", ":ft_irc " + error);
		}
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

std::string ClientParser::pass(std::string prefix, std::string args, Client &client)
{
	(void) prefix;
	if (args.size() == 0)
	{
		client.send("", ":ft_irc 461 * PASS :Not enough parameters");
		return "";
	}
	return client.auth(args);
}

std::string ClientParser::nick(std::string prefix, std::string args, Client &client)
{
	return client.changeNick(prefix, args);
}

std::string ClientParser::user(std::string prefix, std::string args, Client &client)
{
	return client.changeUser(prefix, args);
}

std::string ClientParser::join(std::string prefix, std::string args, Client &client)
{
	return client.addChannel(prefix, args);
}

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

std::string ClientParser::invite(std::string prefix, std::string args, Client &client)
{
	return client.inviteToChannel(prefix, args);
}

std::string ClientParser::kick(std::string prefix, std::string args, Client &client)
{
	(void) prefix;
	(void) args;
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
	std::vector<std::string>::iterator it;
	for (it = kicked_users.begin(); it != kicked_users.end(); it++)
		client.kickChannel(channel, *it, message);
	return "";
}

// std::string ClientParser::mode(std::string prefix, std::string args, Client &client) {return "";}
// std::string ClientParser::topic(std::string prefix, std::string args, Client &client) {return "";}

std::string ClientParser::privmsg(std::string prefix, std::string args, Client &client)
{
	return client.sendMessage(prefix, args);
}

std::string ClientParser::quit(std::string prefix, std::string args, Client &client)
{
	return client.quit(prefix, args);
}
