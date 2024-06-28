/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kcouchma <kcouchma@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/26 11:42:07 by kcouchma          #+#    #+#             */
/*   Updated: 2024/06/28 11:49:01 by aboyreau         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <cstdio>
#include <cstring>
#include <exception>
#include <iostream>

#include "Server.hpp"
#include "PhoneBook.hpp"

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
	std::cout << "End of the server" << std::endl;
	for (std::list<Pair<int, Client *> >::iterator it = this->m_clients.begin(); it != m_clients.end(); it++)
	{
		std::cout << "Deleting " << (*it).value->getfd() << std::endl;
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

struct pollfd	*Server::get_pollfd_array()
{
	int i = 1; // 0 is reserved for the socket fd
	std::list<Pair<int, Client *> >::iterator iter = m_clients.begin();
	struct pollfd *pollfd_array = new struct pollfd[m_clients.size() + 1];

	pollfd_array[0].fd = m_socketfd;
	pollfd_array[0].events = POLLIN;
	pollfd_array[0].revents = 0;

	while (iter != m_clients.end())
	{
		pollfd_array[i].fd = iter->value->getfd();
		pollfd_array[i].events = POLLIN | POLLOUT | POLLHUP;
		pollfd_array[i].revents = 0;
		iter++;
		i++;
	}
	return pollfd_array;
}

void	Server::accept_client(struct pollfd *pfs)
{
	int client_fd = -1;

	if (pfs[0].revents & POLLIN) // Checks if there's anything incoming
	{
		client_fd = accept(m_socketfd, NULL, NULL);
		if (client_fd == -1)
			throw AcceptException();
		Client *c = new Client(client_fd);
		this->m_clients.push_back(Pair<int, Client *>(client_fd, c));
	}
}

std::string	Server::read_message(int fd)
{
	int i = 1;
	char buf[20];
	std::string msg = "";

	memset(buf, 0, 20);
	while (1)
	{
		i = recv(fd, buf, 20, 0);
		if (i == -1 || i == 0)
			throw std::exception();
		buf[i] = '\0';
		msg += std::string(buf);
		if (i < 20 || buf[19] == '\n')
			return msg;
	}
	return msg;
}

void	Server::run(void)
{
	size_t	pfs_size;
	struct	pollfd *pfs;
	std::list<Pair<int, Client *> >::iterator iter;

	while (Server::m_run)
	{
		usleep(500000);

		pfs = this->get_pollfd_array();
		pfs_size = m_clients.size(); // Must be stored here because if we accept a new client we're gonna invalid read in this array later

		std::cout << "Pollin'" << std::endl;
		if (poll(pfs, m_clients.size() + 1, 0) == -1)
		{
			delete[] pfs;
			perror("poll");
			throw std::exception();
		}

		this->accept_client(pfs);

		// this->handle_clients_messages(); // TODO create this and move the code that triggers client events here
		iter = m_clients.begin();
		size_t	j = 1;
		while (j <= pfs_size && iter != m_clients.end()) // If we accepted, m_clients.size() is now equal to pfs_size + 2 instead of + 1
		{
			std::string msg;
			// read and write 
			if (pfs[j].revents & POLLIN)
			{
				try
				{
					msg = this->read_message(pfs[j].fd);
					(*iter).value->parse(msg);
				}
				catch (std::exception& e)
				{
					delete[] pfs;
					delete (*iter).value;
					iter = m_clients.erase(iter);
					throw std::exception();
				}
				std::cout << msg << std::endl;
				msg = "";
			}

			if (pfs[j].revents & POLLHUP)
			{
				delete (*iter).value;
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
