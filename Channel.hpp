/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kcouchma <kcouchma@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/27 14:57:02 by kcouchma          #+#    #+#             */
/*   Updated: 2024/06/27 16:00:44 by aboyreau         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Messageable.h"
#include <list>

class Channel : public Messageable
{
	public:
		Channel(std::string topic);
		~Channel(void);

		void join(std::string client_name);
		void quit(std::string client_name);
		// void mode(std::string client_name, std::string flag);

		void send(std::string message);

	private:
		Channel(void);
		std::string	m_topic;
		std::list<std::string> m_listenList;
		// std::list<std::string> m_operatorList;
};
