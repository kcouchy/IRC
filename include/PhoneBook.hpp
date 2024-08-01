/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   PhoneBook.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lribette <lribette@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/27 13:41:40 by kcouchma          #+#    #+#             */
/*   Updated: 2024/08/01 18:35:16 by lribette         ###   ########.fr       */
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

		Messageable *getRecipient(std::string name);
		void addRecipient(Messageable *newRecipient);
		void removeRecipient(Messageable *killRecipient);
		Channel *getChannel(std::string name);
		Client *getClient(std::string name);

		size_t	getNumberOfClients();
		
	private:
		std::list<Messageable*> m_messageableList;

		static PhoneBook* instance;
};
