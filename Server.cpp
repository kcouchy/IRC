/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kcouchma <kcouchma@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/26 11:42:07 by kcouchma          #+#    #+#             */
/*   Updated: 2024/06/27 18:11:45 by aboyreau         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include "PhoneBook.hpp"
#include <cstdio>

#define LISTEN 5

Server::Server(int	port, std::string password) : 
	m_port(port), m_password(password), m_socketfd(-1), m_bindfd(-1)
{
	std::cout << "Port number: " << m_port << std::endl;
	std::cout << "Password: " << m_password << std::endl;
	// Address creation
	memset(&m_address, 0, sizeof(struct sockaddr_in));
	m_address.sin_family = AF_INET;
	m_address.sin_addr.s_addr = htonl(INADDR_ANY);
	m_address.sin_port = htons(m_port);
	// m_address.sin_zero = 0;
	return ;
}

bool Server::m_run = true;

Server::~Server(void)
{
	std::cout << "test" << std::endl;
	for (std::list<Pair<int, Client *> >::iterator it = this->m_clients.begin(); it != m_clients.end(); it++)
	{
		close((*it).value->getfd());
		delete (*it).value;
	}
	close(m_bindfd);
	close(m_socketfd);
	PhoneBook::KiLl();
	return ;
}

void	Server::initialise(void)
{
	int socketOption = 1;
	// Socket creation
	this->m_socketfd = socket(m_address.sin_family, SOCK_STREAM, 0);
	if (m_socketfd == -1)
		throw SocketException();
	setsockopt(m_socketfd, SOL_SOCKET, SO_REUSEADDR, (char *) &socketOption,
        sizeof(socketOption));

	// Listen address binding
	this->m_bindfd = bind(m_socketfd, (struct sockaddr *)&m_address, sizeof(m_address));
	if (m_bindfd == -1)
		throw BindException();

	// Listen
	listen(m_socketfd, LISTEN);
}

void	Server::run(void)
{
	while (Server::m_run)
	{
		int i = 1; // 0 is reserved for the socket fd
		std::list<Pair<int, Client *> >::iterator iter = m_clients.begin();

		// pollfd structure is built from the client list every time we loop
		struct pollfd *pfs = new struct pollfd[m_clients.size() + 1];

		pfs[0].fd = m_socketfd;
		pfs[0].events = POLLIN; // I tried with POLLOUT but nothing happens on disconnection with POLLOUT
		pfs[0].revents = 0;

		while (iter != m_clients.end())
		{
			pfs[i].fd = iter->value->getfd();
			pfs[i].events = POLLIN | POLLOUT | POLLHUP; // This should change to also handle disconnection events and more
			pfs[i].revents = 0;
			iter++;
			i++;
		}

		// If the first file descriptor (socket) received an event, accept new connections (this is partially wrong as there could be disconnections too)
		std::cout << "Pollin'" << std::endl;
		usleep(500000);
		poll(pfs, i, 0); //TODO SECURE POLL
		if (pfs[0].revents & POLLIN) // Checks if there's anything incoming
		{
			std::cout << "New client : try" << std::endl;
			int client_fd = accept(m_socketfd, NULL, NULL);
			if (client_fd == -1)
				throw AcceptException();
			std::cout << "New client : " << client_fd << std::endl;
			Client *c = new Client(client_fd);
			m_clients.push_back(Pair<int, Client *>(client_fd, c));
			sleep(1); //TODO - remove at the end
		}

		// TODO iterate over clients and handle whatever they want to do
		int	j = 1;
		// for (int j = 1; j < i; j++)
		iter = m_clients.begin();
		while (j < i && iter != m_clients.end())
		{

			// read and write 
			if (pfs[j].revents & POLLIN)
			{
				std::string msg = "";
				char buf[20] = {0};
				buf[19] = 0;
				int i = 1;
				while ((i = recv(pfs[j].fd, buf, 20, 0)))
				{
					if (i == -1)
						break ;
					if (i == 0)
					{
						delete (*iter).value;
						iter = m_clients.erase(iter);
						break ;
					}
					msg += buf;
					if (i < 20 || buf[19] == '\n')
						break ;
				}
				std::cout << msg << std::endl;
				msg = "";
			}

			if (pfs[j].revents & POLLHUP)
			{
				iter = m_clients.erase(iter);
				// Note that when reading from a channel such as a pipe or a
				// stream socket, this event merely indicates that the peer
				// closed its end of the channel. Subsequent reads from the
				// channel will return 0 (end of file) only after all
				// outstanding data in the channel has been consumed.
				
				j++;
				continue ;
			}
			j++;
			iter++;
		}
		delete[] pfs;
	}
}

const char *Server::AcceptException::what() const throw()
{
	return ("Server initialisation: Accept error (New client failed to connect)");
}

const char *Server::SocketException::what() const throw()
{
	return ("Server initialisation: Socket error");
}

const char *Server::BindException::what() const throw()
{
	return ("Server initialisation: Bind error");
}
