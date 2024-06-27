/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kcouchma <kcouchma@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/26 11:37:22 by kcouchma          #+#    #+#             */
/*   Updated: 2024/06/27 13:49:42 by kcouchma         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <iostream>
#include <exception>
#include <list>
#include <netinet/in.h>
#include <string>
#include <unistd.h>
#include <poll.h>
#include <cstring>

#include "Pair.h"
#include "Client.h"

class Server
{
	public:
		Server(int	port, std::string password);

		~Server(void);
		// operator=[]; to get client

		void	initialise(void);
		void	run(void);

		class AcceptException : public std::exception{
			public:
				virtual const char *what() const throw();
		};
		class SocketException : public std::exception{
			public:
				virtual const char *what() const throw();
		};
		class BindException : public std::exception{
			public:
				virtual const char *what() const throw();
		};

	private:
		Server(void);
		int					m_port;
		std::string			m_password;
		std::list<Pair<int, Client *> >	m_clients;

		//socket
		int					m_socketfd;
		int					m_bindfd;
		struct sockaddr_in	m_address;
};