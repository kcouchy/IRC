/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kcouchma <kcouchma@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/27 14:57:02 by kcouchma          #+#    #+#             */
/*   Updated: 2024/08/01 22:16:41 by aboyreau         ###   ########.fr       */
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

		std::string	join(Client *client, std::string key);
		void		quit(Client *client, bool display);

		void		invite(std::string inviter_name, std::string invitee_name);

		void		send(std::string sender_name, std::string message);
		std::string	getTopic(void)const;
		std::string	setTopic(std::string topic, Client *client);
		bool		getTopicProtected(void)const;
		bool		getInvite(void)const;
		int			getClientLimit(void)const;
		bool		getPasswordProtected(void)const;
		std::string	kick(Client* toKick, Client *client);
		std::string	mode(Client *client, bool plusminus, char modechar, std::string mode_arg);
		std::string getListenList(std::string client_name);
		bool		isInListenList(std::string client_name);

		class EmptyChannel : public std::exception
		{
			public:
				virtual const char *what() const throw();
		};

	private:
		Channel(void);
		bool		m_inviteOnly;
		bool		m_topicProtected;
		std::string	m_topic;
		int			m_clientLimit;
		std::pair<std::string, bool>		m_channelKey;
		std::list<Pair<std::string, bool> >	m_listenList; //std::string = client_name; bool = is_operator status
		std::vector<std::string>			m_inviteList;
};
