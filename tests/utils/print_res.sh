# **************************************************************************** #
#                                                                              #
#                                                               ++             #
#    print_res.sh                                       :+:      :+:    :+:    #
#                                                              ##%#*###*+++    #
#    By: aboyreau <bnzlvosnb@mozmail.com>                     +**+ -- ##+      #
#                                                             # *   *. #*      #
#    Created: 2024/07/15 23:50:23 by aboyreau          **+*+  * -_._-   #+     #
#    Updated: 2024/07/17 11:39:10 by aboyreau         ###   ########.fr        #
#                                                      *-.. *   ++       #     #
# **************************************************************************** #

#!/bin/bash

RED='\033[0;31m'
GREEN='\033[0;32m'
WHITE='\033[0m'

if [ "$4" == 1 ]
then
	printf $RED
	echo "LEAKS - KO"
	printf $WHITE
	exit
fi

if [ "$3" == 124 ]
then
	printf $RED
	echo "TIMEOUT - KO"
	printf $WHITE
	exit
fi

if [[ ! "$1" =~ "$2" ]]
then
	printf $RED
	echo "KO"
else
	printf $GREEN
	echo "OK"
fi
printf $WHITE
