/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kcouchma <kcouchma@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/27 14:56:59 by kcouchma          #+#    #+#             */
/*   Updated: 2024/08/01 22:17:28 by aboyreau         ###   ########.fr       */
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

std::string Channel::join(Client *client, std::string key)
{
	if (m_clientLimit != 0 && m_listenList.size() >= (size_t)m_clientLimit)
		throw std::logic_error(":ft_irc " + ERR_CHANNELISFULL + " " +
				client->getName() + " :Cannot join channel (+l)");
	if (m_inviteOnly == true && contains(m_inviteList, client->getName()) == false)
		throw std::logic_error(":ft_irc " + ERR_INVITEONLYCHAN + " " +
				client->getName() + " :Cannot join channel (+i)");
	if (m_channelKey.second == true && key != m_channelKey.first)
		throw std::logic_error(":ft_irc " + ERR_BADCHANNELKEY + " " +
				client->getName() + " :Cannot join channel (+k)");
	Pair<std::string, bool> to_add(client->getName(), false);
	if(m_listenList.size() == 0)
		to_add.value = true;
	m_listenList.push_back(to_add);
	find_erase(m_inviteList, client->getName());
	send("", ":" + client->getIdentifier() + " JOIN " + m_name);
	return (":ft_irc " +
			RPL_TOPIC + " " +
			client->getName() + " " +
			m_name + " :" +
			m_topic + "\n" +
			":ft_irc " +
			RPL_NAMEREPLY + " " +
			getListenList(client->getName()) + "\n" +
			":ft_irc " +
			RPL_ENDOFNAMES + " " +
			client->getName() + " " +
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

bool	Channel::isInListenList(std::string client_name)
{
	return (contains(m_listenList, client_name));
}

void Channel::quit(Client *client, bool display)
{
	if (display)
		send("", ":" + client->getIdentifier() + " PART " + m_name);
	find_erase(m_listenList, client->getName());

	if (m_listenList.size() == 0)
		throw EmptyChannel();

	std::list<Pair<std::string, bool> >::iterator iter;
	for (iter = m_listenList.begin(); iter != m_listenList.end(); iter++)
		if ((*iter).value == true)
			return ;
	m_listenList.begin()->value = true;
	send("", ":ft_irc MODE " + m_name + " +o " + m_listenList.begin()->getKey());
	return ;
}

void	Channel::invite(std::string inviter_name, std::string invitee_name)
{
	if (contains(m_listenList, invitee_name) == true)
	{
		throw std::logic_error(":ft_irc " + ERR_USERONCHANNEL + " " +
			inviter_name + " " +
			invitee_name + " " +
			m_name + " " +
			":is already on that channel");
	}
	if (contains(m_listenList, inviter_name) == false)
	{
		throw std::logic_error(":ft_irc " + ERR_NOTONCHANNEL + " " +
			inviter_name + " " +
			m_name + " " +
			":You're not on that channel");
	}
	if (m_inviteOnly == true && find_return(m_listenList, inviter_name)->value == false)
	{
		throw std::logic_error(":ft_irc " + ERR_CHANOPRIVSNEEDED + " " +
			inviter_name + " " +
			m_name + " " +
			":You're not channel operator");
	}
	m_inviteList.push_back(invitee_name);
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

std::string Channel::setTopic(std::string topic, Client *client)
{
	std::list<Pair<std::string, bool> >::iterator user = find_return(m_listenList, client->getName());
	if (user == m_listenList.end())
		return (":ft_irc " + ERR_NOTONCHANNEL + " " +
			client->getName() + " " +
			m_name + " :You're not on that channel");
	if (m_topicProtected == true)
	{
		if ((*user).value == false)
			return (":ft_irc " + ERR_CHANOPRIVSNEEDED + " " +
				client->getName() + " " +
				m_name + " :You're not channel operator");
	}
	m_topic = topic;
	send("", ":" + client->getIdentifier() + " TOPIC " + m_name + " :" + topic);
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

std::string	Channel::kick(Client* toKick, Client *client)
{
	if (contains(m_listenList, toKick->getName()) == false)
		return (ERR_USERNOTINCHANNEL + " " +
			client->getName() + " " +
			toKick->getName() + " " +
			m_name + " :They aren't on that channel");
	std::list<Pair<std::string, bool> >::iterator kicker_isOP = find_return(m_listenList, client->getName());
	if (kicker_isOP == m_listenList.end())
		return (ERR_NOTONCHANNEL + " " +
			client->getName() + " " +
			m_name + " :You're not on that channel");
	else if ((*kicker_isOP).value == false)
		return (ERR_CHANOPRIVSNEEDED + " " +
			client->getName() + " " +
			m_name + " :You're not channel operator");
	send("", ":" + client->getIdentifier() + " KICK " + m_name + " " + toKick->getName());
	return ("");
}

std::string	Channel::mode(Client *client, bool plusminus, char modechar, std::string mode_arg)
{
	if (contains(m_listenList, client->getName()) == false)
		return (ERR_NOTONCHANNEL + " " +
			client->getName() + " " +
			m_name + " :You're not on that channel");
	if (find_return(m_listenList, client->getName())->value == false)
		return (ERR_CHANOPRIVSNEEDED + " " +
			client->getName() + " " +
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
					client->getName() + " " +
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
				client->getName() + " " +
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
					client->getName() + " " +
					m_name + " l " + mode_arg +
					" :invalid Client Limit");
			double	new_limit = std::atof(mode_arg.c_str());
			if (new_limit > INT_MAX || new_limit < 1)
				return (ERR_INVALIDMODEPARAM + " " +
					client->getName() + " " +
					m_name + " l " + mode_arg +
					" :invalid Client Limit");
			m_clientLimit = (int)new_limit;
		}
		else if (plusminus == false)
			m_clientLimit = 0;
	}
	send("", ":" + client->getIdentifier() + " MODE " +
		m_name + " " +
		(plusminus ? "+" : "-") +
		modechar + " " +
		mode_arg);
	return ("");
}

const char *Channel::EmptyChannel::what() const throw()
{
	return ("Don't leave me alone, I'm a lonely empty channel :c");
}
