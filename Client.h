/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kcouchma <kcouchma@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/25 23:17:50 by atu               #+#    #+#             */
/*   Updated: 2024/06/26 17:28:49 by kcouchma         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <string>
# define BUFF_SIZE 50

class Client
{
	public:
		Client (int	client_fd);
		~Client (void);

		int fd;
		// DummyClient();
		// DummyClient(const DummyClient& instance);
		// DummyClient& operator=(const DummyClient&);

	private:
		Client(void);
		std::string	buffer[BUFF_SIZE];
};
