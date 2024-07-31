/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.cpp                                               +**+   +*  *   */
/*                                                    +:+ +:+         +:+     */
/*   By: kcouchma <kcouchma@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/27 14:56:59 by kcouchma          #+#    #+#             */
/*   Updated: 2024/07/31 15:17:37 by aboyreau          +#-.-*  +         *    */
/*                                                                            */
/* ************************************************************************** */

#include "Channel.hpp"
#include "Messageable.h"
#include "PhoneBook.hpp"
#include "Utils.h"
#include <climits>
#include <stdexcept>
#include <vector>

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

Channel::~Channel(void)
{
	PhoneBook::get().removeRecipient(this);
}

std::string Channel::join(std::string client_name, std::string key)
{
	if (m_clientLimit != 0 && m_listenList.size() >= (size_t)m_clientLimit)
		throw std::logic_error(":ft_irc " + ERR_CHANNELISFULL + " " +
				client_name + " :Cannot join channel (+l)");
	if (m_inviteOnly == true && contains(m_inviteList, client_name) == false)
		throw std::logic_error(":ft_irc " + ERR_INVITEONLYCHAN + " " +
				client_name + " :Cannot join channel (+i)");
	if (m_channelKey.second == true && key != m_channelKey.first)
		throw std::logic_error(":ft_irc " + ERR_BADCHANNELKEY + " " +
				client_name + " :Cannot join channel (+k)");
	Pair<std::string, bool> to_add(client_name, false);
	if(m_listenList.size() == 0)
		to_add.value = true;
	m_listenList.push_back(to_add);
	find_erase(m_inviteList, client_name);
	send("", ":" + client_name + " JOIN " + m_name);
	return (":ft_irc " +
			RPL_TOPIC + " " +
			client_name + " " +
			m_name + " :" +
			m_topic + "\n" +
			":ft_irc " +
			RPL_NAMEREPLY + " " +
			getListenList(client_name) + "\n" +
			":ft_irc " +
			RPL_ENDOFNAMES + " " +
			client_name + " " +
			m_name +
			" :End of /NAMES list");
}

std::string Channel::getListenList(std::string client_name)
{
	std::string namelist;

	namelist = client_name +
		(m_inviteOnly ? " * " : " = ") +
		m_name + " :";
	std::list<Pair<std::string, bool> >::iterator it = m_listenList.begin();
	for (; it != m_listenList.end(); it++)
		namelist += ((*it).value ? "@" : "") + 
			(*it).getKey() + " ";
	namelist = namelist.substr(0, namelist.size() - 1);
	return (namelist);
}

void Channel::quit(std::string client_name)
{
	find_erase(m_listenList, client_name);

	if (m_listenList.size() == 0)
		throw EmptyChannel();
		
	send("", ":" + client_name + " PART " + m_name);
	std::list<Pair<std::string, bool> >::iterator iter;
	for (iter = m_listenList.begin(); iter != m_listenList.end(); iter++)
		if ((*iter).value == true)
			return ;
	m_listenList.begin()->value = true;
	send("", ":ft_irc MODE " + m_name + " +o " + m_listenList.begin()->getKey());
	// this->setOperator(m_listenList.front().getKey(), true);
	return ;
}

void	Channel::invite(std::string inviter_name, std::string invitee_name)
{
	for (std::list<Pair<std::string, bool> >::iterator itr = m_listenList.begin(); itr != m_listenList.end(); itr++)
	{
		if (invitee_name == (*itr).getKey())
			throw std::logic_error(":ft_irc " + ERR_USERONCHANNEL + " " +
					inviter_name + " " +
					invitee_name + " " +
					m_name + " " +
					":is already on that channel");
		if (m_inviteOnly == false && inviter_name == (*itr).getKey())
				return ;
		else if (m_inviteOnly == true && inviter_name == (*itr).getKey() && (*itr).value == 0)
			throw std::logic_error(":ft_irc " + ERR_CHANOPRIVSNEEDED + " " +
					inviter_name + " " +
					m_name + " " +
					":You're not channel operator");
		else if (m_inviteOnly == true && inviter_name == (*itr).getKey() && (*itr).value == 1)
		{
			m_inviteList.push_back(invitee_name);
			return ;
		}
	}
	throw std::logic_error(":ft_irc " + ERR_NOTONCHANNEL + " " +
		inviter_name + " " +
		m_name + " " +
		":You're not on that channel");
}

