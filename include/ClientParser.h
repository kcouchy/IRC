/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ClientParser.h                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kcouchma <kcouchma@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/17 11:59:19 by aboyreau          #+#    #+#             */
/*   Updated: 2024/07/30 18:27:12 by kcouchma         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <climits>
#ifndef CLIENTPARSER_H
#define CLIENTPARSER_H

#include "Pair.h"
#include "Client.h"
#include <string>

class ClientParser
{
	public:
		ClientParser();
		~ClientParser();
		// void raw_parse(std::string command);

		// raw-command parser
		void parse(std::string cmd, Client &client);

	private:
		// utils
		std::string parse_postfix(std::string args);

		// command-name parser
		void parse_command(std::string prefix, std::string command, std::string param, Client &client);

		// command-specific parsers
		std::string cap(std::string prefix, std::string arsg, Client &client);
		std::string pass(std::string prefix, std::string args, Client &client);
		std::string nick(std::string prefix, std::string args, Client &client);
		std::string user(std::string prefix, std::string args, Client &client);

		std::string join(std::string prefix, std::string arsg, Client &client);
		// std::string part(std::string prefix, std::string arsg, Client &client);
		std::string invite(std::string prefix, std::string arsg, Client &client);
		std::string kick(std::string prefix, std::string arsg, Client &client);
		std::string topic(std::string prefix, std::string arsg, Client &client);
		std::string mode(std::string prefix, std::string arsg, Client &client);

		std::string privmsg(std::string prefix, std::string arsg, Client &client);

		std::string quit(std::string prefix, std::string args, Client &client);
};

typedef Pair<std::string, std::string (ClientParser::*)(std::string, std::string, Client&)> function;

#endif
