/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   InputErrors.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kcouchma <kcouchma@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/25 15:37:51 by kcouchma          #+#    #+#             */
/*   Updated: 2024/06/25 17:43:43 by kcouchma         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef INPUT_ERRORS_HPP
#define INPUT_ERRORS_HPP

# include <iostream>
# include <exception>
# include <sstream>
# include <cstring>

# define PORT_MIN 0
# define PORT_MAX 65535

class InvalidInputException : public std::exception
{
	public:
		virtual const char *what() const throw();
};

class InvalidPortException : public std::exception
{
	public:
		virtual const char *what() const throw();
};

#endif