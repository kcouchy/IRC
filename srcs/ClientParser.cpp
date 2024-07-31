/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ClientParser.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kcouchma <kcouchma@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/17 11:59:26 by aboyreau          #+#    #+#             */
/*   Updated: 2024/07/31 19:41:58 by kcouchma         ###   ########.fr       */
/*                                                                            */
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

	msg = client.getBuffer() + msg;
	client.clearBuffer();
	std::cout << "msg : " << msg << std::endl;
	bool	end_nl = msg.find_last_of("\n") == msg.size() - 1;

	std::vector<std::string> actions = strsplit(msg, '\n');
	for (std::vector<std::string>::iterator it = actions.begin(); it != actions.end(); it++)
	{
		if (end_nl == false && (*it).at((*it).size() - 1) != '\r')
		{
			std::cout << "-----" << (*it) << "-------" << std::endl;
			client.addBuffer(*it);
			return ;
		}
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
	std::vector<function> handlers; //TODO - delete testing notes
	handlers.push_back(function("CAP", &ClientParser::cap));			// tested, OK
	handlers.push_back(function("PASS", &ClientParser::pass));			// tested, OK
	handlers.push_back(function("NICK", &ClientParser::nick));			// tested, KO
	handlers.push_back(function("USER", &ClientParser::user));			// tested, KO
	handlers.push_back(function("QUIT", &ClientParser::quit));			// untested, KO

	handlers.push_back(function("JOIN", &ClientParser::join));			// partially tested, KO
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
			client.send("", ":ft_irc " + ERR_NOTREGISTERED + " * :You have not registered");
			return ;
		}
		error = (this->*(*it).value)(prefix, args, client);
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
		client.send("", ":ft_irc " + ERR_NEEDMOREPARAMS + " " + client.getName() + " :Not enough parameters");
	return client.capabilites(prefix, args);
}

std::string ClientParser::pass(std::string, std::string args, Client &client)
{
	std::string name = client.getName();
	name = " " + (name.size() == 0 ? "*" : name);
	if (args.size() == 0)
	{
		client.send("", ":ft_irc " + ERR_NEEDMOREPARAMS +
			name + " PASS :Not enough parameters");
		return "";
	}
	return client.auth(args);
}

std::string ClientParser::nick(std::string prefix, std::string args, Client &client)
{
	std::string name = client.getName();
	name = " " + (name.size() == 0 ? "*" : name);
	if (args == "")
		client.send("", ":ft_irc " + ERR_NONICKNAMEGIVEN +
			name + " :No nickname given");
	if (args.find_first_not_of(AUTHORISED_SET) != std::string::npos)
		client.send("", ":ft_irc " + ERR_ERRONEUSNICKNAME +
			name + " :Erroneous nickname");
	return client.changeNick(prefix, args);
}

std::string ClientParser::user(std::string prefix, std::string args, Client &client)
{
	std::string name = client.getName();
	name = " " + (name.size() == 0 ? "*" : name);
	if (args.size() == 0)
	{
		client.send("", ":ft_irc " + ERR_NEEDMOREPARAMS +
			name + " USER :Not enough parameters");
		return "";
	}
	return client.changeUser(prefix, args);
}

