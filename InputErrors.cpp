/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   InputErrors.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kcouchma <kcouchma@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/25 15:45:19 by kcouchma          #+#    #+#             */
/*   Updated: 2024/06/25 17:27:08 by kcouchma         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "InputErrors.hpp"

const char *InvalidInputException::what() const throw()
{
	return ("Wrong number of input arguments\n Try: ./ircserv <port_number> <password>");
}

const char *InvalidPortException::what() const throw()
{
	// std::stringstream mes;
	// std::string message;

	// mes << "Invalid port number (" << PORT_MIN << " - " << PORT_MAX << ")";
	// message = mes.str();
	// std::cout << message << std::endl;
	// const char	*message_c = message.c_str();
	// return (message_c);
	return ("Invalid port number\n Try: 0 - 65535");
}