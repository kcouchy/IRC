/* ************************************************************************** */
/*                                                                            */
/*                                                              ++            */
/*   utils.cpp                                                 +**+   +*  *   */
/*                                                             ##%#*###*+++   */
/*   By: aboyreau <bnzlvosnb@mozmail.com>                     +**+ -- ##+     */
/*                                                            # *   *. #*     */
/*   Created: 2024/07/17 12:06:06 by aboyreau          **+*+  * -_._-   #+    */
/*   Updated: 2024/07/17 12:07:46 by aboyreau          +#-.-*  +         *    */
/*                                                     *-.. *   ++       #    */
/* ************************************************************************** */

#include "utils.h"
#include <iostream>

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
		std::cerr << "Pushing back : " << str.substr(start, stop - start) << std::endl;
		splitted.push_back(str.substr(start, stop - start));
		start = stop + 1;
		std::cerr << "start : " << start << std::endl;
		std::cerr << "searching `" << delim << "` in " << str.substr(start, str.size() - 1) << std::endl;
		stop = str.find(delim, start);
		std::cerr << "stop : " << stop << std::endl;
	}
	return splitted;
}
