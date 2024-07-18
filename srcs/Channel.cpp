/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kcouchma <kcouchma@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/27 14:56:59 by kcouchma          #+#    #+#             */
/*   Updated: 2024/07/18 16:50:59 by kcouchma         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Channel.hpp"
#include "Messageable.h"
#include "PhoneBook.hpp"
#include "Utils.h"
#include <climits>

Channel::Channel(std::string channelName) :
	Messageable(channelName),
	m_inviteOnly(false),
	m_topicProtected(false),
	m_topic(),
	m_clientLimit(0),
	m_channelKey("", false),
	m_listenList(),
	m_inviteList() 
{
	while (true)
	{
		try
		{
			PhoneBook::get().addRecipient(this);
			break ;
		}
		catch (std::exception &e)
		{
			m_name += "_nope";
		}
	}
}

Channel::~Channel(void) {}

std::string Channel::join(std::string client_name, std::string key)
{
	if (m_clientLimit != 0 && m_listenList.size() >= (size_t)m_clientLimit)
		return (ERR_CHANNELISFULL);
	if (m_inviteOnly == true && contains(m_inviteList, client_name) == false)
		return (ERR_INVITEONLYCHAN);
	if (m_channelKey.second == true && key != m_channelKey.first)
		return (ERR_BADCHANNELKEY);
	Pair<std::string, bool> to_add(client_name, false);
	if(m_listenList.size() == 0)
		to_add.value = true;
	m_listenList.push_back(to_add);
	find_erase(m_inviteList, client_name);
	return ("");
}

std::string Channel::quit(std::string client_name)
{
	find_erase(m_listenList, client_name);

	std::list<Pair<std::string, bool> >::iterator iter;
	for (iter = m_listenList.begin(); iter != m_listenList.end(); iter++)
		if ((*iter).value == true)
			return ("");

	this->setOperator(m_listenList.front().getKey(), true);
	if (m_listenList.size() == 0)
		throw EmptyChannel();
	return ("");
}

std::string	Channel::invite(std::string inviter_name, std::string invitee_name)
{
	for (std::list<Pair<std::string, bool> >::iterator itr; itr != m_listenList.end(); itr++)
	{
		if (invitee_name == (*itr).getKey())
			return (ERR_USERONCHANNEL);
		if (m_inviteOnly == false && inviter_name == (*itr).getKey())
				return ("");
		else if (m_inviteOnly == true && inviter_name == (*itr).getKey() && (*itr).value == 0)
			return (ERR_CHANOPRIVSNEEDED);
		else if (m_inviteOnly == true && inviter_name == (*itr).getKey() && (*itr).value == 1)
		{
			m_inviteList.push_back(invitee_name);
			return ("");
		}
	}
	return (ERR_NOTONCHANNEL);
}

void Channel::send(std::string sender_name, std::string message)
{
	std::list<Pair<std::string, bool> >::iterator iter;
	Messageable *target;
	for (iter = m_listenList.begin(); iter != m_listenList.end(); iter++)
	{
		target = PhoneBook::get().getRecipient((*iter).getKey());
		if (target == NULL)
			find_erase(m_listenList, target->getName());
		if (target->getName() != sender_name)
			(*target).send("", message);
	}
}

std::string Channel::getTopic(void)const
{
	return (m_topic);
}

std::string Channel::setTopic(std::string topic, std::string client_name)
{
	if (m_topicProtected == true)
	{
		std::list<Pair<std::string, bool> >::iterator user = find_return(m_listenList, client_name);
		if (user == m_listenList.end())
			return(ERR_NOTONCHANNEL);
		else if ((*user).value == false)
			return(ERR_CHANOPRIVSNEEDED);
	}
	m_topic = topic;
	send("", ":" + m_name + " PRIVMSG " + m_name + " :channel topic has been changed to: " + topic);
	return ("");
}

bool	Channel::getTopicProtected(void)const
{
	return (m_topicProtected);
}

void	Channel::setTopicProtected(bool isProtected)
{
	m_topicProtected = isProtected;
}

void Channel::setOperator(std::string client_name, bool new_value)
{
	std::list<Pair<std::string, bool> >::iterator iter;

	for (iter = m_listenList.begin(); iter != m_listenList.end(); iter++)
		if ((*iter).getKey() == client_name)
			(*iter).value = new_value;
}

void	Channel::setInvite(bool inviteOnly)
{
	m_inviteOnly = inviteOnly;
	return ;
}

std::string	Channel::kick(Client* toKick, std::string kicker)
{
	if (contains(m_listenList, toKick->getName()))
		return (ERR_USERNOTINCHANNEL);
	std::list<Pair<std::string, bool> >::iterator kicker_isOP = find_return(m_listenList, kicker);
	if (kicker_isOP == m_listenList.end())
		return (ERR_NOTONCHANNEL);
	else if ((*kicker_isOP).value == false)
		return (ERR_CHANOPRIVSNEEDED);
	send("", ":" + kicker + " KICK " + m_name + " " + toKick->getName());
	return ("");
}

std::string	Channel::mode(std::string client_name, bool plusminus, char modechar, std::string mode_arg)
{
	if (contains(m_listenList, client_name) == false)
		return (ERR_NOTONCHANNEL);//not in doc but is logical to add
	if (find_return(m_listenList, client_name)->value == false)
		return (ERR_CHANOPRIVSNEEDED);

// · i: Set/remove Invite-only channel
	if (modechar == 'i')
	{
		if (plusminus == true)
			m_inviteOnly = true;
		else if (plusminus == false)
			m_inviteOnly = false;
	}

// · t: Set/remove the restrictions of the TOPIC command to channel operators
	if (modechar == 't')
	{
		if (plusminus == true)
			m_topicProtected = true;
		else if (plusminus == false)
			m_topicProtected = false;
	}

// · k: Set/remove the channel key (replicate allowed character list from nickname set)
	if (modechar == 'k')
	{
		//TO PARSING LEVEL- ERR_INVALIDMODEPARAM (696)
		if (plusminus == true)
		{
			if (mode_arg.find_first_not_of(AUTHORISED_SET) != mode_arg.npos)
				return (ERR_INVALIDKEY);
			m_channelKey.second = true;
			m_channelKey.first = mode_arg;
		}
		else if (plusminus == false)
		{
			m_channelKey.second = false;
			m_channelKey.first = "";
		}
	}

// · o: Give/take channel operator privilege
	if (modechar == 'o')
	{
		if (contains(m_listenList, mode_arg) == false)
			return (ERR_USERNOTINCHANNEL);
		if (plusminus == true)
			find_return(m_listenList, mode_arg)->value = true;
		else if (plusminus == false)
			find_return(m_listenList, mode_arg)->value = false;
	}

// · l: Set/remove the user limit to channel
	if (modechar == 'l')
	{
		if (plusminus == true)
		{
			if (mode_arg.find_first_not_of("1234567890") != mode_arg.npos)
				return (ERR_INVALIDMODEPARAM);
			double	new_limit = std::atof(mode_arg.c_str());
			if (new_limit > INT_MAX || new_limit < 1)
				return (ERR_INVALIDMODEPARAM);
			m_clientLimit = (int)new_limit;
		}
		else if (plusminus == false)
			m_clientLimit = 0;
	}
	return ("");
}

const char *Channel::EmptyChannel::what() const throw()
{
	return ("Don't leave me alone, I'm a lonely empty channel :c");
}
