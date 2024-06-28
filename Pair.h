/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Pair.h                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kcouchma <kcouchma@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/25 12:01:42 by atu               #+#    #+#             */
/*   Updated: 2024/06/28 15:17:10 by kcouchma         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

template <typename T, typename D>

class Pair
{
	private:
		const T key;

	public:
		D value;
		Pair(T key, D value);
		T getKey();
};

template <typename T, typename D> Pair<T, D>::Pair(T key, D value) : key(key), value(value)
{
}

template <typename T, typename D> T Pair<T, D>::getKey()
{
	return (key);
}

