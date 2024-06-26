/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kcouchma <kcouchma@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/25 23:17:50 by atu               #+#    #+#             */
/*   Updated: 2024/06/26 18:10:34 by kcouchma         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <string>

class Client
{
	public:
		Client (int	client_fd);
		~Client (void);

		// DummyClient();
		// DummyClient(const DummyClient& instance);
		// DummyClient& operator=(const DummyClient&);
		int getfd()const;

	private:
		Client(void);
		int m_fd;
		// std::string	buffer;
};
