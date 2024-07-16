# **************************************************************************** #
#                                                                              #
#                                                               ++             #
#    run_test_multiuser.sh                                     +**+   +*  *    #
#                                                              ##%#*###*+++    #
#    By: aboyreau <bnzlvosnb@mozmail.com>                     +**+ -- ##+      #
#                                                             # *   *. #*      #
#    Created: 2024/07/16 23:18:58 by aboyreau          **+*+  * -_._-   #+     #
#    Updated: 2024/07/16 23:29:14 by aboyreau          +#-.-*  +         *     #
#                                                      *-.. *   ++       #     #
# **************************************************************************** #

#!/bin/bash

if [[ $# != 6 ]]
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

NAME=$1
PASSWORD=$2
COMMAND_1=$3
EXPECTED_RESULT_1=$4
COMMAND_2=$5
EXPECTED_RESULT_2=$6

echo -n "$NAME"

./ircserv 6667 "$PASSWORD" >/dev/null 2>/dev/null &
SRV_PID=$!

timeout 5 <<< $COMMAND_1 nc localhost 6667 > tmp_1 &
USR1_PID=$!
timeout 5 <<< $COMMAND_2 nc localhost 6667 > tmp_2
USR2_PID=$!

EXIT_CODE_1=$(wait $USR1_PID)
EXIT_CODE_2=$(wait $USR2_PID)

echo -n " client 1 : "
tests/utils/print_res.sh "$(cat tmp_1)" "$EXPECTED_RESULT_1" $EXIT_CODE_1
echo -n ", client 2 : "
tests/utils/print_res.sh "$(cat tmp_2)" "$EXPECTED_RESULT_2" $EXIT_CODE_2

kill $SRV_PID
