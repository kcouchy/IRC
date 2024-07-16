# **************************************************************************** #
#                                                                              #
#                                                               ++             #
#    nick.sh                                                   +**+   +*  *    #
#                                                              ##%#*###*+++    #
#    By: aboyreau <bnzlvosnb@mozmail.com>                     +**+ -- ##+      #
#                                                             # *   *. #*      #
#    Created: 2024/07/15 23:08:17 by aboyreau          **+*+  * -_._-   #+     #
#    Updated: 2024/07/16 22:31:11 by aboyreau          +#-.-*  +         *     #
#                                                      *-.. *   ++       #     #
# **************************************************************************** #

RED='\033[0;31m'
GREEN='\033[0;32m'
WHITE='\033[0m'

empty()
{
	TEST="Empty nickname : "
	COMMAND=`<<- EOF cat
		NICK
		QUIT
	EOF`
	EXPECTED="431"
	$TESTDIR/utils/run_test.sh "$TEST" "" "$COMMAND" "$EXPECTED"
}

invalid()
{
	TEST="Invalid nickname : "
	EXPECTED="432"

	for nick in "#channel_like_name" "~" ":prefix" "1test" ""
	do
		COMMAND=`<<- EOF cat
			NICK $nick
			QUIT
		EOF`
		$TESTDIR/utils/run_test.sh "$TEST" "" "$COMMAND" "$EXPECTED"
	done
}

duplicated() # This guy is a bit too complicated to use run_test.sh
{
	echo -n "Duplicated nickname : "

	./ircserv 6667 "" >/dev/null 2>/dev/null &
	SRV_PID=$!

	<<- EOF nc localhost 6667 &
		NICK dup
	EOF

	CLIENT_PID=$!

	timeout 5 <<- EOF nc localhost 6667 > tmp
		NICK dup
		QUIT
	EOF

	EXIT_CODE=$?
	$TESTDIR/utils/print_res.sh "$RES" "433" "$EXIT_CODE"

	kill $CLIENT_PID
	kill $SRV_PID
}

echo "#######################"
echo "# NICK command tester #"
echo "#######################"
echo ""

empty
invalid
duplicated

rm tmp
