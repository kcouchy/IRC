# **************************************************************************** #
#                                                                              #
#                                                               ++             #
#    run_test.sh                                               +**+   +*  *    #
#                                                              ##%#*###*+++    #
#    By: aboyreau <bnzlvosnb@mozmail.com>                     +**+ -- ##+      #
#                                                             # *   *. #*      #
#    Created: 2024/07/15 23:52:33 by aboyreau          **+*+  * -_._-   #+     #
#    Updated: 2024/07/16 22:19:12 by aboyreau          +#-.-*  +         *     #
#                                                      *-.. *   ++       #     #
# **************************************************************************** #

if [[ $# != 4 ]]
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

NAME=$1
PASSWORD=$2
COMMAND=$3
EXPECTED_RESULT=$4

echo -n "$NAME"

./ircserv 6667 "$PASSWORD" >/dev/null 2>/dev/null &
SRV_PID=$!

timeout 5 <<< $COMMAND nc localhost 6667 > tmp

EXIT_CODE=$?

tests/utils/print_res.sh "$(cat tmp)" "$EXPECTED_RESULT" $EXIT_CODE

kill $SRV_PID
