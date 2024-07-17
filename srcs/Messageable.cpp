/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Messageable.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kcouchma <kcouchma@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/27 14:26:45 by aboyreau          #+#    #+#             */
/*   Updated: 2024/07/16 16:40:01 by kcouchma         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Messageable.h"
#include "PhoneBook.hpp"

Messageable::Messageable()
{
	PhoneBook::get().addRecipient(this);
}

Messageable::Messageable(std::string name)
	: m_name(name)
{
	PhoneBook::get().addRecipient(this);
}

Messageable::~Messageable()
{
	PhoneBook::get().removeRecipient(this);
}

Messageable::Messageable(const Messageable& instance) : m_name(instance.m_name)
{
	PhoneBook &pb = PhoneBook::get();
	pb.addRecipient(this);
}

Messageable& Messageable::operator=(const Messageable&)
{
	return (*this);
}

std::string Messageable::getName() const
{
	return (this->m_name);
}

const char *Messageable::RecipientNotFound::what() const throw()
{
	return ("Recipient not found.");
}
