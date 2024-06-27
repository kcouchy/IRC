/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Messageable.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aboyreau <bnzlvosnb@mozmail.com>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/27 14:26:45 by aboyreau          #+#    #+#             */
/*   Updated: 2024/06/27 14:37:41 by aboyreau         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Messageable.h"
#include "PhoneBook.hpp"

Messageable::Messageable() {}

Messageable::Messageable(const Messageable& instance) : name(instance.name)
{
	PhoneBook &pb = PhoneBook::get();
	pb.addRecipient(this);
}

Messageable& Messageable::operator=(const Messageable&)
{ return *this; }

std::string Messageable::getName()
{
	return this->name;
}
