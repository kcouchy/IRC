/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Messageable.h                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kcouchma <kcouchma@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/27 14:12:41 by atu               #+#    #+#             */
/*   Updated: 2024/06/29 14:43:24 by aboyreau         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <string>

class Messageable
{
	public:
		Messageable();
		Messageable(std::string name);
		Messageable(const Messageable& instance);
		Messageable& operator=(const Messageable&);
		virtual ~Messageable();
		
		virtual void send(std::string message) = 0;
		std::string getName() const;
		void setName(std::string);

	protected:
		std::string m_name;
};