std::string ClientParser::join(std::string, std::string args, Client &client)
{
	size_t index = 0;
	std::string channels, keys;

	if (args.size() == 0)
	{
		client.send("", ":ft_irc " + ERR_NEEDMOREPARAMS +
			client.getName() + " JOIN :Not enough parameters");
		return "";
	}
	std::vector<std::string> split_args = strsplit(args, ' ');
	if (split_args.size() >= 1)
	{
		if (split_args[0] == "0")
		{
			client.removeAllChannels();
			return "";
		}
		channels = split_args.at(0);
	}
	if (split_args.size() >= 2)
		keys = split_args.at(1);
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

std::string ClientParser::invite(std::string, std::string params, Client &client)
{
	std::vector<std::string> args = strsplit(params, ' ');
	if (args.size() < 2)
	{
		client.send("", ":ft_irc " + ERR_NEEDMOREPARAMS + " " +
			client.getName() +
			" INVITE :Not enough parameters");
		return "";
	}
	return client.inviteToChannel(args[0], args[1]);
}

std::string ClientParser::kick(std::string, std::string args, Client &client)
{
	std::string channel;
	std::string message;
	std::vector<std::string> kicked_users;
	std::vector<std::string> temp;

	std::cout << "HI" << std::endl;

	message = parse_postfix(args);
	args = args.substr(0, args.size() - message.size());
	if (message == "")
		message = "You have been kicked";
	temp = strsplit(args, ' ');
	if (temp.size() < 2)
	{
		client.send("", ":ft_irc " + ERR_NEEDMOREPARAMS + " " +
			client.getName() + " " + 
			"KICK :Not enough parameters");
		return "";
	}
	channel = temp[0];
	kicked_users = strsplit(temp[1], ',');
	if (kicked_users.size() == 0)
	{
		client.send("", ":ft_irc " + ERR_NEEDMOREPARAMS + " " +
			client.getName() + " " + 
			"KICK :Not enough parameters");
		return "";
	}
	std::vector<std::string>::iterator username;
	for (username = kicked_users.begin(); username != kicked_users.end(); username++)
		client.kickChannel(channel, *username, message);
	return "";
}

std::string ClientParser::mode(std::string, std::string args, Client &client)
{
	std::vector<std::string> split_args = strsplit(args, ' ');
	bool plusminus = false;

	if (split_args.size() == 0)
		client.send("", ":ft_irc " + ERR_NEEDMOREPARAMS + " " +
			client.getName() + " " + 
			"MODE :Not enough parameters");
	if (split_args.size() == 1)
	{
		client.getMode(split_args[0]);
		return "";
	}

	std::string modes = split_args[1];
	std::string::iterator mode_it = modes.begin();
	std::vector<std::string>::iterator arg_it = split_args.end();
	if (split_args.size() > 2)
		arg_it = split_args.begin() + 2;
	for (; mode_it != modes.end() ; mode_it++)
	{
		if (*mode_it == '-' || *mode_it == '+')
		{
			plusminus = (*mode_it == '-' ? false : true);
			continue ;
		}
		if (TYPE_B.find(*mode_it) != std::string::npos && arg_it != split_args.end())
		{
			client.modeChannel(split_args[0], plusminus, *mode_it, *arg_it);
			arg_it++;
		}
		if (TYPE_C.find(*mode_it) != std::string::npos)
		{
			if (plusminus == true && arg_it != split_args.end())
			{
				client.modeChannel(split_args[0], plusminus, *mode_it, *arg_it);
				arg_it++;
			}
			else if (plusminus == false)
			{
				client.modeChannel(split_args[0], plusminus, *mode_it, "");
			}
		}
		if (TYPE_D.find(*mode_it) != std::string::npos)
		{
			client.modeChannel(split_args[0], plusminus, *mode_it, "");
		}
	}
	return "";
}

std::string ClientParser::topic(std::string, std::string params, Client &client)
{
	bool changeTopic = false;
	std::string topic = parse_postfix(params);
	std::string channel_name;
	if (topic.size() != 0)
	{
		changeTopic = true;
		topic = topic.substr(1, topic.size() - 1);
		channel_name = params.erase(params.find(":") - 1, params.size() - 1);
	}
	else
	{
		channel_name = params;
	}
	if (channel_name.size() == 0)
	{
		client.send("", ":ft_irc " + ERR_NEEDMOREPARAMS + " " +
			client.getName() +
			" TOPIC :Not enough parameters");
		return ("");
	}
	if (!changeTopic)
		client.topicChannel(channel_name);
	else
		client.topicChannel(channel_name, topic);
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
