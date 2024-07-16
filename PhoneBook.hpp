/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   PhoneBook.hpp                                             +**+   +*  *   */
/*                                                    +:+ +:+         +:+     */
/*   By: kcouchma <kcouchma@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/27 13:41:40 by kcouchma          #+#    #+#             */
/*   Updated: 2024/07/16 19:20:50 by aboyreau          +#-.-*  +         *    */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <list>
#include <string>
#include "Messageable.h"
#include "Channel.hpp"
#include "Client.h"

class Messageable;

class PhoneBook
{
	public:
		static PhoneBook& get();
		static void KiLl();

		Messageable *getRecipient(std::string name);
		void addRecipient(Messageable *newRecipient);
		void removeRecipient(Messageable *killRecipient);
		Channel *getChannel(std::string name);
		Client *getClient(std::string name);

	private:
		std::list<Messageable*> m_messageableList;

		PhoneBook(void);
		~PhoneBook(void);
		static PhoneBook* instance;
};
