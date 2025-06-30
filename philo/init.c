/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mesasaki <mesasaki@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/30 15:38:14 by mesasaki          #+#    #+#             */
/*   Updated: 2025/06/30 15:58:24 by mesasaki         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

int	valid_or_not(int c, char **arg)
{
	int	i;
	int	num;

	i = 1;
	while (i < c)
	{
		if (is_number(arg[i]) || over_int(arg[i]))
			return (NOT_NUMBER);
		num = ft_atoi(arg[i]);
		if (i == 1 && (num <= 0 || num > 200))
			return (NOT_NUMBER);
		if (i != 1 && num <= 0)
			return (NOT_NUMBER);
		i++;
	}
	return (NUMBER);
}

void	arg_init(char **arg, t_table *table)
{
	table->n_philos = ft_atoi(arg[1]);
	table->time_to_die = ft_atoi(arg[2]);
	table->time_to_eat = ft_atoi(arg[3]);
	table->time_to_sleep = ft_atoi(arg[4]);
	if (arg[5])
		table->number_of_meals = ft_atoi(arg[5]);
	table->someone_died = 0;
	table->all_nourished = 0;
}

void	philo_init(t_philo *phi, char **arg, t_table *table)
{
	int	i;

	i = 0;
	while (i < ft_atoi(arg[1]))
	{
		{
			phi[i].id = i + 1;
			phi[i].dead_or_alive = 1;
			phi[i].table = table;
			phi[i].left_fork = &table->forks[i];
			phi[i].right_fork = &table->forks[(i + 1) % ft_atoi(arg[1])];
			phi[i].last_meal = table->start_time;
			phi[i].times_nourished = 0;
			pthread_mutex_init(&phi[i].meal_lock, NULL);
			i++;
		}
	}
}

void	mutex_init(t_table *table)
{
	int	j;

	j = 0;
	while (j < table->n_philos)
	{
		pthread_mutex_init(&table->forks[j], NULL);
		j++;
	}
	pthread_mutex_init(&table->moments_nourished_lock, NULL);
	pthread_mutex_init(&table->died, NULL);
}
