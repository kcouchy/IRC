#!/bin/bash

# **************************************************************************** #
#                                                                              #
#                                                               ++             #
#    print_res.sh                                              +**+   +*  *    #
#                                                              ##%#*###*+++    #
#    By: aboyreau <bnzlvosnb@mozmail.com>                     +**+ -- ##+      #
#                                                             # *   *. #*      #
#    Created: 2024/07/15 23:50:23 by aboyreau          **+*+  * -_._-   #+     #
#    Updated: 2024/07/31 22:40:37 by aboyreau          +#-.-*  +         *     #
#                                                      *-.. *   ++       #     #
# **************************************************************************** #

RED='\033[0;31m'
GREEN='\033[0;32m'
WHITE='\033[0m'

if [ $# = 0 ]
then
	echo "./print_res.sh <result> <expected_result> <exit_code> <leaks>"
	echo "If the exit code is 124, the timeout message will be triggered."
	echo "If the leaks value is 1, the leaks message will be triggered."
	echo 'If $1 and $2 are different, the diff will be shown.'
	exit
fi

if [ "$4" == 1 ]
then
	printf $RED
	echo -n "LEAKS - KO"
	printf $WHITE
	exit
fi

if [ "$3" == 124 ]
then
	printf $RED
	echo -n "TIMEOUT - KO"
	printf $WHITE
	exit
fi

if [[ ! "$1" =~ "$2" ]]
then
	printf $RED
	echo -n "KO"
	echo
	diff <(echo "$1") <(echo "$2") | sed 's/^---$/--- EXPECTED : ---/g' | sed 's/.*/	\0/g'
else
	printf $GREEN
	echo -n "OK"
fi
printf $WHITE
