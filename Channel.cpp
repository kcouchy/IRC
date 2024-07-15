/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kcouchma <kcouchma@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/27 14:56:59 by kcouchma          #+#    #+#             */
/*   Updated: 2024/07/15 18:17:04 by kcouchma         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Channel.hpp"
#include "Messageable.h"
#include "PhoneBook.hpp"
#include <exception>

Channel::Channel(std::string channelName) :
	Messageable(channelName),
	m_inviteOnly(false) {}

Channel::~Channel(void) {}

//TODO check inviteOnly status
void Channel::join(std::string client_name)
{
	Pair<std::string, bool> to_add(client_name, false);

	if(m_listenList.size() == 0)
		to_add.value = true;
	m_listenList.push_back(to_add);
}

void Channel::quit(std::string client_name)
{
	std::list<Pair<std::string, bool> >::iterator iter;

	for (iter = m_listenList.begin(); iter != m_listenList.end(); iter++)
		if ((*iter).getKey() == client_name)
			iter = m_listenList.erase(iter);
	for (iter = m_listenList.begin(); iter != m_listenList.end(); iter++)
		if ((*iter).value == true)
			return;
	this->setOperator(m_listenList.front().getKey(), true);
	if (m_listenList.size() == 0)
		throw EmptyChannel();
}

void	Channel::invite(std::string inviter_name, std::string invitee_name)
{
	for (std::list<Pair<std::string, bool> >::iterator itr; itr != m_listenList.end(); itr++)
	{
		if (invitee_name == (*itr).getKey())
			throw UserOnChannel();
		if (m_inviteOnly == false && inviter_name == (*itr).getKey())
				return ;
		else if (m_inviteOnly == true && inviter_name == (*itr).getKey() && (*itr).value == 0)
			throw NotOperator();
		else if (m_inviteOnly == true && inviter_name == (*itr).getKey() && (*itr).value == 1)
			return ;
	}
	throw NotOnChannel();
}

// void	Channel::kick(std::string client_name)
// {

// 	return;
// }

// void	Channel::topic(std::string topic_name)
// {

// 	return;
// }

void Channel::send(std::string msg)
{
	std::list<Pair<std::string, bool> >::iterator iter;
	Messageable *target;
	for (iter = m_listenList.begin(); iter != m_listenList.end(); iter++)
	{
		target = PhoneBook::get().getRecipient((*iter).getKey());
		if (target == NULL)
			throw Messageable::RecipientNotFound();
		(*target).send(msg);
	}
}
std::string Channel::getTopic(void)const
{
	return (m_topic);
}

void Channel::setTopic(std::string topic)
{
	m_topic = topic;
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

const char *Channel::EmptyChannel::what() const throw()
{
	return ("Don't leave me alone, I'm a lonely empty channel :c");
}

const char *Channel::NotOperator::what() const throw()
{
	return ("Nope, not allowed, you're not a channel operator");
}

const char *Channel::NotOnChannel::what() const throw()
{
	return ("Nope, not allowed, the user cannot invite to a channel they don't belong to");
}

const char *Channel::UserOnChannel::what() const throw()
{
	return ("Nope, can't invite a user to a channel they already belong to");
}