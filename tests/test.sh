#!/bin/bash

# **************************************************************************** #
#                                                                              #
#                                                               ++             #
#    test.sh                                                   +**+   +*  *    #
#                                                              ##%#*###*+++    #
#    By: aboyreau <bnzlvosnb@mozmail.com>                     +**+ -- ##+      #
#                                                             # *   *. #*      #
#    Created: 2024/07/18 00:10:47 by aboyreau          **+*+  * -_._-   #+     #
#    Updated: 2024/07/18 00:10:47 by aboyreau          +#-.-*  +         *     #
#                                                      *-.. *   ++       #     #
# **************************************************************************** #

detect_params()
{
	for i in $@
	do
		if [ "$i" == "--leaks" ]
		then
			export LEAKS="1"
		fi
		
		if [ "$i" = "--verbose" ]
		then
			export VERBOSE="1"
		fi
	done
}

trap "exit 1" INT

echo "##############################"
echo "#          IRC tester        #"
echo "##############################"
echo

echo "./test.sh --leaks will also check for the leaks"
echo "./test.sh --verbose will display the server and clients logs for each test"
echo
echo "You can combine both flags, or use \`export VERBOSE=1; export LEAKS=1\`"
echo "to run invidual commands with verbose logs or leak checks."
echo
echo "To run a single command test file from your IRC project :"
printf "\033[0;31m"
echo '	TESTDIR=$PWD/tests ./tests/commands/<command>.sh'
printf "\033[0m\n"
echo "Of course, you should replace <command> with a real command name, in lowercase."
echo

detect_params $@

if [ -z "$TESTDIR" ]
then
	export TESTDIR=$PWD/tests
fi

for testfile in $TESTDIR/commands/*.sh
do
	$testfile
	echo ""
done
