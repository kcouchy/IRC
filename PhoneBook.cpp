/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   PhoneBook.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kcouchma <kcouchma@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/27 13:59:21 by kcouchma          #+#    #+#             */
/*   Updated: 2024/06/27 14:32:54 by kcouchma         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "PhoneBook.hpp"

PhoneBook* PhoneBook::instance = nullptr;

PhoneBook::PhoneBook(void)
{
	return ;
}

PhoneBook::~PhoneBook(void)
{
	if (instance != nullptr)
		delete instance;
	instance = nullptr;
	return ;
}

PhoneBook& PhoneBook::get()
{
	if (instance == nullptr)
		instance = new PhoneBook;
	return (*instance);
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
		{
			iter = m_messageableList.erase(iter);
			break;
		}
	}
	return ;
}
