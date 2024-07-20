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

	for tests in "" "test" "test 0 *" "myname :my real name" "0 * my_real_name"
	do
		COMMAND=`<<- EOF cat
			USER $tests
			QUIT
		EOF`
		$TESTDIR/utils/run_test.sh "$TEST" "" "$COMMAND" "461"

	done
}

already_registered()
{
	COMMAND=`<<- EOF cat
		NICK atu
		USER atu 0 * :My NAME
		USER atu 0 * :My NAME
		QUIT
	EOF`

	$TESTDIR/utils/run_test.sh "Multiple user commands : " "" "$COMMAND" "462"
}

echo ""
echo "#######################"
echo "# USER command tester #"
echo "#######################"

not_enough_params
already_registered
