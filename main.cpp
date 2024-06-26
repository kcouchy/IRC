/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kcouchma <kcouchma@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/25 12:01:42 by atu               #+#    #+#             */
/*   Updated: 2024/06/26 14:59:56 by kcouchma         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <cstdio>
#include <poll.h>
#include <netinet/in.h>
#include <sys/poll.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <list>
#include <cstdio>
#include <iostream>

#include "Pair.h"
#include "Client.h"

int main(int argc, char **argv)
{
	// class private attributes
	int bindfd;
	int socketfd;
	struct sockaddr_in address = { 0, 0, 0, 0 };

	// Constructor ?
	{
		// Address creation
		address.sin_family = AF_INET;
		address.sin_addr.s_addr = htonl(INADDR_ANY);
		address.sin_port = htons(3030);

		// Socket creation
		socketfd = socket(address.sin_family, SOCK_STREAM, 0);
		if (socketfd == -1)
		{
			perror("socket");
			return (-1);
		}

		// Listen address binding
		bindfd = bind(socketfd, (struct sockaddr *) &address, sizeof(address));
		if (bindfd == -1)
		{
			perror("bind");
			close(socketfd);
			return (-1);
		}

		// Listen
		listen(socketfd, 5);
	}


	// main loop code (a server method ?)
	{
		std::list<Pair<int, Client> > clients; // class private attribute too

		while (1)
		{
			int i = 1; // 0 is reserved for the socket fd
			std::list<Pair<int, Client> >::iterator iter = clients.begin();

			// pollfd structure is built from the client list every time we loop
			// I'm trying some nothrow stuff to make it return NULL instead of throwing a bad_alloc exception
			struct pollfd *pfs;
			try
			{
				pfs = new struct pollfd[clients.size() + 1];
			}
			catch (std::bad_alloc &b_a)
			{
				std::cerr << b_a.what() << std::endl;
				return (1); //TODO make sure we didn't add something that we need to delete first
			}
			pfs[0].fd = socketfd;
			pfs[0].events = POLLIN; // I tried with POLLOUT but nothing happens on disconnection with POLLOUT
			pfs[0].revents = 0;
			while (iter != clients.end())
			{
				pfs[i].fd = iter->value.fd;
				pfs[i].events = POLLIN | POLLOUT; // This should change to also handle disconnection events and more
				pfs[i].revents = 0;
				iter++;
				i++;
			}

			// If the first file descriptor (socket) received an event, accept new connections (this is partially wrong as there could be disconnectons too)
			std::cout << "Pollin'" << std::endl;
			poll(pfs, i, 2000);
			if (pfs[0].revents & POLLIN) // Checks if there's anything incoming
			{
				int client = accept(socketfd, NULL, NULL);
				if (client == -1)
				{
					throw("New client failed to connect");
					break ;
				}
				std::cout << "New client : " << client << std::endl;
				Client c = Client();
				c.fd = client;
				clients.push_back(Pair<int, Client>(client, c));
				sleep(1);
			}
			
			// TODO iterate over clients and handle whatever they want to do
			for (int j = 1; j < i; j++)
			{
				std::cout << (j) << std::endl;
				j++;
			}
			delete[] pfs;
		}
	}

	// Cleaning my mess up (server destructor ?)
	{
		close(bindfd);
		close(socketfd);
	}
	
	return (0);
}
