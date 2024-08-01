/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lribette <lribette@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/16 17:33:15 by aboyreau          #+#    #+#             */
/*   Updated: 2024/08/01 16:31:20 by kcouchma         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Messageable.h"
#include "PhoneBook.hpp"
#include "Channel.hpp"
#include "Client.h"
#include <cstdio>
#include <cstring>
#include <iostream>
#include <stdexcept>
#include <string>
#include <sys/socket.h>
#include <unistd.h>
#include <list>
#include "Utils.h"

Client::Client(int client_fd, std::string password) :
	m_fd(client_fd),
	m_password(password),
	m_channelList(),
	m_registrationComplete(false)
{
	m_authenticated = m_password.size() == 0; // Authenticated by default if password is empty
}

Client::~Client(void)
{
	std::cout << "Oh no I'm dying" << this->getfd() << std::endl;
	std::list<std::string>::iterator it;
	for (it = m_channelList.begin(); it != m_channelList.end();)
	{
		Channel *channel = PhoneBook::get().getChannel(*it);
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
		it = m_channelList.erase(it);
	}
	close(m_fd);
}

int Client::getfd()const
{
	return (this->m_fd);
}

void Client::send(std::string, std::string msg)
{
	std::cerr << this->m_name << " <- " << msg << std::endl;
	if (*(msg.end() - 1) != '\n')
		msg += "\n";
	int check = ::send(m_fd, msg.c_str(), msg.size(), 0);
	// if (check < 0)
	// 	throw KillMePlease();
	return ;
}

bool Client::is_registered()
{
	return this->m_registrationComplete;
}

// Authentication
std::string Client::capabilites(std::string, std::string params)
{
	if (params == "LS" || params == "LS 302")
	{
		send("", "CAP * LS :\n");
		return ("");
	}
	if (params == "END")
		return ("");
	send("", ":ft_irc 404 :Not found");
	return ("");
}

std::string Client::auth(std::string password)
{
	std::string name = " " + (m_name.size() == 0 ? "*" : m_name);
	if (m_authenticated)
		send("", ":ft_irc " + ERR_ALREADYREGISTERED + name + " :You may not reregister");
	if (password == m_password)
		this->m_authenticated = true;
	else
	{
		send("", ":ft_irc " + ERR_PASSWDMISMATCH + name + " :Password mismatch");
		throw KillMePlease(); // required by irssi, specified as a MAY by the IRC protocol
	}
	return ("");
}

std::string Client::changeNick(std::string, std::string params)
{
	std::string name = " " + (m_name.size() == 0 ? "*" : m_name);
	if (m_authenticated == false)
	{
		send("", ":ft_irc " + ERR_PASSWDMISMATCH + name + " :Password mismatch");
		return ("");
	}
	if (PhoneBook::get().getRecipient(params) != NULL)
	{
		send("", ":ft_irc " + ERR_NICKNAMEINUSE + name + " " + params + " :Nickname is already in use");
		return ("");
	}
	this->m_name = params;
	return ("");
}

std::string Client::changeUser(std::string, std::string)
{
	std::string name = " " + (m_name.size() == 0 ? "*" : m_name);
	if (m_authenticated == false)
	{
		send("", ":ft_irc " + ERR_PASSWDMISMATCH + name + " :Password mismatch");
		return ("");
	}
	if (m_name.length() == 0)
		return (":ft_irc " + ERR_NONICKNAMEGIVEN + name + " :No nickname given");
	if (m_registrationComplete == true)
		return (":ft_irc " + ERR_ALREADYREGISTERED + name + " :You may not reregister");
	try
	{
		PhoneBook::get().addRecipient(this);
	}
	catch (std::exception &e)
	{
		send("", ":ft_irc " + ERR_NICKNAMEINUSE + name + " :Nickname is already in use");
		return ("");
	}
	this->send("", ":ft_irc 001 " + this->getName() + " :Welcome here");
	this->m_registrationComplete = true;
	return ("");
}

std::string	Client::joinChannel(std::string channel, std::string key)
{
	std::string topic;
	std::list<std::string>::iterator iter;

	for (iter = m_channelList.begin(); iter != m_channelList.end(); iter++)
		if (channel == *iter)
			return "";
	Channel *chan;
	Messageable *c = PhoneBook::get().getRecipient(channel);
	if (c == NULL)
	{
		chan = new Channel(channel);
	}
	else
	{
		chan = dynamic_cast<Channel *>(c);
		if (chan == NULL)
		{
			send("", ":ft_irc " + ERR_NOSUCHCHANNEL + " " +
				m_name + " " +
				channel + " :No such channel");
			return ("");
		}
	}
	try
	{
		std::string join_return = chan->join(this->getName(), key);
		send("", join_return);
		m_channelList.push_back(channel);
	}
	catch (const std::logic_error& e)
	{
		send("", e.what());
	}
	return ("");
}

void	Client::removeAllChannels(void)
{
	while (m_channelList.size() > 0)
	{
		removeChannel("", m_channelList.front());
	}
}

void	Client::removeChannel(std::string, std::string channelName)
{
	std::list<std::string>::iterator iter = m_channelList.begin();
	Channel *channel = NULL;

	while (iter != m_channelList.end())
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
			m_channelList.erase(iter);
			return ;
		}
	}
	return ;
}

std::string	Client::inviteToChannel(std::string invitee, std::string channel)
{
	std::string invite_return;
	Channel* temp_channel;
	temp_channel = PhoneBook::get().getChannel(channel);
	if (temp_channel == NULL)
	{
		send("", ":ft_irc " + ERR_NOSUCHCHANNEL + " " +
				m_name + " " +
				channel + " :No such channel");
		return "";
	}
	try 
	{
		temp_channel->invite(m_name, invitee);
	}
	catch (const std::logic_error &e)
	{
		send ("", e.what());
		return "";
	}
	Messageable* temp_client;
	temp_client = PhoneBook::get().getRecipient(invitee);
	if (temp_client == NULL)
		return "";
	send("", ":ft_irc " + RPL_INVITING + " " +
		m_name + " " +
		invitee + " " +
		channel);
	temp_client->send("", ":" + m_name + " INVITE " + invitee + " " + channel);
	return "";
}

// Message-related stuff
std::string	Client::getBuffer()const
{
	return (m_buffer);
}

void		Client::addBuffer(std::string buffer)
{
	m_buffer += buffer;
}

void		Client::clearBuffer(void)
{
	m_buffer.clear();
}

std::string Client::sendMessage(std::string, std::string params)
{
	std::string msg;
	std::vector<std::string>::iterator it;
	std::vector<std::string> args = strsplit(params, ' ');
	std::vector<std::string> recipients = strsplit((*args.begin()), ',');
	for (it = recipients.begin(); it != recipients.end(); it++)
	{
		std::string recipient = *it;
		if (recipient.size() == 0)
		{
			send("", ":ft_irc " + ERR_NORECIPIENT + " " + m_name + " :No recipient given (<PRIVMSG>)");
			continue ;
		}
		Messageable *m = PhoneBook::get().getRecipient(recipient);
		if (m == NULL)
		{
			if (recipient.at(0) == '#')
				send("", ":ft_irc " + ERR_CANNOTSENDTOCHAN + " " + m_name + " " + recipient + " " + ":Cannot send to channel");
			else
				send("", ":ft_irc " + ERR_NOSUCHNICK + " " + m_name + " :There was no such nickname");
			continue ;
		}
	
		std::string msg = ":" + m_name + " PRIVMSG " + recipient + " ";
		std::vector<std::string>::iterator it2 = ++args.begin();
		for (; it2 != args.end(); it2++)
			msg += *it2 + ' ';
		msg = msg.substr(0, msg.size() - 1);
		msg += "\n";
		m->send(m_name, msg);
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

std::string	Client::topicChannel(std::string channel)
{
	Channel* temp_channel;
	temp_channel = PhoneBook::get().getChannel(channel);
	if (temp_channel == NULL)
	{
		send("", ":ft_irc " + ERR_NOSUCHCHANNEL + " " +
				m_name + " " + 
				channel + " :No such channel");
		return ("");
	}
	std::string temp_topic = temp_channel->getTopic();
	if (temp_topic == "")
		send("", ":" + temp_channel->getName() + " " + RPL_NOTOPIC + " " +
			m_name + " " +
			channel + " :No topic is set");
	else
		send("", ":" + temp_channel->getName() + " " + RPL_TOPIC + " " +
			m_name + " " +
			channel + " " +
			" :"  + temp_topic);
	return "";
}

std::string	Client::topicChannel(std::string channel, std::string topic)
{
	Channel* temp_channel;
	temp_channel = PhoneBook::get().getChannel(channel);
	if (temp_channel == NULL)
	{
		send("", ":ft_irc " + ERR_NOSUCHCHANNEL + " " +
				m_name + " " + 
				channel + " :No such channel");
		return ("");
	}
	std::string topic_return = temp_channel->setTopic(topic, m_name);
	if (topic_return != "")
		send("", topic_return);
	return "";
}

std::string	Client::kickChannel(std::string channel, std::string kickee, std::string reason)
{
	Channel* temp_channel;
	temp_channel = PhoneBook::get().getChannel(channel);
	if (temp_channel == NULL)
	{
		send("", ":ft_irc " + ERR_NOSUCHCHANNEL + " " +
			m_name + " " +
			channel + " :No such channel");
		return ("");
	}
	Client* toKick;
	toKick = PhoneBook::get().getClient(kickee);
	if (toKick == NULL)
	{
		send("", ":ft_irc " +
			ERR_NOSUCHNICK + " " +
			m_name + " " +
			kickee + " :No such nick");
		return ("");
	}
	std::string kick_return = temp_channel->kick(toKick, m_name);
	if (kick_return != "")
	{
		send("", ":ft_irc " + kick_return);
		return "";
	}
	else
		toKick->send("", "KICK " + channel + " " + kickee + " :" + reason);
	toKick->removeChannel("", channel);
	return ("");
}

void	Client::getMode(std::string channel_name)
{
	Channel *temp_channel = PhoneBook::get().getChannel(channel_name);
	if (temp_channel == NULL)
		send("", ":ft_irc " + ERR_NOSUCHCHANNEL + " " +
			m_name + " " +
			channel_name + " :No such channel");
	std::string modes = "+";
	modes += temp_channel->getPasswordProtected() ? "k" : "";
	modes += temp_channel->getInvite() ? "i" : "";
	modes += temp_channel->getTopicProtected() ? "t" : "";
	modes += temp_channel->getClientLimit() > 0 ? "l" : "";
	if (modes.length() == 1)
		modes = "";
	send("", ":" + channel_name + " " + RPL_CHANNELMODEIS + " " +
		m_name + " " + 
		channel_name + " " + modes);
}

std::string	Client::modeChannel(std::string channel_name, bool plusminus, char modechar, std::string mode_arg)
{
	Channel *temp_channel = PhoneBook::get().getChannel(channel_name);
	if (temp_channel == NULL)
	{
		send("", ":ft_irc " + ERR_NOSUCHCHANNEL + " " +
			m_name + " " +
			channel_name + " :No such channel");
		return "";
	}
	std::string mode_return = temp_channel->mode(m_name, plusminus, modechar, mode_arg);
	if (mode_return != "")
		send("", ":ft_irc " + mode_return);
	return ("");
}