void Channel::send(std::string sender_name, std::string message)
{
	std::list<Pair<std::string, bool> >::iterator iter;
	Messageable *target;

	if (m_listenList.size() == 0)
		return ;
	for (iter = m_listenList.begin(); iter != m_listenList.end(); iter++)
	{
		target = PhoneBook::get().getRecipient((*iter).getKey());
		if (target == NULL)
			find_erase(m_listenList, (*iter).getKey());
		else if (target->getName() != sender_name)
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
			return (":ft_irc " + ERR_NOTONCHANNEL + " " +
				client_name + " " +
				m_name + " :You're not on that channel");
		else if ((*user).value == false)
			return (":ft_irc " + ERR_CHANOPRIVSNEEDED + " " +
				client_name + " " +
				m_name + " :You're not channel operator");
	}
	m_topic = topic;
	send("", ":" + client_name + " PRIVMSG " + m_name + " :channel topic has been changed to: " + topic);
	return ("");
}

bool	Channel::getTopicProtected(void)const
{
	return (m_topicProtected);
}

bool	Channel::getInvite(void)const
{
	return (m_inviteOnly);
}

int		Channel::getClientLimit(void)const
{
	return (m_clientLimit);
}

bool	Channel::getPasswordProtected(void)const
{
	return (m_channelKey.second);
}

std::string	Channel::kick(Client* toKick, std::string kicker)
{
	if (contains(m_listenList, toKick->getName()) == false)
		return (ERR_USERNOTINCHANNEL + " " +
			kicker + " " +
			toKick->getName() + " " +
			m_name + " :They aren't on that channel");
	std::list<Pair<std::string, bool> >::iterator kicker_isOP = find_return(m_listenList, kicker);
	if (kicker_isOP == m_listenList.end())
		return (ERR_NOTONCHANNEL + " " +
				kicker + " " +
				m_name + " :You're not on that channel");
	else if ((*kicker_isOP).value == false)
		return (ERR_CHANOPRIVSNEEDED + " " +
				kicker + " " +
				m_name + " :You're not channel operator");
	send("", ":" + kicker + " KICK " + m_name + " " + toKick->getName());
	return ("");
}

std::string	Channel::mode(std::string client_name, bool plusminus, char modechar, std::string mode_arg)
{
	if (contains(m_listenList, client_name) == false)
		return (ERR_NOTONCHANNEL + " " +
			client_name + " " +
			m_name + " :You're not on that channel");
	if (find_return(m_listenList, client_name)->value == false)
		return (ERR_CHANOPRIVSNEEDED + " " +
			client_name + " " +
			m_name + " :You're not channel operator");

// · i: Set/remove Invite-only channel
	if (modechar == 'i')
		m_inviteOnly = plusminus;

// · t: Set/remove the restrictions of the TOPIC command to channel operators
	if (modechar == 't')
		m_topicProtected = plusminus;

// · k: Set/remove the channel key (replicate allowed character list from nickname set)
	if (modechar == 'k')
	{
		if (plusminus == true)
		{
			if (mode_arg.find_first_not_of(AUTHORISED_SET) != mode_arg.npos)
				return (ERR_INVALIDKEY + " " +
					client_name + " " +
					m_name + " :Key is not well-formed");
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
			return (ERR_USERNOTINCHANNEL + " " +
				client_name + " " +
				mode_arg + " " +
				m_name + " :They aren't on that channel");
		find_return(m_listenList, mode_arg)->value = plusminus;
	}

// · l: Set/remove the user limit to channel
	if (modechar == 'l')
	{
		if (plusminus == true)
		{
			if (mode_arg.find_first_not_of("1234567890") != mode_arg.npos)
				return (ERR_INVALIDMODEPARAM + " " +
					client_name + " " +
					m_name + " l " + mode_arg +
					" :invalid Client Limit");
			double	new_limit = std::atof(mode_arg.c_str());
			if (new_limit > INT_MAX || new_limit < 1)
				return (ERR_INVALIDMODEPARAM + " " +
					client_name + " " +
					m_name + " l " + mode_arg +
					" :invalid Client Limit");
			m_clientLimit = (int)new_limit;
		}
		else if (plusminus == false)
			m_clientLimit = 0;
	}
	send("", ":" + client_name + " MODE " +
		(plusminus ? "+" : "-") +
		modechar + " " +
		mode_arg);
	return ("");
}

const char *Channel::EmptyChannel::what() const throw()
{
	return ("Don't leave me alone, I'm a lonely empty channel :c");
}
