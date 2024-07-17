/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Messageable.h                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kcouchma <kcouchma@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/27 14:12:41 by atu               #+#    #+#             */
/*   Updated: 2024/07/17 12:03:52 by kcouchma         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <string>
#include <algorithm>

#define ERR_UNKNOWNERROR "400"
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
#define ERR_USERNOTINCHANNEL "441"
#define RPL_INVITING "341" //Sent as a reply to the INVITE command to indicate that the attempt was successful and the client with the nickname <nick> has been invited to <channel>.
#define ERR_NEEDMOREPARAMS "461"
#define ERR_ALREADYREGISTERED "462"
#define ERR_PASSWDMISMATCH "464"
#define ERR_ERRONEUSNICKNAME "432"
#define RPL_NOTOPIC "331"
#define RPL_TOPIC "332"

class Messageable
{
	public:
		Messageable();
		Messageable(std::string name);
		Messageable(const Messageable& instance);
		Messageable& operator=(const Messageable&);
		virtual ~Messageable();
		
		virtual void	send(std::string message) = 0;
		std::string 	getName() const;
		void			setName(std::string);

		class RecipientNotFound : public std::exception
		{
			public:
				virtual const char *what() const throw();
		};

	protected:
		std::string m_name;
};
