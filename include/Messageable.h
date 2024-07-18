/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Messageable.h                                             +**+   +*  *   */
/*                                                    +:+ +:+         +:+     */
/*   By: kcouchma <kcouchma@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/27 14:12:41 by atu               #+#    #+#             */
/*   Updated: 2024/07/18 15:35:43 by aboyreau          +#-.-*  +         *    */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <string>

#define	RPL_AWAY "30 :RPL_AWAY"
#define	RPL_CHANNELMODEIS "324 :RPL_CHANNELMODEIS"
#define RPL_NOTOPIC "331 :RPL_NOTOPIC"
#define RPL_TOPIC "332 :RPL_TOPIC"
#define RPL_INVITING "341 :RPL_INVITING"
#define ERR_UNKNOWNERROR "400 :ERR_UNKNOWNERROR"
#define	ERR_NOSUCHNICK "401 :ERR_NOSUCHNICK"
#define	ERR_NOSUCHSERVER "402 :ERR_NOSUCHSERVER"
#define ERR_NOSUCHCHANNEL "403 :ERR_NOSUCHCHANNEL"
#define	ERR_CANNOTSENDTOCHAN "404 :ERR_CANNOTSENDTOCHAN"
#define	ERR_TOOMANYTARGETS "407 :ERR_TOOMANYTARGETS"
#define	ERR_NORECIPIENT "411 :ERR_NORECIPIENT"
#define	ERR_NOTEXTTOSEND "412 :ERR_NOTEXTTOSEND"
#define	ERR_NOTOPLEVEL "413 :ERR_NOTOPLEVEL"
#define	ERR_WILDTOPLEVEL "414 :ERR_WILDTOPLEVEL"
#define ERR_NONICKNAMEGIVEN "431 :ERR_NONICKNAMEGIVEN"
#define ERR_ERRONEUSNICKNAME "432 :ERR_ERRONEUSNICKNAME"
#define ERR_NICKNAMEINUSE "433 :ERR_NICKNAMEINUSE"
#define ERR_NOTONCHANNEL "442 :ERR_NOTONCHANNEL"
#define ERR_USERONCHANNEL "443 :ERR_USERONCHANNEL"
#define ERR_USERNOTINCHANNEL "441 :ERR_USERNOTINCHANNEL"
#define ERR_NEEDMOREPARAMS "461 :ERR_NEEDMOREPARAMS"
#define ERR_ALREADYREGISTERED "462 :ERR_ALREADYREGISTERED"
#define ERR_PASSWDMISMATCH "464 :ERR_PASSWDMISMATCH"
#define ERR_CHANOPRIVSNEEDED "482 :ERR_CHANOPRIVSNEEDED"
#define ERR_INVALIDKEY "525: ERR_INVALIDKEY"
#define ERR_INVALIDMODEPARAM "696: ERR_INVALIDMODEPARAM"

#define AUTHORISED_SET "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_[]{}\\|"

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
