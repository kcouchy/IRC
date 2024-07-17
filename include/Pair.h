/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Pair.h                                                    +**+   +*  *   */
/*                                                    +:+ +:+         +:+     */
/*   By: kcouchma <kcouchma@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/25 12:01:42 by atu               #+#    #+#             */
/*   Updated: 2024/07/16 20:06:27 by aboyreau          +#-.-*  +         *    */
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
		bool operator=(const Pair& inst);
};

template <typename T, typename D>
bool Pair<T, D>::operator=(const Pair<T, D> &inst)
{
	return (key == inst.key && value == inst.value); // keys and values are equals
}

template <typename T, typename D>
Pair<T, D>::Pair(T key, D value) : key(key), value(value)
{
}

template <typename T, typename D>
T Pair<T, D>::getKey()
{
	return (key);
}

template <typename T, typename D>
bool operator==(Pair<T, D> pair, const T a)
{
	return pair.getKey() == a;
}

template <typename T, typename D>
bool operator==(Pair<T, D> pair, const D a)
{
	return pair.value == a;
}
