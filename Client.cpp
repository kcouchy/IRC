/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kcouchma <kcouchma@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/26 16:28:03 by kcouchma          #+#    #+#             */
/*   Updated: 2024/07/15 18:15:23 by kcouchma         ###   ########.fr       */
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
#include <vector>

Client::Client(int client_fd) :
	m_fd(client_fd) {}

Client::~Client(void)
{
	std::cout << "Oh no I'm dying" << this->getfd() << std::endl;
}

int Client::getfd()const
{
	return (this->m_fd);
}

std::vector<std::string>	strsplit(std::string str, char delim)
{
	std::vector<std::string> splitted;
	size_t start = 0;
	size_t stop;

	std::cout << "str : " << str << std::endl << "delim : `" << delim << "`" << std::endl;
	stop = str.find(delim);
	while (1)
	{
		std::cout << "start : " << start << " stop : " << stop << std::endl;
		if (stop == std::string::npos)
		{
			splitted.push_back(str.substr(start, str.size()));
			break ;
		}
		splitted.push_back(str.substr(start, stop));
		start = stop + 1;
		stop = str.find(start, delim);
	}
	return splitted;
}

void Client::parse(std::string msg)
{
	std::string prefix = "", command = "", args = "";

	std::vector<std::string> actions = strsplit(msg, '\n');
	for (std::vector<std::string>::iterator it = actions.begin(); it != actions.end(); it++)
	{
		std::cout << "command : " << *it << std::endl;
		if ((*it).length() == 0)
			continue ;
		prefix = std::strtok((char *) (*it).c_str(), " ");
		if (prefix[0] != ':')
		{
			command = prefix;
			prefix = "";
		}
		else
			command = std::strtok(NULL, " ");
		args = std::strtok(NULL, "");
		std::cout << "Prefix : " << prefix << ", command : " << command << ", args : " << args << std::endl;
		this->exec(prefix, command, args);
	}
	std::cout << "End of parse" << std::endl;
}

// TODO
void Client::auth(std::string , std::string args)
{
	if (args == "test")
		std::cout << "authentication success" << std::endl;
	else
		std::cout << "authentication failure" << std::endl;
}

void Client::changeNick(std::string, std::string params)
{
	this->m_name = params;
}

void Client::sendMessage(std::string, std::string params)
{
	std::string msg;
	std::vector<std::string>::iterator it;
	std::vector<std::string> args = strsplit(params, ' ');
	std::cout << "Args 0 : " << *args.begin() << std::endl;
	std::vector<std::string> recipients = strsplit((*args.begin()), ',');
	for (it = recipients.begin(); it != recipients.end(); it++)
	{
		std::string recipient = *it;
		std::cout << "Recipient : " << recipient << std::endl;
		if (recipient.size() == 0)
		{
			::send(this->m_fd, ERR_NORECIPIENT, 3, 0);
			continue ;
		}
		Messageable *m = PhoneBook::get().getRecipient(recipient);
		if (m == NULL)
		{
			if (recipient.at(0) == '#')
				::send(this->m_fd, ERR_CANNOTSENDTOCHAN, 3, 0);
			else
				::send(this->m_fd, ERR_NOSUCHNICK, 3, 0);
			continue ;
		}
	
		std::string msg = "";
		std::vector<std::string>::iterator it2 = ++args.begin();
		for (; it2 != args.end(); it2++)
			msg += *it2 + ((it2 == --args.end()) ? '\0' : ' ');
		m->send(msg);
	}
}

void Client::changeUser(std::string, std::string param)
{
	std::cout << "Change user : " << param << std::endl;
	this->send("001 " + this->getName() + ":Welcome");
}

void Client::exec(std::string prefix, std::string command, std::string args)
{
	std::vector<Pair<std::string, void (Client::*)(std::string, std::string)> > handlers;
	handlers.push_back(Pair<std::string, void (Client::*)(std::string, std::string)>("PASS", &Client::auth));
	handlers.push_back(Pair<std::string, void (Client::*)(std::string, std::string)>("JOIN", &Client::addChannel));
	handlers.push_back(Pair<std::string, void (Client::*)(std::string, std::string)>("PRIVMSG", &Client::sendMessage));
	handlers.push_back(Pair<std::string, void (Client::*)(std::string, std::string)>("NICK", &Client::changeNick));
	handlers.push_back(Pair<std::string, void (Client::*)(std::string, std::string)>("USER", &Client::changeUser));
	handlers.push_back(Pair<std::string, void (Client::*)(std::string, std::string)>("LEAVE", &Client::removeChannel));
	handlers.push_back(Pair<std::string, void (Client::*)(std::string, std::string)>("INVITE", &Client::inviteToChannel));

	std::vector<Pair<std::string, void (Client::*)(std::string, std::string)> >::iterator iter = handlers.begin();
	while (iter != handlers.end())
	{
		if ((*iter).getKey() == command)
			(this->*((*iter).value))(prefix, args); // Calls this->function() based on function pointer to function but still on this instance
		iter++;
	}
}

void Client::send(std::string msg)
{
	::send(m_fd, msg.c_str(), msg.size(), 0);
	// TODO secure send using check return value
	return ;
}

// TODO handle multi-channel join (split channels on ',' and join each splitted channel)
void	Client::addChannel(std::string, std::string channels)
{
	std::string topic;
	std::vector<std::string>::iterator iter;
	
	for (iter = m_channelList.begin(); iter != m_channelList.end(); iter++)
		if (channels== *iter)
			return;
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
			return ;
		}
	}
	channel->join(this->getName());
	m_channelList.push_back(channels);
}

void	Client::removeChannel(std::string, std::string channelName)
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
}

void	Client::inviteToChannel(std::string, std::string params)
{
	std::vector<std::string> args = strsplit(params, ' ');
	if (args.size() < 2)
	{
		send(ERR_NEEDMOREPARAMS);
		return ;
	}
	//TODO MAYBE also reject if >2 args
	try
	{
		Channel* temp_channel;
		temp_channel = dynamic_cast <Channel*> (PhoneBook::get().getRecipient(args[1]));
		if (temp_channel == NULL)
		{
			send(ERR_NOSUCHCHANNEL);
			return ;
		}
		temp_channel->invite(m_name, args[0]);
	}
	catch (Channel::UserOnChannel& e)
	{
		send(ERR_USERONCHANNEL);
	}
	catch (Channel::NotOnChannel& e)
	{
		send(ERR_NOTONCHANNEL);
	}
	catch (Channel::NotOperator& e)
	{
		send(ERR_CHANOPRIVSNEEDED);
	}
	Messageable* temp_client;
	temp_client = PhoneBook::get().getRecipient(args[0]);
	if (temp_client == NULL)
		return ;
	send(RPL_INVITING);
	std::string msg = m_name + ": has invited you to " + args[1];
	temp_client->send(msg);
}