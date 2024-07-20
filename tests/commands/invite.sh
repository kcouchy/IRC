#!/bin/bash

# **************************************************************************** #
#                                                                              #
#                                                               ++             #
#    invite.sh                                                 +**+   +*  *    #
#                                                              ##%#*###*+++    #
#    By: aboyreau <bnzlvosnb@mozmail.com>                     +**+ -- ##+      #
#                                                             # *   *. #*      #
#    Created: 2024/07/20 16:56:29 by aboyreau          **+*+  * -_._-   #+     #
#    Updated: 2024/07/20 16:56:29 by aboyreau          +#-.-*  +         *     #
#                                                      *-.. *   ++       #     #
# **************************************************************************** #

no_params()
{
	TEST="Invite wihout enough parameters : "
	COMMAND=`<<- EOF cat
		NICK atu
		USER atu 0 * :Arthur
		JOIN #chan
		INVITE
		INVITE user
		INVITE #chan
		QUIT
	EOF`
	EXPECTED=`<<- EOF cat
		:ft_irc 001 atu :Welcome here
		:atu JOIN #chan
		:ft_irc 332 atu #chan :
		:ft_irc 353 atu = #chan :@atu
		:ft_irc 366 atu #chan :end of /NAMES list
		:ft_irc 461 atu INVITE :Not enough parameters
		:ft_irc 461 atu INVITE :Not enough parameters
		:ft_irc 461 atu INVITE :Not enough parameters
	EOF`
	$TESTDIR/utils/run_test.sh "$TEST" "" "$COMMAND" "$EXPECTED"
}

echo "#########################"
echo "# INVITE command tester #"
echo "#########################"

no_params

echo

