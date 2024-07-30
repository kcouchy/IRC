#!/bin/bash

# **************************************************************************** #
#                                                                              #
#                                                               ++             #
#    join.sh                                                   +**+   +*  *    #
#                                                              ##%#*###*+++    #
#    By: aboyreau <bnzlvosnb@mozmail.com>                     +**+ -- ##+      #
#                                                             # *   *. #*      #
#    Created: 2024/07/20 15:25:45 by aboyreau          **+*+  * -_._-   #+     #
#    Updated: 2024/07/20 15:25:45 by aboyreau          +#-.-*  +         *     #
#                                                      *-.. *   ++       #     #
# **************************************************************************** #

no_params()
{
	TEST="Join wihout parameters : "
	COMMAND=`<<- EOF cat
		NICK atu
		USER atu 0 * :Arthur
		JOIN
		QUIT
	EOF`
	EXPECTED=`<<- EOF cat
		:ft_irc 001 atu :Welcome here
		:ft_irc 461 atu JOIN :Not enough parameters
	EOF
	`
	$TESTDIR/utils/run_test.sh "$TEST" "" "$COMMAND" "$EXPECTED"
}

unauthenticated()
{
	TEST="Unauthenticated channel join : "
	COMMAND=`<<- EOF cat
		JOIN #chan
		QUIT
	EOF`
	EXPECTED=`<<- EOF cat
		:ft_irc 451 * :You have not registered
	EOF`
	$TESTDIR/utils/run_test.sh "$TEST" "" "$COMMAND" "$EXPECTED"
}

nonexistent_channel()
{
	TEST="Nonexistent channel join : "
	COMMAND=`<<- EOF cat
		NICK atu
		USER atu 0 * :Atu
		JOIN #chan
		QUIT
	EOF`
	EXPECTED=`<<- EOF cat
		:ft_irc 001 atu :Welcome here
		:atu JOIN #chan
		:ft_irc 332 atu #chan :
		:ft_irc 353 atu = #chan :@atu
		:ft_irc 366 atu #chan :End of /NAMES list
	EOF`
	$TESTDIR/utils/run_test.sh "$TEST" "" "$COMMAND" "$EXPECTED"
}

existing_channel()
{
	TEST="Existing channel join : "
	COMMAND_1=`<<- EOF cat
		NICK op
		USER op 0 * :Operator
		JOIN #chan
	EOF`
	COMMAND_2=`<<- EOF cat
		NICK atu
		USER atu 0 * :Atu
		JOIN #chan
		QUIT
	EOF`
	EXPECTED=`<<- EOF cat
		:ft_irc 001 atu :Welcome here
		:atu JOIN #chan
		:ft_irc 332 atu #chan :
		:ft_irc 353 atu = #chan :@op atu
		:ft_irc 366 atu #chan :End of /NAMES list
	EOF`
	$TESTDIR/utils/run_test_multiuser.sh "$TEST" "" "$COMMAND_1" "" "$COMMAND_2" "$EXPECTED"
}

multiple_join()
{
	TEST="Multiple channel join : "
	COMMAND=`<<- EOF cat
		NICK atu
		USER atu 0 * :Atu
		JOIN #chan,#test,#tmp
		QUIT
	EOF`
	EXPECTED=`<<- EOF cat
		:ft_irc 001 atu :Welcome here
		:atu JOIN #chan
		:ft_irc 332 atu #chan :
		:ft_irc 353 atu = #chan :@atu
		:ft_irc 366 atu #chan :End of /NAMES list
		:atu JOIN #test
		:ft_irc 332 atu #test :
		:ft_irc 353 atu = #test :@atu
		:ft_irc 366 atu #test :End of /NAMES list
		:atu JOIN #tmp
		:ft_irc 332 atu #tmp :
		:ft_irc 353 atu = #tmp :@atu
		:ft_irc 366 atu #tmp :End of /NAMES list
	EOF`
	$TESTDIR/utils/run_test.sh "$TEST" "" "$COMMAND" "$EXPECTED"
}

name_conflict()
{
	TEST="Channel that can't be created : "
	COMMAND=`<<- EOF cat
		NICK atu
		USER atu 0 * :Arthur
		JOIN atu
		QUIT
	EOF`
	EXPECTED=`<<- EOF cat
		:ft_irc 001 atu :Welcome here
		:ft_irc 403 atu atu :No such channel
	EOF`
	$TESTDIR/utils/run_test.sh "$TEST" "" "$COMMAND" "$EXPECTED"
}

leave_everything()
{
	TEST="JOIN 0 should leave everything : "
	COMMAND=`<<- EOF cat
		NICK atu
		USER atu 0 * :Arthur
		JOIN #chan,#test,#tmp
		JOIN 0
		QUIT
	EOF`
	EXPECTED=`<<- EOF cat
		:ft_irc 001 atu :Welcome here
		:atu JOIN #chan
		:ft_irc 332 atu #chan :
		:ft_irc 353 atu = #chan :@atu
		:ft_irc 366 atu #chan :End of /NAMES list
		:atu JOIN #test
		:ft_irc 332 atu #test :
		:ft_irc 353 atu = #test :@atu
		:ft_irc 366 atu #test :End of /NAMES list
		:atu JOIN #tmp
		:ft_irc 332 atu #tmp :
		:ft_irc 353 atu = #tmp :@atu
		:ft_irc 366 atu #tmp :End of /NAMES list
		:atu PART #chan
		:atu PART #test
		:atu PART #tmp
	EOF`
	$TESTDIR/utils/run_test.sh "$TEST" "" "$COMMAND" "$EXPECTED"
}

echo "#######################"
echo "# JOIN command tester #"
echo "#######################"

unauthenticated
nonexistent_channel
existing_channel
multiple_join
name_conflict
leave_everything

echo
