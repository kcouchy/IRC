#!/bin/bash

# **************************************************************************** #
#                                                                              #
#                                                               ++             #
#    run_test.sh                                               +**+   +*  *    #
#                                                              ##%#*###*+++    #
#    By: aboyreau <bnzlvosnb@mozmail.com>                     +**+ -- ##+      #
#                                                             # *   *. #*      #
#    Created: 2024/07/15 23:52:33 by aboyreau          **+*+  * -_._-   #+     #
#    Updated: 2024/07/17 11:43:34 by aboyreau          +#-.-*  +         *     #
#                                                      *-.. *   ++       #     #
# **************************************************************************** #

RED='\033[0;31m'
WHITE='\033[0m'

server_crash()
{
	printf $RED
	echo -n "CRASH"
	printf $WHITE
	echo 
	echo "Server log :"
	echo 
	cat srv_log | sed 's/.*/	\0/g' 
	rm srv_log -f
	rm tmp -f
	exit 1
}

print_logs()
{
	echo
	echo
	echo "Server logs :"
	echo
	cat srv_log | sed 's/.*/	\0/g'
	echo
	echo "Client logs :"
	echo
	cat tmp | sed 's/.*/	\0/g'
	echo
}

if [ $# != 4 ]
then
	echo "./run_test <name> <password> <command> <expected result>"
	echo "This command runs a basic test :"
	echo "First, it says what's going on (first parameter)"
	echo "An IRC server is started, with the password passed as parameter"
	echo "A fake client connects"
	echo "The client runs a command (passed as a parameter) and record the server answers"
	echo "The answer is then matched against an expected result (last parameter)"
	echo "The result is then displayed through another script"
	exit 1
fi

if [ -n "$LEAKS" ]
then
	PREFIX="valgrind --log-file=leaks.log "
fi

NAME=$1
PASSWORD=$2
COMMAND=$3
EXPECTED_RESULT=$4

echo
echo -n "$NAME"

$PREFIX ./ircserv 6667 "$PASSWORD" > srv_log 2>&1 &
SRV_PID=$!

if [ -n "$LEAKS" ]
then
	sleep 1
fi

timeout 5 <<< $COMMAND nc localhost 6667 > tmp 2>&1

EXIT_CODE=$?

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
	print_logs
fi

tests/utils/print_res.sh "$(cat tmp)" "$EXPECTED_RESULT" $EXIT_CODE $LEAKS

rm srv_log -f
rm tmp -f
rm leaks.log -f
