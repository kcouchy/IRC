/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kcouchma <kcouchma@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/26 16:28:03 by kcouchma          #+#    #+#             */
/*   Updated: 2024/06/26 18:11:39 by kcouchma         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.h"

Client::Client(int	client_fd) : m_fd(client_fd)
{
	return ;
}

Client::~Client(void)
{
	// delete buffer; //only if there's a new somewhere - when we join reads
	return ;
}

int Client::getfd()const
{
	return(this->m_fd);
}