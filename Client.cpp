/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kcouchma <kcouchma@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/26 16:28:03 by kcouchma          #+#    #+#             */
/*   Updated: 2024/06/27 23:22:59 by aboyreau         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.h"
#include "PhoneBook.hpp"
#include <iostream>

Client::Client(int client_fd) : m_fd(client_fd)
{
	(void) client_fd;
	PhoneBook::get().addRecipient(this);
	return ;
}

Client::~Client(void)
{
	std::cout << "Oh no I'm dying" << this->getfd() << std::endl;
	PhoneBook::get().removeRecipient(this);
	// delete buffer; //only if there's a new somewhere - when we join reads
	return ;
}

int Client::getfd()const
{
	return(this->m_fd);
}

void Client::parse(std::string msg) const
{
	std::cout << this->m_fd << ": I'm parsing " << msg << std::endl;
}

void Client::send(std::string )
{
	return ;
}
