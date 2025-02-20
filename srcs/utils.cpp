/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lribette <lribette@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/17 12:06:06 by aboyreau          #+#    #+#             */
/*   Updated: 2024/08/02 09:21:58 by aboyreau         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Utils.h"

std::vector<std::string> strsplit(std::string str, char delim)
{
	std::vector<std::string> splitted;
	size_t start = 0;
	size_t stop;

	stop = str.find(delim);
	while (1)
	{
		if (stop == std::string::npos)
		{
			splitted.push_back(str.substr(start, str.size()));
			break ;
		}
		splitted.push_back(str.substr(start, stop - start));
		start = stop + 1;
		while (str.at(0) == delim)
			str = str.substr(1, str.size() - 1);
		stop = str.find(delim, start);
	}
	return splitted;
}
