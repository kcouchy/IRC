/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kcouchma <kcouchma@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/16 17:33:15 by aboyreau          #+#    #+#             */
/*   Updated: 2024/07/17 18:47:52 by kcouchma         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Messageable.h"
#include "PhoneBook.hpp"
#include "Channel.hpp"
#include "Client.h"
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
		std::cerr << "Pushing back : " << str.substr(start, stop - start) << std::endl;
		splitted.push_back(str.substr(start, stop - start));
		start = stop + 1;
		std::cerr << "start : " << start << std::endl;
		std::cerr << "searching `" << delim << "` in " << str.substr(start, str.size() - 1) << std::endl;
		stop = str.find(delim, start);
		std::cerr << "stop : " << stop << std::endl;
	}
	return splitted;
}

Client::Client(int client_fd, std::string password) :
	m_fd(client_fd),
	m_password(password),
	m_registrationComplete(false)
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

// Authentication
std::string Client::capabilites(std::string, std::string params)
{
	std::cerr << "`" << params << "`" << std::endl;
	if (params == "LS")
	{
		send("CAP * LS :\n");
		return ("");
	}
	if (params == "END")
		return ("");
	send(":localhost 404 :Not found");
	return ("");
}

std::string Client::auth(std::string password)
{
	if (m_authenticated)
		send(ERR_ALREADYREGISTERED);
	if (password == m_password)
		this->m_authenticated = true;
	else
		send(ERR_PASSWDMISMATCH);
	return ("");
}

std::string Client::changeNick(std::string, std::string params)
{
	if (m_authenticated == false)
		return (ERR_PASSWDMISMATCH);
	std::string authorized_set = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_[]{}\\|";
	if (params == "" || params.find_first_not_of(authorized_set) != std::string::npos)
		return (ERR_ERRONEUSNICKNAME);
	if (PhoneBook::get().getRecipient(params) != NULL)
		return (ERR_NICKNAMEINUSE);
	this->m_name = params;
	return ("");
}

std::string Client::changeUser(std::string, std::string param)
{
	(void) param;
	if (m_authenticated == false)
		return (ERR_PASSWDMISMATCH);
	if (m_name.length() == 0)
		return (ERR_NONICKNAMEGIVEN);
	if (m_registrationComplete == true)
		return (ERR_ALREADYREGISTERED);
	try
	{
		PhoneBook::get().addRecipient(this);
	}
	catch (std::exception &e)
	{
		return (ERR_NICKNAMEINUSE);
	}
	this->send(":ft_irc 001 " + this->getName() + " :Welcome here\n");
	this->m_registrationComplete = true;
	return ("");
}

// Channel
//TODO While a client is joined to a channel, they receive all relevant 
// information about that channel including the JOIN, PART, KICK, and MODE messages
//  affecting the channel. AND THE REST
std::string	Client::addChannel(std::string, std::string params)
{
	// TODO handle multi-channel join (split params on ',' and join each splitted channel)
	std::string topic;
	std::vector<std::string>::iterator iter;
	
	for (iter = m_channelList.begin(); iter != m_channelList.end(); iter++)
		if (params == *iter)
			return "";
	Channel *channel;
	Messageable *c = PhoneBook::get().getRecipient(params);
	if (c == NULL)
	{
		channel = new Channel(params);
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
	m_channelList.push_back(params);
	return ("");
}

std::string	Client::removeChannel(std::string, std::string channelName)
{
	std::vector<std::string>::iterator iter;
	Channel *channel = NULL;

	for (iter = m_channelList.begin(); iter != m_channelList.end(); iter++)
	{
		if (channelName == *iter)
		{
			channel = PhoneBook::get().getChannel(channelName);
			if (channel == NULL)
				continue ;
			try
			{
				channel->quit(this->getName());
			}
			catch(const Channel::EmptyChannel& e)
			{
				delete channel;
			}
			iter = m_channelList.erase(iter);
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
	temp_channel = PhoneBook::get().getChannel(args[1]);
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
	Channel* temp_channel;
	temp_channel = PhoneBook::get().getChannel(args[0]);
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
		std::string topic_return = temp_channel->setTopic(new_topic, m_name);
		if (topic_return != "")
			send(":" + temp_channel->getName() + " " + topic_return);
	}
	return "";
}

std::string	Client::kickChannel(std::string channel_name, std::string client_name, std::string kick_msg)
{
	Channel* temp_channel;
	temp_channel = PhoneBook::get().getChannel(channel_name);
	if (temp_channel == NULL)
	{
		send(":" + temp_channel->getName() + " " + ERR_NOSUCHCHANNEL);
		return ("");
	}
	Client* temp_client;
	temp_client = PhoneBook::get().getClient(client_name);
	if (temp_client == NULL)
	{
		send(":" + temp_channel->getName() + " " + ERR_NOSUCHNICK);
		return ("");
	}
	std::string kick_return = temp_channel->kick(temp_client, m_name);
	if (kick_return != "")
		send(":" + temp_channel->getName() + " " + kick_return);
	else //send kick_msg to kicked client?
	{
		if (kick_msg == "")
			temp_client->send("KICK " + channel_name + " " + client_name + "\n");
		else
			temp_client->send("KICK " + channel_name + " " + client_name + " :" + kick_msg + "\n");
	}
	temp_client->removeChannel("", channel_name);
	return ("");
}
