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
		:ft_irc 366 atu #chan :End of /NAMES list
		:ft_irc 461 atu INVITE :Not enough parameters
		:ft_irc 461 atu INVITE :Not enough parameters
		:ft_irc 461 atu INVITE :Not enough parameters
	EOF`
	$TESTDIR/utils/run_test.sh "$TEST" "" "$COMMAND" "$EXPECTED"
}

nonexistent_channel()
{
	TEST="Invitation to a nonexistent channel : "
	COMMAND_1=`<<- EOF cat
		NICK atu
		USER atu 0 * :Arthur
	EOF`
	COMMAND_2=`<<- EOF cat
		NICK other
		USER other 0 * :Arthur
		INVITE atu #chan
		QUIT
	EOF`
	EXPECTED=`<<- EOF cat
		:ft_irc 001 other :Welcome here
		:ft_irc 403 other #chan :No such channel
	EOF`
	$TESTDIR/utils/run_test_multiuser.sh "$TEST" "" "$COMMAND_1" "" "$COMMAND_2" "$EXPECTED"
}

notjoined_channel()
{
	TEST="Invitation to a channel you are not part of : "
	COMMAND_1=`<<- EOF cat
		NICK atu
		USER atu 0 * :Arthur
		JOIN #chan
	EOF`
	COMMAND_2=`<<- EOF cat
		NICK other
		USER other 0 * :Other
	EOF`
	COMMAND_3=`<<- EOF cat
		NICK kiri
		USER kiri 0 * :Kiri
		INVITE other #chan
		QUIT
	EOF`
	EXPECTED=`<<- EOF cat
		:ft_irc 001 kiri :Welcome here
		:ft_irc 442 kiri #chan :You're not on that channel
	EOF`
	$TESTDIR/utils/run_test_three_users.sh "$TEST" "" "$COMMAND_1" "" "$COMMAND_2" "" "$COMMAND_3" "$EXPECTED"
}

user_already_on_channel()
{
	TEST="Invitee is already on channel : "
	COMMAND_1=`<<- EOF cat
		NICK atu
		USER atu 0 * :Arthur
		#join chan
	EOF`
	COMMAND_2=`<<- EOF cat
		NICK other
		USER other 0 * :Arthur
		JOIN #chan
		INVITE atu #chan
		QUIT
	EOF`
	EXPECTED=`<<- EOF cat
		:ft_irc 001 other :Welcome here
		:ft_irc 443 other atu #chan :is already on channel
	EOF`
	$TESTDIR/utils/run_test_multiuser.sh "$TEST" "" "$COMMAND_1" "" "$COMMAND_2" "$EXPECTED"
}

valid_invitation()
{
	TEST="Valid invitation : "
	COMMAND_1=`<<- EOF cat
		NICK atu
		USER atu 0 * :Arthur
	EOF`
	COMMAND_2=`<<- EOF cat
		NICK other
		USER other 0 * :Arthur
		JOIN #chan
		INVITE atu #chan
		QUIT
	EOF`
	EXPECTED_1=`<<- EOF cat
		:ft_irc 001 atu :Welcome here
		:other INVITE atu #chan
	EOF`
	EXPECTED_2=`<<- EOF cat
		:ft_irc 001 other :Welcome here
		:other JOIN #chan
		:ft_irc 332 other #chan :
		:ft_irc 353 other = #chan :@other
		:ft_irc 366 other #chan :end of /NAMES list
		:ft_irc 341 other atu #chan
	EOF`
	$TESTDIR/utils/run_test_multiuser.sh "$TEST" "" "$COMMAND_1" "$EXPECTED_1" "$COMMAND_2" "$EXPECTED_2"
}

echo "#########################"
echo "# INVITE command tester #"
echo "#########################"

no_params
nonexistent_channel
notjoined_channel
user_already_on_channel

echo

