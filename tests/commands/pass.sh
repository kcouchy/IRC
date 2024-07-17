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

#!/bin/bash

empty()
{
	TEST="Empty password : "
	COMMAND=`<<- EOF cat
		PASS test
		QUIT
	EOF`
	EXPECTED="462"
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
	EXPECTED="464"
	$TESTDIR/utils/run_test.sh "$TEST" "$PASSWORD" "$COMMAND" "$EXPECTED"
}

not_enough_params()
{
	TEST="Not enough params : "
	COMMAND=`<<- EOF cat
		PASS
		QUIT
	EOF`
	EXPECTED="461"
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
	EXPECTED="464"
	$TESTDIR/utils/run_test.sh "$TEST" "$PASSWORD" "$COMMAND" "$EXPECTED"
}

echo "#######################"
echo "# PASS command tester #"
echo "#######################"
echo ""

empty
wrong
not_enough_params
too_many_params
