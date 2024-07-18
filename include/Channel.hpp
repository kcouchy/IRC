/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kcouchma <kcouchma@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/27 14:57:02 by kcouchma          #+#    #+#             */
/*   Updated: 2024/07/18 15:23:57 by kcouchma         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Messageable.h"
#include "Client.h"
#include "Pair.h"
#include <list>

class Channel : public Messageable
{
	public:
		Channel(std::string channelName);
		~Channel(void);

		void		join(std::string client_name);
		std::string	quit(std::string client_name);

		std::string	invite(std::string inviter_name, std::string invitee_name);
		// void		mode(std::string client_name, std::string flag);

		void		send(std::string message);
		std::string	getTopic(void)const;
		std::string	setTopic(std::string topic, std::string client_name);
		bool		getTopicProtected(void)const;
		void		setTopicProtected(bool isProtected);
		void		setOperator(std::string client_name, bool new_value);
		void		setInvite(bool inviteOnly);
		std::string	kick(Client* toKick, std::string kicker);
		std::string	mode(std::string client_name, bool plusminus, char modechar, std::string mode_arg);

		class EmptyChannel : public std::exception
		{
			public:
				virtual const char *what() const throw();
		};
		// class UserOnChannel : public std::exception
		// {
		// 	public:
		// 		virtual const char *what() const throw();
		// };
		class NotOnChannel : public std::exception
		{
			public:
				virtual const char *what() const throw();
		};
		// class NotOperator : public std::exception
		// {
		// 	public:
		// 		virtual const char *what() const throw();
		// };

	private:
		Channel(void);
		bool		m_inviteOnly;
		bool		m_topicProtected;
		std::string	m_topic;
		int			m_clientLimit;
		std::pair<std::string, bool>		m_channelKey;//channel password
		std::list<Pair<std::string, bool> >	m_listenList; //std::string = client_name; bool = is_operator status
		std::vector<std::string>			m_inviteList;
};
