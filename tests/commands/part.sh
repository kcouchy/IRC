#!/bin/bash

# **************************************************************************** #
#                                                                              #
#                                                               ++             #
#    part.sh                                                   +**+   +*  *    #
#                                                              ##%#*###*+++    #
#    By: aboyreau <bnzlvosnb@mozmail.com>                     +**+ -- ##+      #
#                                                             # *   *. #*      #
#    Created: 2024/07/29 23:01:20 by aboyreau          **+*+  * -_._-   #+     #
#    Updated: 2024/07/29 23:01:20 by aboyreau          +#-.-*  +         *     #
#                                                      *-.. *   ++       #     #
# **************************************************************************** #

no_param()
{
	TEST="PART without parameters : "
	COMMAND=`<<- EOF cat
		NICK atu
		USER atu 0 * :Arthur
		PART
		QUIT
	EOF`
	EXPECTED=`<<- EOF cat
		:ft_irc 001 atu :Welcome here
		:ft_irc 461 atu PART :Not enough parameters
	EOF`
	$TESTDIR/utils/run_test.sh "$TEST" "" "$COMMAND" "$EXPECTED"
}

nonexistent_channel()
{
	TEST="Leaving a nonexistent channel : "
	COMMAND=`<<- EOF cat
		NICK atu
		USER atu 0 * :Arthur
		PART #chan
		QUIT
	EOF`
	EXPECTED=`<<- EOF cat
		:ft_irc 001 atu :Welcome here
		:ft_irc 403 atu #chan :No such channel
	EOF`
	$TESTDIR/utils/run_test.sh "$TEST" "" "$COMMAND" "$EXPECTED"
}

leaver_not_on_channel()
{
	TEST="Leaving a channel you're not in : "
	COMMAND_1=`<<- EOF cat
		NICK kiri
		USER kiri 0 * :Kiri
		JOIN #chan
	EOF`
	COMMAND_2=`<<- EOF cat
		NICK atu
		USER atu 0 * :Arthur
		PART #chan
		QUIT
	EOF`
	EXPECTED=`<<- EOF cat
		:ft_irc 001 atu :Welcome here
		:ft_irc 442 atu #chan :You're not that channel
	EOF`
	$TESTDIR/utils/run_test_multiuser.sh "$TEST" "" "$COMMAND_1" "" "$COMMAND_2" "$EXPECTED"
}

successful_leave()
{
	TEST="Leaving a channel : "
	COMMAND=`<<- EOF cat
		NICK atu
		USER atu 0 * :Arthur
		JOIN #chan
		PART #chan
		QUIT
	EOF`
	EXPECTED=`<<- EOF cat
		:ft_irc 001 atu :Welcome here
		:atu PART #chan
	EOF`
	$TESTDIR/utils/run_test.sh "$TEST" "" "$COMMAND" "$EXPECTED"
}

# This one also tests broadcasted messages and stuff
successful_leave_reason()
{
	TEST="Leaving a channel with a reason : "
	COMMAND_1=`<<- EOF cat
		NICK kiri
		USER kiri 0 * :Kiri
		JOIN #chan
	EOF`
	COMMAND_2=`<<- EOF cat
		NICK atu
		USER atu 0 * :Arthur
		JOIN #chan
		PART #chan :see ya
		QUIT
	EOF`
	EXPECTED_1=`<<- EOF cat
		:ft_irc 001 kiri :Welcome here
		:ft_irc 332 kiri #chan :
		:ft_irc 353 kiri = #chan :@kiri
		:ft_irc 366 kiri #chan :end of /NAMES list
		:atu JOIN #chan
		:atu PART #chan :see ya
	EOF`
	EXPECTED_2=`<<- EOF cat
		:ft_irc 001 atu :Welcome here
		:ft_irc 332 atu #chan :
		:ft_irc 353 atu = #chan :@kiri atu
		:ft_irc 366 atu #chan :end of /NAMES list
		:ft_irc PART #chan :see ya
	EOF`
	$TESTDIR/utils/run_test_multiuser.sh "$TEST" "" "$COMMAND_1" "$EXPECTED_1" "$COMMAND_2" "$EXPECTED_2"
}

echo "#######################"
echo "# PART command tester #"
echo "#######################"

no_param
nonexistent_channel
leaver_not_on_channel
successful_leave
successful_leave_reason

echo
