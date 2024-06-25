/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: atu <bnzlvosnb@mozmail.com>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/25 12:01:42 by atu               #+#    #+#             */
/*   Updated: 2024/06/25 17:36:46 by atu              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <cstring>
#include <iostream>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

int main()
{
	int bindfd;
	int socketfd;
	struct sockaddr_in address = { 0 };

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

	while (1)
	{
		struct pollfd *pfs;
		// int client;

		// client = accept(socketfd, NULL, NULL);
		// std::cout << "New client : " << client << std::endl;
		// close(client);
		// sleep(1);
	}
	
	return (0);
}
