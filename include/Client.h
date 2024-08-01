/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kcouchma <kcouchma@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/25 23:17:50 by aboyreau          #+#    #+#             */
/*   Updated: 2024/08/01 22:03:03 by aboyreau         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <exception>
#include <list>
#include <vector>
#include <string>

#include "Messageable.h"

class Client : public Messageable
{
	public:
		Client (int client_fd, std::string password);
		~Client(void);

		int		getfd() const;
		void	send(std::string, std::string msg);
		bool	is_registered();
		std::string getIdentifier() const;

		class KillMePlease : public std::exception
		{
			public:
				const char *what() const throw();
		};

	private:

		int							m_fd;
		std::string					m_buffer;
		std::string					m_password;
		std::list<std::string>		m_channelList;
		bool						m_authenticated;
		bool						m_userSet;
		bool 						m_registrationComplete;
		std::string					m_identifier;

		Client(void);

	public:
		// Authentication
		std::string	capabilites(std::string, std::string params);
		std::string	auth(std::string password);
		std::string	changeNick(std::string, std::string);
		std::string	changeUser(std::string, std::string params);
		void 		clientRegistered(void);


		// Channel-related stuff
		std::string	joinChannel(std::string channel, std::string key);
		void		removeChannel(std::string, std::string channelName, bool display);
		void		removeAllChannels(void);
		std::string	inviteToChannel(std::string, std::string params);
		std::string	topicChannel(std::string);
		std::string	topicChannel(std::string, std::string params);
		std::string	kickChannel(std::string channel_name, std::string client_name, std::string kick_msg);
		void		getMode(std::string channel_name);
		std::string	modeChannel(std::string client_name, bool plusminus, char modechar, std::string mode_arg);

		// Message-related stuff
		std::string	getBuffer(void)const;
		void		addBuffer(std::string buffer);
		void		clearBuffer(void);
		std::string	sendMessage(std::string, std::string channels);

		// I'm outta here
		std::string	quit(std::string, std::string params);
};

