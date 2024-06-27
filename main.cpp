/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kcouchma <kcouchma@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/25 13:18:55 by kcouchma          #+#    #+#             */
/*   Updated: 2024/06/27 17:54:14 by aboyreau         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include "InputErrors.hpp"
#include <csignal>

//TODO ctrlC

long	check_inputs(int argc, char **argv)
{
	std::string	port_s;
	long		port_i;

	if (argc != 3)
		throw InvalidInputException();
	port_s = argv[1];
	if (port_s.find_first_not_of("0123456789") != std::string::npos)
		throw InvalidPortException();
	std::istringstream(port_s) >> port_i;
	if (port_i < PORT_MIN || port_i > PORT_MAX)
		throw InvalidPortException();
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
	return (0);
}
