#!/bin/bash

# **************************************************************************** #
#                                                                              #
#                                                               ++             #
#    topic.sh                                           :+:      :+:    :+:    #
#                                                              ##%#*###*+++    #
#    By: aboyreau <bnzlvosnb@mozmail.com>                     +**+ -- ##+      #
#                                                             # *   *. #*      #
#    Created: 2024/07/16 23:11:32 by aboyreau          **+*+  * -_._-   #+     #
#    Updated: 2024/07/17 01:54:48 by aboyreau          +#-.-*  +         *     #
#                                                      *-.. *   ++       #     #
# **************************************************************************** #

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

no_params()
{
	TEST="Topic without argument : "
	PASSWORD="test"
	COMMAND=`<<- EOF cat
		PASS $PASSWORD
		NICK atu
		USER atu 0 * :Atu
		TOPIC
		QUIT
	EOF`
	EXPECTED="461"
	$TESTDIR/utils/run_test.sh "$TEST" "$PASSWORD" "$COMMAND" "$EXPECTED"
}

change_topic()
{
	TEST="Change topic : "
	PASSWORD="test"
	COMMAND=`<<- EOF cat
		PASS $PASSWORD
		NICK atu
		USER atu 0 * :Atu
		JOIN #chan
		TOPIC #chan :test
		QUIT
	EOF`
	EXPECTED="332 #chan :test"
	$TESTDIR/utils/run_test.sh "$TEST" "$PASSWORD" "$COMMAND" "$EXPECTED"
}

get_topic()
{
	TEST="Get topic : "
	PASSWORD="test"
	COMMAND_1=`<<- EOF cat
		PASS $PASSWORD
		NICK atu
		USER atu 0 * :Atu
		JOIN #chan
		TOPIC #chan :test
	EOF`
	COMMAND_2=`<<- EOF cat
		PASS $PASSWORD
		NICK other
		USER other 0 * :Other
		JOIN #chan
		TOPIC #chan
		QUIT
	EOF`
	EXPECTED="332 #chan :test"
	$TESTDIR/utils/run_test_multiuser.sh "$TEST" "$PASSWORD" "$COMMAND_1" "" "$COMMAND_2" "$EXPECTED"
}

removal()
{
	TEST="Get topic : "
	PASSWORD="test"
	COMMAND_1=`<<- EOF cat
		PASS $PASSWORD
		NICK atu
		USER atu 0 * :Atu
		JOIN #chan
		TOPIC #chan :test
	EOF`
	COMMAND_2=`<<- EOF cat
		PASS $PASSWORD
		NICK other
		USER other 0 * :Other
		JOIN #chan
		TOPIC #chan :
		QUIT
	EOF`
	EXPECTED="332 #chan :"
	$TESTDIR/utils/run_test_multiuser.sh "$TEST" "$PASSWORD" "$COMMAND_1" "" "$COMMAND_2" "$EXPECTED"
}

unauthorized()
{
	TEST="Unauthorized topic change (YOU NEED MODE) : "
	PASSWORD="test"
	COMMAND_1=`<<- EOF cat
		PASS $PASSWORD
		NICK atu
		USER atu 0 * :Atu
		JOIN #chan
		TOPIC #chan :test
		MODE +t #chan
	EOF`
	COMMAND_2=`<<- EOF cat
		PASS $PASSWORD
		NICK other
		USER other 0 * :Other
		JOIN #chan
		TOPIC #chan :new
		QUIT
	EOF`
	EXPECTED="482 #chan"
	$TESTDIR/utils/run_test_multiuser.sh "$TEST" "$PASSWORD" "$COMMAND_1" "" "$COMMAND_2" "$EXPECTED"
}

echo ""
echo "########################"
echo "# TOPIC command tester #"
echo "########################"

nonexistent_channel
not_on_channel
no_params
change_topic
get_topic
removal
unauthorized
