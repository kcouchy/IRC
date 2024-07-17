/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kcouchma <kcouchma@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/27 14:56:59 by kcouchma          #+#    #+#             */
/*   Updated: 2024/07/17 14:45:17 by kcouchma         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Channel.hpp"
#include "Messageable.h"
#include "PhoneBook.hpp"

Channel::Channel(std::string channelName) :
	Messageable(channelName),
	m_inviteOnly(false),
	m_topicProtected(false),
	m_topic(),
	m_listenList(),
	m_inviteList() {}

Channel::~Channel(void) {}

//TODO check inviteOnly status
void Channel::join(std::string client_name)
{
	if (m_inviteOnly == true && find_only(m_inviteList, client_name) == false)
		return ;

	Pair<std::string, bool> to_add(client_name, false);
	if(m_listenList.size() == 0)
		to_add.value = true;
	m_listenList.push_back(to_add);
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

void Channel::send(std::string msg)
{
	std::list<Pair<std::string, bool> >::iterator iter;
	Messageable *target;
	for (iter = m_listenList.begin(); iter != m_listenList.end(); iter++)
	{
		target = PhoneBook::get().getRecipient((*iter).getKey());
		if (target == NULL)
			throw Messageable::RecipientNotFound(); //TODO CATCH ME BABY
		(*target).send(msg);
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
		std::list<Pair<std::string, bool> >::iterator user = std::find(m_listenList.begin(), m_listenList.end(), client_name);
		if (user == m_listenList.end())
			return(ERR_NOTONCHANNEL);
		else if ((*user).value == false)
			return(ERR_CHANOPRIVSNEEDED);
	}
	m_topic = topic;
	send(":" + m_name + " PRIVMSG " + m_name + " :channel topic has been changed to: " + topic);
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
	if (std::find( m_listenList.begin(), m_listenList.end(), toKick->getName()) == m_listenList.end())
		return (ERR_USERNOTINCHANNEL);
	std::list<Pair<std::string, bool> >::iterator kicker_isOP = std::find(m_listenList.begin(), m_listenList.end(), kicker);
	if (kicker_isOP == m_listenList.end())
		return (ERR_NOTONCHANNEL);
	else if ((*kicker_isOP).value == false)
		return (ERR_CHANOPRIVSNEEDED);
	send(":" + kicker + " KICK " + m_name + " " + toKick->getName());
	return ("");
}

const char *Channel::EmptyChannel::what() const throw()
{
	return ("Don't leave me alone, I'm a lonely empty channel :c");
}

// const char *Channel::NotOperator::what() const throw()
// {
// 	return ("Nope, not allowed, you're not a channel operator");
// }

const char *Channel::NotOnChannel::what() const throw()
{
	return ("Nope, not allowed, the user is not on the channel");
}

// const char *Channel::UserOnChannel::what() const throw()
// {
// 	return ("Nope, can't invite a user to a channel they already belong to");
// }
