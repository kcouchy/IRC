/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                                  +**+   +*  *   */
/*                                                    +:+ +:+         +:+     */
/*   By: kcouchma <kcouchma@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/25 13:18:55 by kcouchma          #+#    #+#             */
/*   Updated: 2024/07/31 20:06:00 by aboyreau          +#-.-*  +         *    */
/*                                                                            */
/* ************************************************************************** */

#include "PhoneBook.hpp"
#include "Server.hpp"
#include <csignal>
#include <iostream>
#include <sstream>
#include <cstring>

long	check_inputs(int argc, char **argv)
{
	std::string	port_s;
	long		port_i;

	if (argc != 3)
		throw std::logic_error("Wrong number of input arguments\n Try: ./ircserv <port_number> <password>");
	port_s = argv[1];
	if (port_s.find_first_not_of("0123456789") != std::string::npos)
		throw std::logic_error("Invalid port number\n Try: 0 - 65535");
	std::istringstream(port_s) >> port_i;
	if (port_i < 0 || port_i > 65535)
		throw std::logic_error("Invalid port number\n Try: 0 - 65535");
	return (port_i);
}

void stop_server(int)
{
	std::cout << "stop" << std::endl;
	Server::m_run = false;
}

int	main(int argc, char **argv)
{
	int			port;
	std::string	password;
	struct sigaction sigint;

	memset(&sigint, 0, sizeof(struct sigaction));
	sigint.sa_handler = stop_server;
	sigaction(SIGINT, &sigint, NULL);
	sigaction(SIGQUIT, &sigint, NULL);
	try
	{
		port = check_inputs(argc, argv);
		password = argv[2];
		Server serv(port, password);
		serv.initialise();
		serv.run();
	}
	catch (std::bad_alloc &b_a)
	{
		std::cerr << b_a.what() << std::endl;
		return (1);
	}
	catch (std::exception &e)
	{
		std::cerr << e.what() << std::endl;
		return (1);
	}
	delete &PhoneBook::get();
	return (0);
}
