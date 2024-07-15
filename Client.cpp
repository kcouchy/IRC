/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                                +**+   +*  *   */
/*                                                    +:+ +:+         +:+     */
/*   By: kcouchma <kcouchma@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/26 16:28:03 by kcouchma          #+#    #+#             */
/*   Updated: 2024/07/05 15:48:51 by aboyreau          +#-.-*  +         *    */
/*                                                                            */
/* ************************************************************************** */

#include "Client.h"
#include "Messageable.h"
#include "PhoneBook.hpp"
#include "Channel.hpp"
#include <cstring>
#include <iostream>
#include <string>
#include <sys/socket.h>
#include <list>

Client::Client(int client_fd) :
	m_fd(client_fd) {}

Client::~Client(void)
{
	std::cout << "Oh no I'm dying" << this->getfd() << std::endl;
}

int Client::getfd()const
{
	return(this->m_fd);
}

std::list<std::string>	strsplit(std::string str, char delim)
{
	std::list<std::string> splitted;
	size_t start = 0;
	size_t stop;

	std::cout << "str : " << str << std::endl << "delim : " << delim << std::endl;
	stop = str.find('\n');
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
		stop = str.find(start, '\n');
	}
	return splitted;
}

void Client::parse(std::string msg)
{
	std::string prefix = "", command = "", args = "";

	std::list<std::string> actions = strsplit(msg, '\n');
	for (std::list<std::string>::iterator it = actions.begin(); it != actions.end(); it++)
	{
		std::cout << "command : " << *it << std::endl;
		prefix = std::strtok((char *) (*it).c_str(), " ");
		if (prefix[0] != ':')
		{
			command = prefix;
			prefix = "";
		}
		else
			command = std::strtok(NULL, " ");
		args = std::strtok(NULL, "\0");
		std::cout << "Prefix : " << prefix << ", command : " << command << ", args : " << args << std::endl;
		// this->exec(prefix, command, args);
	}
	std::cout << "End of parse" << std::endl;
}

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
	std::string dest = params.substr(0, params.find(' '));
	std::string msg = params.substr(params.find(' ') + 1, params.find(params.find(' ') + 1, ' '));

	std::cout << "msg : " << msg << std::endl;
	Messageable *m = PhoneBook::get().getRecipient(dest);
	m->send(msg);
}

void Client::changeUser(std::string, std::string param)
{
	std::cout << "Change user : " << param << std::endl;
	this->send("001 " + this->getName() + ":Welcome");
}

void Client::exec(std::string prefix, std::string command, std::string args)
{
	std::list<Pair<std::string, void (Client::*)(std::string, std::string)> > handlers;
	handlers.push_back(Pair<std::string, void (Client::*)(std::string, std::string)>("PASS", &Client::auth));
	handlers.push_back(Pair<std::string, void (Client::*)(std::string, std::string)>("JOIN", &Client::addChannel));
	handlers.push_back(Pair<std::string, void (Client::*)(std::string, std::string)>("SEND", &Client::sendMessage));
	handlers.push_back(Pair<std::string, void (Client::*)(std::string, std::string)>("NICK", &Client::changeNick));
	handlers.push_back(Pair<std::string, void (Client::*)(std::string, std::string)>("USER", &Client::changeUser));

	std::cout << "I'm trying to execute " << command << " " << args << std::endl;
	std::list<Pair<std::string, void (Client::*)(std::string, std::string)> >::iterator iter = handlers.begin();
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
	std::list<std::string>::iterator iter;
	
	for (iter = m_channelList.begin(); iter != m_channelList.end(); iter++)
		if (channels== *iter)
			return;
	Channel *channel;
	Messageable *c = PhoneBook::get().getRecipient(channels);
	if (c == NULL)
		channel = new Channel(channels);
	else
		channel = reinterpret_cast<Channel *>(c);
	channel->join(this->getName());
	m_channelList.push_back(channels);
}

void	Client::removeChannel(std::string channelName)
{
	std::list<std::string>::iterator iter;
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
		catch(const std::exception& e)
		{
			delete channel;
		}
	}
}
