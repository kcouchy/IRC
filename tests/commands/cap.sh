# **************************************************************************** #
#                                                                              #
#                                                               ++             #
#    cap.sh                                                    +**+   +*  *    #
#                                                              ##%#*###*+++    #
#    By: aboyreau <bnzlvosnb@mozmail.com>                     +**+ -- ##+      #
#                                                             # *   *. #*      #
#    Created: 2024/07/16 22:32:28 by aboyreau          **+*+  * -_._-   #+     #
#    Updated: 2024/07/16 22:38:34 by aboyreau          +#-.-*  +         *     #
#                                                      *-.. *   ++       #     #
# **************************************************************************** #

ls302()
{
	TEST="CAP LS 302 : "
	COMMAND=`<<- EOF cat
		CAP LS 302
		QUIT
	EOF`
	EXPECTED="CAP * LS :"
	$TESTDIR/utils/run_test.sh "$TEST" "" "$COMMAND" "$EXPECTED"
}

end()
{
	TEST="CAP END : "
	COMMAND=`<<- EOF cat
		CAP END
		QUIT
	EOF`
	EXPECTED=""
	$TESTDIR/utils/run_test.sh "$TEST" "" "$COMMAND" "$EXPECTED"
}

echo "######################"
echo "# CAP command tester #"
echo "######################"
echo ""

ls302
end

rm tmp
