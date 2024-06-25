/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Pair.h                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: atu <bnzlvosnb@mozmail.com>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/25 12:01:42 by atu               #+#    #+#             */
/*   Updated: 2024/06/25 23:19:29 by atu              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

template <typename T, typename D>

class Pair
{
	private:
		const T key;

	public:
		D value;
		Pair(T key, D value);
};

template <typename T, typename D> Pair<T, D>::Pair(T key, D value) : key(key), value(value)
{
}
