#!/bin/bash

# **************************************************************************** #
#                                                                              #
#                                                               ++             #
#    test.sh                                            :+:      :+:    :+:    #
#                                                              ##%#*###*+++    #
#    By: aboyreau <bnzlvosnb@mozmail.com>                     +**+ -- ##+      #
#                                                             # *   *. #*      #
#    Created: 2024/07/16 22:20:26 by aboyreau          **+*+  * -_._-   #+     #
#    Updated: 2024/07/17 11:38:05 by aboyreau         ###   ########.fr        #
#                                                      *-.. *   ++       #     #
# **************************************************************************** #

echo "##############################"
echo "#          IRC tester        #"
echo "##############################"
echo

echo "./test.sh --leaks will also check for the leaks"
echo "Each command has its own test file."
echo "Commands test files are in the subdirectory command."
echo
echo "To run a single command test file from your IRC project :"
printf "\033[0;31m"
echo '	TESTDIR=$PWD/tests ./tests/commands/<command>.sh'
printf "\033[0m\n"
echo "Of course, you should replace <command> with a real command."
echo

if [ "$1" == "--leaks" ]
then
	export LEAKS="1"
else
	unset LEAKS
fi

if [ -z "$TESTDIR" ]
then
	export TESTDIR=$PWD/tests
fi

for testfile in $TESTDIR/commands/*.sh
do
	$testfile
	echo ""
done
