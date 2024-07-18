# **************************************************************************** #
#                                                                              #
#                                                               ++             #
#    run_test_multiuser.sh                                     +**+   +*  *    #
#                                                              ##%#*###*+++    #
#    By: aboyreau <bnzlvosnb@mozmail.com>                     +**+ -- ##+      #
#                                                             # *   *. #*      #
#    Created: 2024/07/16 23:18:58 by aboyreau          **+*+  * -_._-   #+     #
#    Updated: 2024/07/18 17:36:06 by aboyreau          +#-.-*  +         *     #
#                                                      *-.. *   ++       #     #
# **************************************************************************** #

#!/bin/bash

RED='\033[0;31m'
WHITE='\033[0m'

cleanup()
{
	rm tmp_1 -f
	rm tmp_2 -f
	rm srv_log -f
	rm leaks.log -f
}

server_crash()
{
	printf $RED
	echo -n "CRASH"
	printf $WHITE
	echo 
	echo "Server log :"
	echo 
	cat srv_log | sed 's/.*/	\0/g' 
	cleanup
	exit 1
}

if [ $# != 6 ]
then
	echo "./run_test <name> <password> <command_user_1> <expected_result_1> <command_user_2> <expected_result_2>"
	echo "This command runs a basic test involving 2 users:"
	echo "First, it says what's going on (first parameter)"
	echo "An IRC server is started, with the password passed as parameter"
	echo "A fake client connects"
	echo "The client runs a command (passed as a parameter) and record the server answers"
	echo "A 2nd fake client connects"
	echo "The 2nd client runs a command (passed as a parameter) and record the server answers"
	echo "The answer of each clients are then matched against expected results"
	echo "The result is then displayed through another script"
	exit 1
fi

if [ -n "$LEAKS" ]
then
	PREFIX="valgrind --log-file=leaks.log "
fi

NAME=$1
PASSWORD=$2
COMMAND_1=$3
EXPECTED_RESULT_1=$4
COMMAND_2=$5
EXPECTED_RESULT_2=$6

echo -n "$NAME"

$PREFIX ./ircserv 6667 "$PASSWORD" > srv_log 2>&1 &
SRV_PID=$!

if [ -n "$LEAKS" ]
then
	sleep 1
fi

<<< "$COMMAND_1" nc localhost 6667 > tmp_1 2>&1 &
USR1_PID=$!

timeout 5 <<< "$COMMAND_2" nc localhost 6667 > tmp_2 2>&1

EXIT_CODE_2=$?

EXIT_CODE_1=$(wait $USR1_PID)

kill -s INT $SRV_PID 2> /dev/null || server_crash || exit 1

if [ -n "$LEAKS" ]
then
	sleep 1
	LEAKS=$(cat leaks.log | grep 'LEAK SUMMARY' | wc -l)
else
	LEAKS=0
fi

if [ -n "$VERBOSE" ]
then
	echo
	echo "log"
	cat srv_log
	echo
	echo "client 1"
	cat tmp_1
	echo
	echo "client 2"
	cat tmp_2
	echo
fi

echo -n " client 1 : "
tests/utils/print_res.sh "$(cat tmp_1)" "$EXPECTED_RESULT_1" $EXIT_CODE_1 $LEAKS
echo -n ", client 2 : "
tests/utils/print_res.sh "$(cat tmp_2)" "$EXPECTED_RESULT_2" $EXIT_CODE_2 $LEAKS

cleanup
