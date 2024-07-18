/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Utils.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kcouchma <kcouchma@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/17 14:19:12 by kcouchma          #+#    #+#             */
/*   Updated: 2024/07/18 14:51:55 by kcouchma         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# pragma once

#include <string>

template <typename L>
bool find_erase(L &list, std::string search)
{
	typename L::iterator iter;
	iter = std::find(list.begin(), list.end(), search);
	if (iter == list.end())
		return (false);
	list.erase(iter);
	return (true);
}

template <typename L>
bool contains(L &list, std::string search)
{
	typename L::iterator iter;
	iter = std::find(list.begin(), list.end(), search);
	if (iter == list.end())
		return (false);
	return (true);
}

template <typename L> //MUST RUN CONTAINS FIRST
typename L::iterator find_return(L &list, std::string search)
{
	return (std::find(list.begin(), list.end(), search));
}
