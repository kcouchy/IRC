/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kcouchma <kcouchma@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/26 16:28:03 by kcouchma          #+#    #+#             */
/*   Updated: 2024/07/16 16:46:03 by kcouchma         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.h"
#include "Messageable.h"
#include "PhoneBook.hpp"
#include "Channel.hpp"
#include <cstdio>
#include <cstring>
#include <iostream>
#include <string>
#include <sys/socket.h>
#include <unistd.h>
#include <vector>

std::vector<std::string> strsplit(std::string str, char delim)
{
	std::vector<std::string> splitted;
	size_t start = 0;
	size_t stop;

	stop = str.find(delim);
	while (1)
	{
		if (stop == std::string::npos)
		{
			splitted.push_back(str.substr(start, str.size()));
			break ;
		}
		splitted.push_back(str.substr(start, stop - start));
		start = stop + 1;
		stop = str.find(start, delim);
	}
	return splitted;
}

Client::Client(int client_fd, std::string password) :
	m_fd(client_fd),
	m_password(password)
{
	m_authenticated = m_password.size() == 0; // Authenticated by default if password is empty
}

Client::~Client(void)
{
	std::cout << "Oh no I'm dying" << this->getfd() << std::endl;
}

int Client::getfd()const
{
	return (this->m_fd);
}

void Client::send(std::string msg)
{
	std::cerr << this->m_name << " <- " << msg << std::endl;
	::send(m_fd, msg.c_str(), msg.size(), 0);
	// TODO secure send using check return value
	return ;
}

void Client::parse(std::string msg)
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
			args = args.substr(0, args.size() - 2);
		std::cout << "I'm executing " << command << " " << args << std::endl;
		this->exec(prefix, command, args);
		command = "";
		prefix = "";
		args = "";
	}
}

void Client::exec(std::string prefix, std::string command, std::string args)
{
	std::vector<Pair<std::string, std::string (Client::*)(std::string, std::string)> > handlers;
	handlers.push_back(Pair<std::string, std::string (Client::*)(std::string, std::string)>("PASS", &Client::auth));
	handlers.push_back(Pair<std::string, std::string (Client::*)(std::string, std::string)>("JOIN", &Client::addChannel));
	handlers.push_back(Pair<std::string, std::string (Client::*)(std::string, std::string)>("PRIVMSG", &Client::sendMessage));
	handlers.push_back(Pair<std::string, std::string (Client::*)(std::string, std::string)>("NICK", &Client::changeNick));
	handlers.push_back(Pair<std::string, std::string (Client::*)(std::string, std::string)>("USER", &Client::changeUser));
	handlers.push_back(Pair<std::string, std::string (Client::*)(std::string, std::string)>("LEAVE", &Client::removeChannel));
	handlers.push_back(Pair<std::string, std::string (Client::*)(std::string, std::string)>("QUIT", &Client::quit));
	handlers.push_back(Pair<std::string, std::string (Client::*)(std::string, std::string)>("INVITE", &Client::inviteToChannel));
	handlers.push_back(Pair<std::string, std::string (Client::*)(std::string, std::string)>("CAP", &Client::capabilites));

	std::vector<Pair<std::string, std::string (Client::*)(std::string, std::string)> >::iterator iter = handlers.begin();
	while (iter != handlers.end())
	{
		if ((*iter).getKey() == command)
			(this->*((*iter).value))(prefix, args); // Calls this->function() based on function pointer to function but still on this instance
		iter++;
	}
}

// Authentication
std::string Client::capabilites(std::string, std::string params)
{
	std::cerr << "`" << params << "`" << std::endl;
	if (params == "LS 302")
	{
		send("CAP * LS :\n");
		return ("");
	}
	if (params == "END")
		return ("");
	send(":localhost 404 :Not found");
	return ("");
}

std::string Client::auth(std::string , std::string args)
{
	if (args.size() == 0)
		return (ERR_NEEDMOREPARAMS);
	if (m_authenticated)
		return (ERR_ALREADYREGISTERED);
	std::cout << "Authenticating with " << args << " against password " << m_password << std::endl;
	if (args == m_password)
		this->m_authenticated = true;
	else
		return (ERR_PASSWDMISMATCH);
	return ("");
}

std::string Client::changeNick(std::string, std::string params)
{
	std::string authorized_set = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_";
	if (params.find_first_not_of(authorized_set) != std::string::npos)
	{
		return (ERR_ERRONEUSNICKNAME);
	}
	this->m_name = params;
	return ("");
}

std::string Client::changeUser(std::string, std::string param)
{
	(void) param;
	this->send(":ft_irc 001 " + this->getName() + " :Welcome here\n");
	return ("");
}

// Channel
std::string	Client::addChannel(std::string, std::string channels)
{
	// TODO handle multi-channel join (split channels on ',' and join each splitted channel)
	std::string topic;
	std::vector<std::string>::iterator iter;
	
	for (iter = m_channelList.begin(); iter != m_channelList.end(); iter++)
		if (channels == *iter)
			return "";
	Channel *channel;
	Messageable *c = PhoneBook::get().getRecipient(channels);
	if (c == NULL)
	{
		channel = new Channel(channels);
	}
	else
	{
		channel = dynamic_cast<Channel *>(c);
		std::cout << c->getName() << std::endl;
		if (channel == NULL)
		{
			// TODO return an error code to the client
			::send(this->m_fd, ERR_NOSUCHCHANNEL, 3, 0);
			return ("");
		}
	}
	channel->join(this->getName());
	m_channelList.push_back(channels);
	return ("");
}

std::string	Client::removeChannel(std::string, std::string channelName)
{
	std::vector<std::string>::iterator iter;
	Messageable *channel = NULL;

	for (iter = m_channelList.begin(); iter != m_channelList.end(); iter++)
	{
		if (channelName == *iter)
			iter = m_channelList.erase(iter);
		channel = PhoneBook::get().getRecipient(channelName);
		try
		{
			reinterpret_cast<Channel *>(channel)->quit(this->getName());
		}
		catch(const Channel::EmptyChannel& e)
		{
			delete channel;
		}
	}
	return ("");
}

std::string	Client::inviteToChannel(std::string, std::string params)
{
	std::vector<std::string> args = strsplit(params, ' ');
	if (args.size() < 2)
		return (ERR_NEEDMOREPARAMS);
	//TODO MAYBE also reject if >2 args
	std::string invite_return;
	Channel* temp_channel;
	temp_channel = dynamic_cast <Channel*> (PhoneBook::get().getRecipient(args[1]));
	if (temp_channel == NULL)
		return (ERR_NOSUCHCHANNEL);
	invite_return = temp_channel->invite(m_name, args[0]);
	if (invite_return != "")
		send (invite_return);
	Messageable* temp_client;
	temp_client = PhoneBook::get().getRecipient(args[0]);
	if (temp_client == NULL)
		return "";
	send(RPL_INVITING);
	std::string msg = m_name + ": has invited you to " + args[1];
	temp_client->send(msg);
	return "";
}

// Message-related stuff
std::string Client::sendMessage(std::string, std::string params)
{
	std::string msg;
	std::vector<std::string>::iterator it;
	std::vector<std::string> args = strsplit(params, ' ');
	std::cout << "Args 0 : " << *args.begin() << std::endl;
	std::vector<std::string> recipients = strsplit((*args.begin()), ',');
	for (it = recipients.begin(); it != recipients.end(); it++)
	{
		std::string recipient = *it;
		if (recipient.size() == 0)
		{
			send(ERR_NORECIPIENT);
			continue ;
		}
		Messageable *m = PhoneBook::get().getRecipient(recipient);
		if (m == NULL)
		{
			if (recipient.at(0) == '#')
				send(ERR_CANNOTSENDTOCHAN);
			else
				send(ERR_NOSUCHNICK);
			continue ;
		}
	
		std::string msg = ":" + m_name + " PRIVMSG " + recipient + " ";
		std::vector<std::string>::iterator it2 = ++args.begin();
		for (; it2 != args.end(); it2++)
			msg += *it2 + ' ';
		msg = msg.substr(0, msg.size() - 1);
		msg += "\n";
		m->send(msg);
	}
	return ("");
}

// I'm outta here (for real this time)
std::string Client::quit(std::string, std::string)
{
	close(m_fd);
	throw Client::KillMePlease();
	return ("");
}

const char *Client::KillMePlease::what() const throw()
{
	return ("Please delete me");
}

std::string	Client::topicChannel(std::string, std::string params)
{
	std::vector<std::string> args = strsplit(params, ' ');
	if (args.size() < 1)
	{
		send(ERR_NEEDMOREPARAMS);
		return ("");
	}

	// try handle empty topic here, then split by ';' and send to topic method
	// add a messageable method to get channel or return null
	{
		Channel* temp_channel;
		temp_channel = dynamic_cast <Channel*> (PhoneBook::get().getRecipient(args[0]));
		if (temp_channel == NULL)
		{
			send(ERR_NOSUCHCHANNEL);
			return ("");
		}
		if (args.size() == 1)
		{
			std::string temp_topic = temp_channel->getTopic();
			if (temp_topic == "")
				send(":" + temp_channel->getName() + " " + RPL_NOTOPIC);
			else
				send(":" + temp_channel->getName() + " " + RPL_TOPIC + " :"  + temp_topic + "\n");
		}
		else
		{
			if (args[1][0] != ':')
			{
				send(":" + temp_channel->getName() + " " + ERR_UNKNOWNERROR);
				return ("");
			}
			std::string new_topic = args[1].substr(1, args[1].size() - 1);
			std::vector<std::string>::iterator iter = args.begin() + 2;
			for (; iter != args.end(); iter++)
				new_topic += " " + *iter;
			temp_channel->setTopic(new_topic);
		}
	}
}