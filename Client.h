/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.h                                                  +**+   +*  *   */
/*                                                    +:+ +:+         +:+     */
/*   By: kcouchma <kcouchma@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/25 23:17:50 by aboyreau               #+#    #+#             */
/*   Updated: 2024/07/15 22:42:13 by aboyreau          +#-.-*  +         *    */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <exception>
#include <vector>
#include <string>
#include "Messageable.h"

#define	ERR_NOSUCHNICK "401"
#define	ERR_NOSUCHSERVER "402"
#define ERR_NOSUCHCHANNEL "403"
#define	ERR_CANNOTSENDTOCHAN "404"
#define	ERR_TOOMANYTARGETS "407"
#define	ERR_NORECIPIENT "411"
#define	ERR_NOTEXTTOSEND "412"
#define	ERR_NOTOPLEVEL "413"
#define	ERR_WILDTOPLEVEL "414"
#define	RPL_AWAY "30"
#define ERR_NOTONCHANNEL "442" //Only members of the channel are allowed to invite other users.
#define ERR_CHANOPRIVSNEEDED "482" //SHOULD reject it when the channel has invite-only mode set, and the user is not a channel operator.
#define ERR_USERONCHANNEL "443" //If the user is already on the target channel
#define RPL_INVITING "341" //Sent as a reply to the INVITE command to indicate that the attempt was successful and the client with the nickname <nick> has been invited to <channel>.
#define ERR_NEEDMOREPARAMS "461"
#define ERR_ALREADYREGISTERED "462"
#define ERR_PASSWDMISMATCH "464"


class Client : public Messageable
{
	public:
		Client (int client_fd, std::string password);
		~Client(void);

		int		getfd() const;
		void	send(std::string msg);
		void	parse(std::string);

		class KillMePlease : public std::exception
		{
			public:
				const char *what() const throw();
		};

	private:
		int			m_fd;
		std::string	m_buffer;
		std::string	m_password;
		bool		m_authenticated;
		std::vector<std::string> m_channelList;

		Client(void);

		// Handlers
		// Find and run a handler according to the command passed as parameter.
		void	exec(std::string prefix, std::string command, std::string args);

		// Authentication
		void	capabilites(std::string, std::string params);
		void	auth(std::string prefix, std::string args);
		void	changeNick(std::string, std::string);
		void	changeUser(std::string, std::string params);

		// Channel-related stuff
		void	addChannel(std::string, std::string channels);
		void	removeChannel(std::string, std::string channelName);
		void	inviteToChannel(std::string, std::string params);

		// Message-related stuff
		void	sendMessage(std::string, std::string channels);

		// I'm outta here
		void	quit(std::string, std::string params);
};
