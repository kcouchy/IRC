#!/bin/bash

# **************************************************************************** #
#                                                                              #
#                                                               ++             #
#    kick.sh                                                   +**+   +*  *    #
#                                                              ##%#*###*+++    #
#    By: aboyreau <bnzlvosnb@mozmail.com>                     +**+ -- ##+      #
#                                                             # *   *. #*      #
#    Created: 2024/07/20 21:15:55 by aboyreau          **+*+  * -_._-   #+     #
#    Updated: 2024/07/20 21:15:55 by aboyreau          +#-.-*  +         *     #
#                                                      *-.. *   ++       #     #
# **************************************************************************** #

no_param()
{
	TEST="Kick without parameters : "
	COMMAND=`<<- EOF cat
		NICK atu
		USER atu 0 * :Arthur
		KICK
		QUIT
	EOF`
	EXPECTED=`<<- EOF cat
		:ft_irc 001 atu :Welcome here
		:ft_irc 461 atu KICK :Not enough parameters
	EOF`
	$TESTDIR/utils/run_test.sh "$TEST" "" "$COMMAND" "$EXPECTED"
}

nonexistent_channel()
{
	TEST="Kick from a nonexistent channel : "
	COMMAND=`<<- EOF cat
		NICK atu
		USER atu 0 * :Arthur
		KICK #chan atu
		QUIT
	EOF`
	EXPECTED=`<<- EOF cat
		:ft_irc 001 atu :Welcome here
		:ft_irc 403 atu #chan :No such channel
	EOF`
	$TESTDIR/utils/run_test.sh "$TEST" "" "$COMMAND" "$EXPECTED"
}

unauthorized_kick()
{
	TEST="Kick without permission : "
	COMMAND_1=`<<- EOF cat
		NICK kiri
		USER kiri 0 * :Kiri
		JOIN #chan
	EOF`
	COMMAND_2=`<<- EOF cat
		NICK atu
		USER atu 0 * :Arthur
		JOIN #chan
		KICK #chan kiri
		QUIT
	EOF`
	EXPECTED=`<<- EOF cat
		:ft_irc 001 atu :Welcome here
		:ft_irc 482 atu #chan :You're not channel operator
	EOF`
	$TESTDIR/utils/run_test_multiuser.sh "$TEST" "" "$COMMAND_1" "" "$COMMAND_2" "$EXPECTED"
}

kicker_not_on_channel()
{
	TEST="Kick without permission : "
	COMMAND_1=`<<- EOF cat
		NICK kiri
		USER kiri 0 * :Kiri
		JOIN #chan
	EOF`
	COMMAND_2=`<<- EOF cat
		NICK atu
		USER atu 0 * :Arthur
		KICK #chan kiri
		QUIT
	EOF`
	EXPECTED=`<<- EOF cat
		:ft_irc 001 atu :Welcome here
		:ft_irc 442 atu #chan :You're not that channel
	EOF`
	$TESTDIR/utils/run_test_multiuser.sh "$TEST" "" "$COMMAND_1" "" "$COMMAND_2" "$EXPECTED"
}

kicked_not_on_channel()
{
	TEST="Kicked user is not on channel : "
	COMMAND_1=`<<- EOF cat
		NICK kiri
		USER kiri 0 * :Kiri
		JOIN #chan
		KICK #chan atu
		QUIT
	EOF`
	EXPECTED=`<<- EOF cat
		:ft_irc 001 kiri :Welcome here
		:ft_irc 441 kiri atu #chan :They aren't on that channel
	EOF`
	$TESTDIR/utils/run_test.sh "$TEST" "" "$COMMAND" "$EXPECTED"
}

no_message()
{
	TEST="Kick without message : "
	COMMAND_1=`<<- EOF cat
		NICK kiri
		USER kiri 0 * :Kiri
		JOIN #chan
	EOF`
	COMMAND_2=`<<- EOF cat
		NICK atu
		USER atu 0 * :Arthur
		JOIN #chan
		KICK #chan kiri
		QUIT
	EOF`
	EXPECTED=`<<- EOF cat
		:ft_irc 001 atu :Welcome here
		:ft_irc 442 atu #chan :You're not that channel
	EOF`
	$TESTDIR/utils/run_test_multiuser.sh "$TEST" "" "$COMMAND_1" "" "$COMMAND_2" "$EXPECTED"
}

echo "#######################"
echo "# KICK command tester #"
echo "#######################"

no_param
nonexistent_channel
unauthorized_kick
kicker_not_on_channel
kicked_not_on_channel
no_message

echo
