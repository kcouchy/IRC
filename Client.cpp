/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kcouchma <kcouchma@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/26 16:28:03 by kcouchma          #+#    #+#             */
/*   Updated: 2024/06/28 16:02:34 by kcouchma         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.h"
#include "PhoneBook.hpp"
#include "Channel.hpp"
#include <iostream>
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

void Client::parse(std::string msg) const
{
	std::cout << this->m_fd << ": I'm parsing " << msg << std::endl;
}

void Client::send(std::string msg)
{
	ssize_t check;
	check = ::send(m_fd, &msg, msg.size(), 0);
	//TODO secure send using check return value
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
		Channel *c = new Channel(channelName);
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
