/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Utils.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lribette <lribette@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/17 14:19:12 by kcouchma          #+#    #+#             */
/*   Updated: 2024/08/01 15:21:23 by lribette         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# pragma once

#include <string>
#include <algorithm>
#include <vector>

std::vector<std::string> strsplit(std::string str, char delim);

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

/* 
 * MUST RUN CONTAINS FIRST
 */
template <typename L> 
typename L::iterator find_return(L &list, std::string search)
{
	return (std::find(list.begin(), list.end(), search));
}
