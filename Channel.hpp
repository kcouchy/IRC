/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.hpp                                               +**+   +*  *   */
/*                                                    +:+ +:+         +:+     */
/*   By: kcouchma <kcouchma@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/27 14:57:02 by kcouchma          #+#    #+#             */
/*   Updated: 2024/07/16 19:26:22 by aboyreau          +#-.-*  +         *    */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Messageable.h"
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
// DELETE		// std::string	topic(std::vector<std::string> args, std::string client_name);
		// void		kick(std::string client_name);
		// void		mode(std::string client_name, std::string flag);

		void		send(std::string message);
		std::string	getTopic(void)const;
		std::string	setTopic(std::string topic, std::string client_name);
		bool		getTopicProtected(void)const;
		void		setTopicProtected(bool isProtected);
		void		setOperator(std::string client_name, bool new_value);
		void		setInvite(bool inviteOnly);

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
		std::list<Pair<std::string, bool> > m_listenList; //std::string = client_name; bool = is_operator status
};
