/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   PhoneBook.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kcouchma <kcouchma@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/27 13:59:21 by kcouchma          #+#    #+#             */
/*   Updated: 2024/06/28 15:32:48 by kcouchma         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "PhoneBook.hpp"
#include <cstdlib>

PhoneBook* PhoneBook::instance = NULL;

PhoneBook::PhoneBook(void)
{
	return ;
}

PhoneBook::~PhoneBook(void) {}

PhoneBook& PhoneBook::get()
{
	if (instance == NULL)
		instance = new PhoneBook;
	return (*instance);
}

void PhoneBook::KiLl()
{
	if (instance != NULL)
		delete instance;
	instance = NULL;
	return ;
}

void	PhoneBook::addRecipient(Messageable *newRecipient)
{
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
	return (NULL);//TODO this should trigger a destinator not found message to the sender
}