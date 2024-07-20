#!/bin/bash

# **************************************************************************** #
#                                                                              #
#                                                               ++             #
#    user.sh                                            :+:      :+:    :+:    #
#                                                              ##%#*###*+++    #
#    By: aboyreau <bnzlvosnb@mozmail.com>                     +**+ -- ##+      #
#                                                             # *   *. #*      #
#    Created: 2024/07/15 23:16:54 by aboyreau          **+*+  * -_._-   #+     #
#    Updated: 2024/07/17 11:39:35 by aboyreau         ###   ########.fr        #
#                                                      *-.. *   ++       #     #
# **************************************************************************** #

RED='\033[0;31m'
GREEN='\033[0;32m'
WHITE='\033[0m'

not_enough_params()
{
	TEST="Not enough params : "
	EXPECTED=":ft_irc 461 * USER :Not enough parameters"

	for tests in "" "test" "test 0 *" "myname :my real name" "0 * my_real_name" "atu 0 * Arthur B."
	do
		COMMAND=`<<- EOF cat
			USER $tests
			QUIT
		EOF`
		$TESTDIR/utils/run_test.sh "$TEST" "" "$COMMAND" "$EXPECTED"
	done
}

already_registered()
{
	TEST="Multiple user commands : "
	COMMAND=`<<- EOF cat
		NICK atu
		USER atu 0 * :My NAME
		USER atu 0 * :My NAME
		QUIT
	EOF`
	EXPECTED=":ft_irc 462 atu :You may not reregister"
	$TESTDIR/utils/run_test.sh "$TEST" "" "$COMMAND" "$EXPECTED"
}

echo "#######################"
echo "# USER command tester #"
echo "#######################"

not_enough_params
already_registered

echo
