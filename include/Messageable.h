/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Messageable.h                                             +**+   +*  *   */
/*                                                    +:+ +:+         +:+     */
/*   By: kcouchma <kcouchma@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/27 14:12:41 by atu               #+#    #+#             */
/*   Updated: 2024/07/30 20:08:56 by aboyreau          +#-.-*  +         *    */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <string>

#define	RPL_AWAY std::string("301")
#define	RPL_CHANNELMODEIS std::string("324")
#define RPL_NOTOPIC std::string("331")
#define RPL_TOPIC std::string("332")
#define RPL_INVITING std::string("341")
#define RPL_NAMEREPLY std::string("353")
#define RPL_ENDOFNAMES std::string("366")
#define ERR_UNKNOWNERROR std::string("400")
#define	ERR_NOSUCHNICK std::string("401")
#define	ERR_NOSUCHSERVER std::string("402")
#define ERR_NOSUCHCHANNEL std::string("403")
#define	ERR_CANNOTSENDTOCHAN std::string("404")
#define	ERR_TOOMANYTARGETS std::string("407")
#define	ERR_NORECIPIENT std::string("411")
#define	ERR_NOTEXTTOSEND std::string("412")
#define	ERR_NOTOPLEVEL std::string("413")
#define	ERR_WILDTOPLEVEL std::string("414")
#define ERR_NONICKNAMEGIVEN std::string("431")
#define ERR_ERRONEUSNICKNAME std::string("432")
#define ERR_NICKNAMEINUSE std::string("433")
#define ERR_NOTONCHANNEL std::string("442")
#define ERR_USERONCHANNEL std::string("443")
#define ERR_USERNOTINCHANNEL std::string("441")
#define ERR_NOTREGISTERED std::string("451")
#define ERR_NEEDMOREPARAMS std::string("461")
#define ERR_ALREADYREGISTERED std::string("462")
#define ERR_PASSWDMISMATCH std::string("464")
#define ERR_CHANOPRIVSNEEDED std::string("482")
#define ERR_INVALIDKEY std::string("525")
#define ERR_INVALIDMODEPARAM std::string("696")
#define ERR_CHANNELISFULL std::string("471")
#define ERR_INVITEONLYCHAN std::string("473")
#define ERR_BADCHANNELKEY std::string("475")

#define TYPE_B std::string("o")
#define TYPE_C std::string("k")
#define TYPE_D std::string("itl")

#define AUTHORISED_SET "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_[]{}\\|"

class Messageable
{
	public:
		Messageable();
		Messageable(std::string name);
		Messageable(const Messageable& instance);
		Messageable& operator=(const Messageable&);
		virtual ~Messageable();
		
		virtual void	send(std::string sender_name, std::string message) = 0;
		std::string 	getName() const;
		// void			setName(std::string);

		class RecipientNotFound : public std::exception
		{
			public:
				virtual const char *what() const throw();
		};

	protected:
		std::string m_name;
};
