# **************************************************************************** #
#                                                                              #
#                                                               ++             #
#    print_res.sh                                              +**+   +*  *    #
#                                                              ##%#*###*+++    #
#    By: aboyreau <bnzlvosnb@mozmail.com>                     +**+ -- ##+      #
#                                                             # *   *. #*      #
#    Created: 2024/07/15 23:50:23 by aboyreau          **+*+  * -_._-   #+     #
#    Updated: 2024/07/20 16:17:32 by aboyreau          +#-.-*  +         *     #
#                                                      *-.. *   ++       #     #
# **************************************************************************** #

#!/bin/bash

RED='\033[0;31m'
GREEN='\033[0;32m'
WHITE='\033[0m'

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
else
	printf $GREEN
	echo -n "OK"
fi
printf $WHITE
