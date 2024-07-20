#!/bin/bash

# **************************************************************************** #
#                                                                              #
#                                                               ++             #
#    pass.sh                                                   +**+   +*  *    #
#                                                              ##%#*###*+++    #
#    By: aboyreau <bnzlvosnb@mozmail.com>                     +**+ -- ##+      #
#                                                             # *   *. #*      #
#    Created: 2024/07/15 19:54:14 by aboyreau          **+*+  * -_._-   #+     #
#    Updated: 2024/07/17 01:28:17 by aboyreau          +#-.-*  +         *     #
#                                                      *-.. *   ++       #     #
# **************************************************************************** #

empty()
{
	TEST="No password on server : "
	COMMAND=`<<- EOF cat
		NICK atu
		USER atu 0 * :Arthur
		QUIT
	EOF`
	EXPECTED=":ft_irc 001 atu :Welcome here"
	$TESTDIR/utils/run_test.sh "$TEST" "" "$COMMAND" "$EXPECTED"
}

wrong()
{
	TEST="Wrong password : "
	PASSWORD="wr0ng"
	COMMAND=`<<- EOF cat
		PASS wrong
		QUIT
	EOF`
	EXPECTED=":ft_irc 464 * :Password mismatch"
	$TESTDIR/utils/run_test.sh "$TEST" "$PASSWORD" "$COMMAND" "$EXPECTED"
}

not_enough_params()
{
	TEST="Not enough params : "
	COMMAND=`<<- EOF cat
		PASS
		QUIT
	EOF`
	EXPECTED=":ft_irc 461 * PASS :Not enough parameters"
	$TESTDIR/utils/run_test.sh "$TEST" "" "$COMMAND" "$EXPECTED"
}

too_many_params()
{
	TEST="Too many params : "
	PASSWORD="a"
	COMMAND=`<<- EOF cat
		PASS a b c d
		QUIT
	EOF`
	EXPECTED=":ft_irc 464 * :Password mismatch"
	$TESTDIR/utils/run_test.sh "$TEST" "$PASSWORD" "$COMMAND" "$EXPECTED"
}

echo "#######################"
echo "# PASS command tester #"
echo "#######################"

empty
wrong
not_enough_params
too_many_params

echo
