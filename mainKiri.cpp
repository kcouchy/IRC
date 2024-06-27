/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   mainKiri.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kcouchma <kcouchma@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/25 13:18:55 by kcouchma          #+#    #+#             */
/*   Updated: 2024/06/27 15:03:23 by kcouchma         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Pair.h"
#include "Client.h"
#include "Server.hpp"
#include "InputErrors.hpp"

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


int	main(int argc, char **argv)
{
	int			port;
	std::string	password;

	try
	{
		port = check_inputs(argc, argv);
	}
	catch (std::exception &e)
	{
		std::cerr << e.what() << std::endl;
		return (1);
	}
	password = argv[2];
	Server	serv(port, password);

	try
	{
		serv.initialise();
	}
	catch (std::exception &e)
	{
		std::cerr << e.what() << std::endl;
		return (1);
	}

	try
	{
		serv.run();
	}
	catch (std::bad_alloc &b_a)
	{
		std::cerr << b_a.what() << std::endl;
		return (1);
	}
	catch(const std::exception& e) //differentiate from above or dump - cleanup?
	{
		std::cerr << e.what() << std::endl;
		return (1);
	}
	

	return (0);
}