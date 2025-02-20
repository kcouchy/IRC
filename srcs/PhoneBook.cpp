/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   PhoneBook.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lribette <lribette@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/27 13:59:21 by kcouchma          #+#    #+#             */
/*   Updated: 2024/08/01 18:35:31 by lribette         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "PhoneBook.hpp"
#include <cstdlib>

PhoneBook* PhoneBook::instance = NULL;

PhoneBook& PhoneBook::get()
{
	if (instance == NULL)
		instance = new PhoneBook;
	return (*instance);
}

void	PhoneBook::addRecipient(Messageable *newRecipient)
{
	if (getRecipient(newRecipient->getName()))
		throw std::exception();
	m_messageableList.push_back(newRecipient);
	return ;
}

void	PhoneBook::removeRecipient(Messageable *killRecipient)
{
	std::list<Messageable *>::iterator iter;

	for (iter = m_messageableList.begin(); iter != m_messageableList.end(); iter++)
	{
		if (*iter == killRecipient)
			iter = m_messageableList.erase(iter);
	}
}

Messageable *PhoneBook::getRecipient(std::string name)
{
	std::list<Messageable *>::iterator iter;

	for (iter = m_messageableList.begin(); iter != m_messageableList.end(); iter++)
		if ((*iter)->getName() == name)
			return (*iter);
	return (NULL);
}

Channel *PhoneBook::getChannel(std::string channel_name)
{
	return (dynamic_cast<Channel*>(getRecipient(channel_name)));
}

Client *PhoneBook::getClient(std::string client_name)
{
	return (dynamic_cast<Client*>(getRecipient(client_name)));
}

size_t	PhoneBook::getNumberOfClients()
{
	std::list<Messageable*>::iterator it;
	size_t counter = 0;

	for (it = m_messageableList.begin(); it != m_messageableList.end(); it++)
	{
		if (dynamic_cast<Client*>(*it) != NULL)
			counter++;
	}
	return (counter);
}