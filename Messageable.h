/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Messageable.h                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: atu <bnzlvosnb@mozmail.com>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/27 14:12:41 by atu               #+#    #+#             */
/*   Updated: 2024/06/27 14:26:42 by aboyreau         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <string>

class Messageable
{
	public:
		Messageable();
		Messageable(const Messageable& instance);
		Messageable& operator=(const Messageable&);
		virtual void send(std::string message) = 0;
		std::string getName();

	private:
		std::string name;
};
