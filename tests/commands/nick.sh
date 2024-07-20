#!/bin/bash

# **************************************************************************** #
#                                                                              #
#                                                               ++             #
#    nick.sh                                                   +**+   +*  *    #
#                                                              ##%#*###*+++    #
#    By: aboyreau <bnzlvosnb@mozmail.com>                     +**+ -- ##+      #
#                                                             # *   *. #*      #
#    Created: 2024/07/15 23:08:17 by aboyreau          **+*+  * -_._-   #+     #
#    Updated: 2024/07/17 11:39:27 by kcouchma          +#-.-*  +         *     #
#                                                      *-.. *   ++       #     #
# **************************************************************************** #

RED='\033[0;31m'
GREEN='\033[0;32m'
WHITE='\033[0m'

empty()
{
	TEST="Empty nickname : "
	COMMAND=`<<- EOF cat
		NICK
		QUIT
	EOF`
	EXPECTED="431"
	$TESTDIR/utils/run_test.sh "$TEST" "" "$COMMAND" "$EXPECTED"
}

invalid()
{
	TEST="Invalid nickname : "
	EXPECTED="432"

	for nick in "#channel_like_name" "~" ":prefix"
	do
		COMMAND=`<<- EOF cat
			NICK $nick
			QUIT
		EOF`
		$TESTDIR/utils/run_test.sh "$TEST" "" "$COMMAND" "$EXPECTED"
	done
}

duplicated()
{
	TEST="Duplicated nickname : "
	PASSWORD="test"
	COMMAND_1=`<<- EOF cat
		PASS $PASSWORD
		NICK dup
		USER dup
	EOF`
	COMMAND_2=`<<- EOF cat
		PASS $PASSWORD
		NICK dup
		QUIT
	EOF`
	EXPECTED="433"

	$TESTDIR/utils/run_test_multiuser.sh "$TEST" "$PASSWORD" "$COMMAND_1" "" "$COMMAND_2" "$EXPECTED"
}

echo ""
echo "#######################"
echo "# NICK command tester #"
echo "#######################"

empty
invalid
duplicated
