/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kcouchma <kcouchma@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/26 16:28:03 by kcouchma          #+#    #+#             */
/*   Updated: 2024/07/16 11:37:07 by kcouchma         ###   ########.fr       */
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
		prefix = "";
		command = "";
		args = "";
	}
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
	handlers.push_back(Pair<std::string, void (Client::*)(std::string, std::string)>("QUIT", &Client::quit));
	handlers.push_back(Pair<std::string, void (Client::*)(std::string, std::string)>("INVITE", &Client::inviteToChannel));
	handlers.push_back(Pair<std::string, void (Client::*)(std::string, std::string)>("CAP", &Client::capabilites));

	std::vector<Pair<std::string, void (Client::*)(std::string, std::string)> >::iterator iter = handlers.begin();
	while (iter != handlers.end())
	{
		if ((*iter).getKey() == command)
			(this->*((*iter).value))(prefix, args); // Calls this->function() based on function pointer to function but still on this instance
		iter++;
	}
}

// Authentication
void Client::capabilites(std::string, std::string params)
{
	(void) params;
	if (params == "LS 302")
	{
		send("CAP * LS :\n");
		return ;
	}
	if (params == "END")
		return ;
	send(":localhost 404 :Not found");
}

void Client::auth(std::string , std::string args)
{
	if (args.size() == 0)
	{
		send(ERR_NEEDMOREPARAMS);
		return ;
	}
	if (m_authenticated)
	{
		send(ERR_ALREADYREGISTERED);
		return ;
	}
	std::cout << "Authenticating with " << args << " against password " << m_password << std::endl;
	if (args == m_password)
		this->m_authenticated = true;
	else
		send(ERR_PASSWDMISMATCH);
	return ;
}

void Client::changeNick(std::string, std::string params)
{
	this->m_name = params;
}

void Client::changeUser(std::string, std::string param)
{
	(void) param;
	this->send(":ft_irc@localhost:6667 001 " + this->getName() + " :Welcome here\n");
}

// Channel
void	Client::addChannel(std::string, std::string channels)
{
	// TODO handle multi-channel join (split channels on ',' and join each splitted channel)
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
// Message-related stuff
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

// I'm outta here (for real this time)
void Client::quit(std::string, std::string)
{
	close(m_fd);
	throw Client::KillMePlease();
}

const char *Client::KillMePlease::what() const throw()
{
	return ("Please delete me");
}

void	Client::topicChannel(std::string, std::string params)
{
	std::vector<std::string> args = strsplit(params, ' ');
	if (args.size() < 1)
	{
		send(ERR_NEEDMOREPARAMS);
		return ;
	}
	try
	{
		Channel* temp_channel;
		temp_channel = dynamic_cast <Channel*> (PhoneBook::get().getRecipient(args[0]));
		if (temp_channel == NULL)
		{
			send(ERR_NOSUCHCHANNEL);
			return ;
		}
		temp_channel->topic(args, m_name);
	}
}