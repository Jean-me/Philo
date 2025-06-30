/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_atoi.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mesasaki <mesasaki@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/30 15:38:05 by mesasaki          #+#    #+#             */
/*   Updated: 2025/06/30 15:55:56 by mesasaki         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

static int	space_or_not(char c)
{
	if (c == ' ' || c == '\f' || c == '\n' || c == '\r' || c == '\t'
		|| c == '\v')
		return (1);
	else
		return (0);
}

int	ft_atoi(const char *string)
{
	int				minus;
	unsigned long	result;

	minus = 1;
	result = 0;
	while (space_or_not(*string) == 1)
		string++;
	if (*string == '+' || *string == '-')
	{
		if (*string == '-')
			minus = -1;
		string++;
	}
	while (*string >= '0' && *string <= '9')
	{
		if (result * 10 + (*string - '0') > ((unsigned long)LONG_MAX)
			&& minus == 1)
			return ((int)LONG_MAX);
		if (((unsigned long)LONG_MIN - (*string - '0')) / 10 < result
			&& minus == -1)
			return ((int)LONG_MIN);
		result = 10 * result + (*string - '0');
		string++;
	}
	return (minus * (int)result);
}
