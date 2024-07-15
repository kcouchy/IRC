/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.h                                                  +**+   +*  *   */
/*                                                    +:+ +:+         +:+     */
/*   By: kcouchma <kcouchma@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/25 23:17:50 by aboyreau               #+#    #+#             */
/*   Updated: 2024/07/15 14:25:34 by aboyreau          +#-.-*  +         *    */
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

		int		getfd() const;
		void	read()  const;
		void	parse(std::string);
		void	send(std::string message);

	private:
		int			m_fd;
		std::string	m_buffer;
		std::list<std::string> m_channelList;

		Client(void);

		// Handlers
		// Find and run a handler according to the command passed as parameter.
		void	exec(std::string prefix, std::string command, std::string args);
		void	auth(std::string prefix, std::string args);
		void	addChannel(std::string, std::string channels);
		void	removeChannel(std::string, std::string channelName);
		void	sendMessage(std::string, std::string channels);
		void	changeNick(std::string, std::string);
		void	changeUser(std::string, std::string params);

};
