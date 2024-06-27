/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kcouchma <kcouchma@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/27 14:56:59 by kcouchma          #+#    #+#             */
/*   Updated: 2024/06/27 15:46:11 by kcouchma         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "Channel.hpp"

Channel::Channel(std::string topic) : m_topic(topic)
{
	return ;
}

Channel::~Channel(void)
{
	return ;
}

void Channel::join(std::string client_name)
{
	m_listenList.push_back(client_name);
	return ;
}

void Channel::quit(std::string client_name)
{
	std::list<std::string>::iterator iter;
	for (iter = m_listenList.begin(); iter != m_listenList.end(); iter++)
	{
		if (*iter == client_name)
			iter = m_listenList.erase(iter);
	}
}
