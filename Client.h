/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kcouchma <kcouchma@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/25 23:17:50 by atu               #+#    #+#             */
/*   Updated: 2024/06/27 14:22:07 by aboyreau         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <string>
#include "Messageable.h"

class Client : public Messageable
{
	public:
		Client (int client_fd);
		~Client(void);

		int			getfd()const;
		void		read() const;
		void		parse() const;

	private:
		int			m_fd;
		std::string	m_buffer;

		Client(void);
};
