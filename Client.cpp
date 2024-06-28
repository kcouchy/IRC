/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kcouchma <kcouchma@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/26 16:28:03 by kcouchma          #+#    #+#             */
/*   Updated: 2024/06/28 22:57:09 by aboyreau         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.h"
#include "PhoneBook.hpp"
#include "Channel.hpp"
#include <cstring>
#include <iostream>
#include <string>
#include <sys/socket.h>

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

void Client::parse(std::string msg)
{
	std::string prefix = "", command = "", args = "";

	if (msg.size() == 0 || msg[0] == '\n' || msg.at(msg.size() - 1) != '\n')
		return ;
	msg = msg.substr(0, msg.size() - 1);
	prefix = std::strtok((char *) msg.c_str(), " ");
	if (prefix[0] != ':')
	{
		command = prefix;
		prefix = "";
	}
	else
		command = std::strtok(NULL, " ");
	args = std::strtok(NULL, "\0");
	this->exec(prefix, command, args);
	std::cout << "Prefix : " << prefix << ", command : " << command << ", args : " << args << std::endl;
}

void auth(std::string prefix, std::string args)
{
	if (args == "test")
		std::cout << "authentication success" << std::endl;
	else
		std::cout << "authentication failure" << std::endl;

	(void) prefix;
	(void) args;
}

void Client::exec(std::string prefix, std::string command, std::string args)
{
	std::list<Pair<std::string, void (*)(std::string, std::string)> > handlers;
	handlers.push_back(Pair<std::string, void (*)(std::string, std::string)>("PASS", &auth));

	std::cout << command << std::endl;
	std::list<Pair<std::string, void (*)(std::string, std::string)> >::iterator iter = handlers.begin();
	while (iter != handlers.end())
	{
		if ((*iter).getKey() == command)
			(*iter).value(prefix, args);
		iter++;
	}

}

void Client::send(std::string msg)
{
	::send(m_fd, &msg, msg.size(), 0);
	// TODO secure send using check return value
	return ;
}

void	Client::addChannel(std::string channelName)
{
	std::string topic;
	std::list<std::string>::iterator iter;
	
	for (iter = m_channelList.begin(); iter != m_channelList.end(); iter++)
		if (channelName == *iter)
			return;
	m_channelList.push_back(channelName);
	if (PhoneBook::get().getRecipient(channelName) == NULL)
		new Channel(channelName);
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
