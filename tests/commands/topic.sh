# **************************************************************************** #
#                                                                              #
#                                                               ++             #
#    topic.sh                                                  +**+   +*  *    #
#                                                              ##%#*###*+++    #
#    By: aboyreau <bnzlvosnb@mozmail.com>                     +**+ -- ##+      #
#                                                             # *   *. #*      #
#    Created: 2024/07/16 23:11:32 by aboyreau          **+*+  * -_._-   #+     #
#    Updated: 2024/07/16 23:34:18 by aboyreau          +#-.-*  +         *     #
#                                                      *-.. *   ++       #     #
# **************************************************************************** #

#!/bin/bash

nonexistent_channel()
{
	TEST="Topic change on a nonexistent channel : "
	PASSWORD="test"
	COMMAND=`<<- EOF cat
		PASS $PASSWORD
		NICK atu
		USER atu 0 * :Atu
		TOPIC #chan :Hello world
		QUIT
	EOF`
	EXPECTED="442"
	$TESTDIR/utils/run_test.sh "$TEST" "$PASSWORD" "$COMMAND" "$EXPECTED"
}

not_on_channel()
{
	TEST="Topic change on a channel which is not joined : "
	PASSWORD="test"
	COMMAND_1=`<<-EOF cat
		PASS $PASSWORD
		NICK other
		USER other 0 * :Other
		JOIN #chan
	EOF`
	COMMAND_2=`<<- EOF cat
		PASS $PASSWORD
		NICK atu
		USER atu 0 * :Atu
		TOPIC #chan :Hello world
		QUIT
	EOF`
	EXPECTED_1="*"
	EXPECTED_2="442"
	$TESTDIR/utils/run_test_multiuser.sh "$TEST" "$PASSWORD" "$COMMAND_1" "$EXPECTED_1" "$COMMAND_2" "$EXPECTED_2"
}

echo "########################"
echo "# TOPIC command tester #"
echo "########################"
echo ""

nonexistent_channel
not_on_channel
empty
change
unauthorized
removal

rm tmp
