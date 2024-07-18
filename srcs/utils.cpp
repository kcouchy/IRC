/* ************************************************************************** */
/*                                                                            */
/*                                                              ++            */
/*   utils.cpp                                                 +**+   +*  *   */
/*                                                             ##%#*###*+++   */
/*   By: aboyreau <bnzlvosnb@mozmail.com>                     +**+ -- ##+     */
/*                                                            # *   *. #*     */
/*   Created: 2024/07/17 12:06:06 by aboyreau          **+*+  * -_._-   #+    */
/*   Updated: 2024/07/17 14:51:52 by aboyreau          +#-.-*  +         *    */
/*                                                     *-.. *   ++       #    */
/* ************************************************************************** */

#include "utils.h"

std::vector<std::string> strsplit(std::string str, char delim)
{
	std::vector<std::string> splitted;
	size_t start = 0;
	size_t stop;

	stop = str.find(delim);
	while (1)
	{
		if (stop == std::string::npos)
		{
			splitted.push_back(str.substr(start, str.size()));
			break ;
		}
		splitted.push_back(str.substr(start, stop - start));
		start = stop + 1;
		stop = str.find(delim, start);
	}
	return splitted;
}
