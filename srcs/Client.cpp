/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kcouchma <kcouchma@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/16 17:33:15 by aboyreau          #+#    #+#             */
/*   Updated: 2024/08/02 10:00:34 by kcouchma         ###   ########.fr       */
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
#include <sstream>
#include "Utils.h"

Client::Client(int client_fd, std::string password) :
	m_fd(client_fd),
	m_password(password),
	m_channelList(),
	m_userSet(false),
	m_registrationComplete(false)
{
	m_authenticated = m_password.size() == 0;
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
			channel->quit(this, true);
		}
		catch(const Channel::EmptyChannel& e)
		{
			delete channel;
		}
		it = m_channelList.erase(it);
	}
	close(m_fd);
}

std::string Client::getIdentifier() const
{
	return this->m_identifier;
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
	::send(m_fd, msg.c_str(), msg.size(), MSG_NOSIGNAL);
	return ;
}

bool Client::is_registered()
{
	return this->m_registrationComplete;
}

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
		throw KillMePlease();
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
	clientRegistered();
	return ("");
}

std::string Client::changeUser(std::string, std::string params)
{
	std::vector<std::string> args = strsplit(params, ' ');
	std::string name = " " + (m_name.size() == 0 ? "*" : m_name);

	this->m_identifier = this->m_name;
	if (args.size() < 3 || (args.size() >= 3 && (args[0] == "" || args[2] == "")))
	{
		send("", ":ft_irc " + ERR_NEEDMOREPARAMS + name + " USER :Not enough parameters");
		return ("");
	}
	if (args.size() >= 3)
	{
		this->m_identifier += "!" + args.at(0);
		this->m_identifier += "@" + args.at(2);
	}
	if (m_authenticated == false)
	{
		send("", ":ft_irc " + ERR_PASSWDMISMATCH + name + " :Password mismatch");
		return ("");
	}
	m_userSet = true;
	if (m_registrationComplete == true)
	{
		send("", ":ft_irc " + ERR_ALREADYREGISTERED + name + " :You may not reregister");
		return "";
	}
	clientRegistered();
	return ("");
}

void Client::clientRegistered(void)
{
	if (m_authenticated == true && m_name.size() != 0 && m_userSet == true)
	{
		try
		{
			PhoneBook::get().addRecipient(this);
		}
		catch (std::exception &e)
		{
			send("", ":ft_irc " + ERR_NICKNAMEINUSE + m_name + " :Nickname is already in use");
			return ;
		}
		this->send("", ":ft_irc 001 " + this->getName() + " :Welcome to the ft_irc network, " + m_name);
		this->send("", ":ft_irc 002 " + this->getName() + " :Your host is ft_irc, running version 1.0");
		this->send("", ":ft_irc 003 " + this->getName() + " :This server was created today");
		this->send("", ":ft_irc 004 " + this->getName() + " :ft_irc 1.0 USERMODES= CHANMODES=iklot");
		this->send("", ":ft_irc 005 " + this->getName() + " :CHANMODES=iklot");
		this->send("", ":ft_irc 005 " + this->getName() + " :NICKLEN=30");
		this->send("", ":ft_irc 005 " + this->getName() + " :CHANLEN=30");
		this->m_registrationComplete = true;
	}
}

std::string	Client::joinChannel(std::string channel, std::string key)
{
	std::string topic;
	std::list<std::string>::iterator iter;

	if (channel.length() > 30)
	{
		send("", ":ft_irc " + ERR_BADCHANMASK +
			+ " " + channel + " :Bad Channel Mask");
		return ("");
	}
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
		std::string join_return = chan->join(this, key);
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
		removeChannel("", m_channelList.front(), true);
	}
}

void	Client::removeChannel(std::string, std::string channelName, bool display)
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
				channel->quit(this, display);
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
	Client* temp_client;
	temp_client = PhoneBook::get().getClient(invitee);
	if (temp_client == NULL)
	{
		send("", ":ft_irc " + ERR_NOSUCHNICK + " " +
			m_name + " " +
			invitee + " :There was no such nickname");
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
	send("", ":ft_irc " + RPL_INVITING + " " +
		m_name + " " +
		invitee + " " +
		channel);
	temp_client->send("", ":" + m_identifier + " INVITE " + invitee + " " + channel);
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
	std::vector<std::string> args = strsplit(params, ' ');
	std::vector<std::string> recipients = strsplit((args.front()), ',');

	std::vector<std::string>::iterator it;
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
				send("", ":ft_irc " + ERR_NOSUCHNICK + " " + m_name + " " + recipient + " :There was no such nickname");
			continue ;
		}
		if (dynamic_cast<Channel*>(m) != NULL)
		{
			if (((Channel*)m)->isInListenList(m_name) == false)
			{
				send("", ":ft_irc " + ERR_NOTONCHANNEL + " " +
					m_name + " " + recipient + " " +
					":You're not on that channel");
				return ("");
			}
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
		send("", ":ft_irc " + RPL_NOTOPIC + " " +
			m_name + " " +
			channel + " :No topic is set");
	else
		send("", ":ft_irc " + RPL_TOPIC + " " +
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
	std::string topic_return = temp_channel->setTopic(topic, this);
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
			kickee + " :There was no such nickname");
		return ("");
	}
	std::string kick_return = temp_channel->kick(toKick, this);
	if (kick_return != "")
	{
		send("", ":ft_irc " + kick_return);
		return "";
	}
	else
		toKick->send("", " KICK " + channel + " " + kickee + " " + reason);
	toKick->removeChannel("", channel, false);
	return ("");
}

void	Client::getMode(std::string channel_name)
{
	Channel *temp_channel = PhoneBook::get().getChannel(channel_name);
	if (temp_channel == NULL)
	{
		send("", ":ft_irc " + ERR_NOSUCHCHANNEL + " " +
			m_name + " " +
			channel_name + " :No such channel");
		return ;
	}
	std::string modes = "+";
	modes += temp_channel->getPasswordProtected() ? "k" : "";
	modes += temp_channel->getInvite() ? "i" : "";
	modes += temp_channel->getTopicProtected() ? "t" : "";
	modes += temp_channel->getClientLimit() > 0 ? "l" : "";
	if (modes.length() == 1)
		modes = "";
	send("", ":ft_irc " + RPL_CHANNELMODEIS + " " +
		m_name + " " + 
		channel_name + " " + modes);
}

std::string	Client::modeChannel(std::string channel_name, bool plusminus, char modechar, std::string mode_arg)
{
	Channel *temp_channel = PhoneBook::get().getChannel(channel_name);
	if (temp_channel == NULL)
	{
		if (PhoneBook::get().getClient(channel_name) != NULL)
			return "";
		send("", ":ft_irc " + ERR_NOSUCHCHANNEL + " " +
			m_name + " " +
			channel_name + " :No such channel");
		return "";
	}
	std::string mode_return = temp_channel->mode(this, plusminus, modechar, mode_arg);
	if (mode_return != "")
		send("", ":ft_irc " + mode_return);
	return ("");
}
