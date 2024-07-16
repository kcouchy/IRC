# **************************************************************************** #
#                                                                              #
#                                                               ++             #
#    print_res.sh                                              +**+   +*  *    #
#                                                              ##%#*###*+++    #
#    By: aboyreau <bnzlvosnb@mozmail.com>                     +**+ -- ##+      #
#                                                             # *   *. #*      #
#    Created: 2024/07/15 23:50:23 by aboyreau          **+*+  * -_._-   #+     #
#    Updated: 2024/07/16 23:35:12 by aboyreau          +#-.-*  +         *     #
#                                                      *-.. *   ++       #     #
# **************************************************************************** #

RED='\033[0;31m'
GREEN='\033[0;32m'
WHITE='\033[0m'

if [ "$3" == 124 ]
then
	printf "$RED"'TIMEOUT - '
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
