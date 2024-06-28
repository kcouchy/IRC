/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kcouchma <kcouchma@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/27 14:57:02 by kcouchma          #+#    #+#             */
/*   Updated: 2024/06/28 15:27:56 by kcouchma         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Messageable.h"
#include "Pair.h"
#include <list>

class Channel : public Messageable
{
	public:
		Channel(std::string channelName);
		~Channel(void);

		void		join(std::string client_name);
		void		quit(std::string client_name);
		// void mode(std::string client_name, std::string flag);

		void		send(std::string message);
		std::string	getTopic(void)const;
		void		setTopic(std::string topic);
		void		setOperator(std::string client_name, bool new_value);

	private:
		Channel(void);
		std::string	m_topic;
		std::list<Pair<std::string, bool> > m_listenList; //std::string = client_name; bool = is_operator status
};
