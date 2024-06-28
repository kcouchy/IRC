/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   PhoneBook.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kcouchma <kcouchma@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/27 13:41:40 by kcouchma          #+#    #+#             */
/*   Updated: 2024/06/28 11:58:23 by kcouchma         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <list>
#include <string>
#include "Messageable.h"

class Messageable;

class PhoneBook
{
	public:
		static PhoneBook& get();
		static void KiLl();

		Messageable *getRecipient(std::string name);
		void addRecipient(Messageable *newRecipient);
		void removeRecipient(Messageable *killRecipient);

	private:
		std::list<Messageable*> m_messageableList;

		PhoneBook(void);
		~PhoneBook(void);
		static PhoneBook* instance;
};
