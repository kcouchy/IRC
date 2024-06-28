/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kcouchma <kcouchma@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/25 23:17:50 by aboyreau               #+#    #+#             */
/*   Updated: 2024/06/29 13:58:01 by aboyreau         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <list>
#include <string>
#include "Messageable.h"

class Client : public Messageable
{
	public:
		Client (int client_fd);
		~Client(void);

		int			getfd() const;
		void		read()  const;
		void		parse(std::string);
		void		send(std::string message);
		void		addChannel(std::string channelName);
		void		removeChannel(std::string channelName);

	private:
		int			m_fd;
		std::string	m_buffer;
		std::list<std::string> m_channelList;

		Client(void);

		// Handlers
		// Find and run a handler according to the command passed as parameter.
		void		exec(std::string prefix, std::string command, std::string args);

};
