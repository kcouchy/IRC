/* ************************************************************************** */
/*                                                                            */
/*                                                              ++            */
/*   ClientParser.h                                            +**+   +*  *   */
/*                                                             ##%#*###*+++   */
/*   By: aboyreau <bnzlvosnb@mozmail.com>                     +**+ -- ##+     */
/*                                                            # *   *. #*     */
/*   Created: 2024/07/17 11:59:19 by aboyreau          **+*+  * -_._-   #+    */
/*   Updated: 2024/07/17 14:06:41 by aboyreau          +#-.-*  +         *    */
/*                                                     *-.. *   ++       #    */
/* ************************************************************************** */

#pragma once

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
		void raw_parse(std::string command);

		// raw-command parser
		void parse(std::string cmd, Client &client);

	private:
		// utils
		std::string parse_postfix(std::string args);

		// command-name parser
		void parse_command(std::string prefix, std::string command, std::string param, Client &client);

		// command-specific parsers
		std::string kick(std::string prefix, std::string arsg, Client &client);
		std::string cap(std::string prefix, std::string arsg, Client &client);
};

typedef Pair<std::string, std::string (ClientParser::*)(std::string, std::string, Client&)> function;

#endif

