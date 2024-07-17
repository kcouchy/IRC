/* ************************************************************************** */
/*                                                                            */
/*                                                              ++            */
/*   ClientParser.cpp                                          +**+   +*  *   */
/*                                                             ##%#*###*+++   */
/*   By: aboyreau <bnzlvosnb@mozmail.com>                     +**+ -- ##+     */
/*                                                            # *   *. #*     */
/*   Created: 2024/07/17 11:59:26 by aboyreau          **+*+  * -_._-   #+    */
/*   Updated: 2024/07/17 14:17:09 by aboyreau          +#-.-*  +         *    */
/*                                                     *-.. *   ++       #    */
/* ************************************************************************** */

#include "ClientParser.h"
#include "Messageable.h"
#include "utils.h"
#include <iostream>

ClientParser::ClientParser() {}
ClientParser::~ClientParser() {}

void ClientParser::parse(std::string msg, Client &client)
{
	size_t i = 0;
	std::string prefix = "", command = "", args = "";

	std::vector<std::string> actions = strsplit(msg, '\n');
	for (std::vector<std::string>::iterator it = actions.begin(); it != actions.end(); it++)
	{
		(*it).erase((*it).find_last_not_of("\n") + 1);
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
		args.erase(args.find_last_not_of("\n\r") + 1);
		std::cout << "I'm executing " << command << " " << args << std::endl;
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
	// handlers.push_back(function("PASS", &ClientParser::auth));				// tested, OK
	// handlers.push_back(function("NICK", &ClientParser::changeNick));			// tested, KO
	// handlers.push_back(function("USER", &ClientParser::changeUser));			// tested, KO

	// handlers.push_back(function("JOIN", &ClientParser::addChannel));			// untested, KO
	// handlers.push_back(function("PART", &ClientParser::removeChannel));		// untested, KO
	// handlers.push_back(function("INVITE", &ClientParser::inviteToChannel));	// untested, KO
	handlers.push_back(function("KICK", &ClientParser::kick));	// untested, KO

	// handlers.push_back(function("PRIVMSG", &ClientParser::sendMessage));		// untested, KO
	// handlers.push_back(function("QUIT", &ClientParser::quit));				// untested, KO

	// TODO KICK
	// TODO INVITE
	// TODO TOPIC
	// TODO MODE

	std::vector<function>::iterator iter = handlers.begin();
	while (iter != handlers.end())
	{
		if ((*iter).getKey() == command)
		{
			std::string error;
			error = (this->*((*iter).value))(prefix, args, client); // Calls this->function() based on function pointer to function but still on this instance
			if (error != "")
			{
				; // send(":ft_irc " + error);
			}
		}
		iter++;
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
		client.kickChannel(channel, *it, message); // TODO
	return "";
}

std::string ClientParser::cap(std::string prefix, std::string args, Client &client)
{
	if (args.size() == 0)
		return (ERR_NEEDMOREPARAMS);
	(void) prefix;
	(void) client;
	return "";
}
