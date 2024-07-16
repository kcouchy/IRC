/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.h                                                  +**+   +*  *   */
/*                                                    +:+ +:+         +:+     */
/*   By: kcouchma <kcouchma@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/25 23:17:50 by aboyreau               #+#    #+#             */
/*   Updated: 2024/07/15 22:42:13 by aboyreau          +#-.-*  +         *    */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <exception>
#include <vector>
#include <string>
#include "Messageable.h"

class Client : public Messageable
{
	public:
		Client (int client_fd, std::string password);
		~Client(void);

		int		getfd() const;
		void	send(std::string msg);
		void	parse(std::string);

		class KillMePlease : public std::exception
		{
			public:
				const char *what() const throw();
		};

	private:
		int			m_fd;
		std::string	m_buffer;
		std::string	m_password;
		bool		m_authenticated;
		std::vector<std::string> m_channelList;

		Client(void);

		// Handlers
		// Find and run a handler according to the command passed as parameter.
		void	exec(std::string prefix, std::string command, std::string args);

		// Authentication
		std::string	capabilites(std::string, std::string params);
		std::string	auth(std::string prefix, std::string args);
		std::string	changeNick(std::string, std::string);
		std::string	changeUser(std::string, std::string params);

		// Channel-related stuff
		std::string	addChannel(std::string, std::string channels);
		std::string	removeChannel(std::string, std::string channelName);
		std::string	inviteToChannel(std::string, std::string params);
		std::string	topicChannel(std::string, std::string params);

		// Message-related stuff
		std::string	sendMessage(std::string, std::string channels);

		// I'm outta here
		std::string	quit(std::string, std::string params);
};
