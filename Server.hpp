/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kcouchma <kcouchma@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/26 11:37:22 by kcouchma          #+#    #+#             */
/*   Updated: 2024/06/27 22:58:30 by aboyreau         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <exception>
#include <list>
#include <netinet/in.h>
#include <string>
#include <unistd.h>
#include <poll.h>

#include "Pair.h"
#include "Client.h"

class Server
{
	public:
		Server(int	port, std::string password);

		~Server(void);
		// operator=[]; to get client
		
		static bool m_run;

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

		// Main server loop functions
		struct pollfd	*get_pollfd_array();
		void			accept_client(struct pollfd *pfs);
		std::string		read_message(int fd);

		// Actual server stuff
		int					m_port;
		std::string			m_password;
		std::list<Pair<int, Client *> >	m_clients;

		//socket
		int					m_socketfd;
		int					m_bindfd;
		struct sockaddr_in	m_address;
};
