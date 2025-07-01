/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   monitor.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mesasaki <mesasaki@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/30 15:38:24 by mesasaki          #+#    #+#             */
/*   Updated: 2025/07/01 15:13:29 by mesasaki         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

void	set_someone_died(t_table *table)
{
	pthread_mutex_lock(&table->died);
	table->someone_died = 1;
	pthread_mutex_unlock(&table->died);
}

static int	all_philos_nourished(t_philo *philos, t_table *table)
{
	int	i;

	i = 0;
	if (table->number_of_meals == 0)
		return (0);
	while (i < table->n_philos)
	{
		pthread_mutex_lock(&philos[i].meal_lock);
		if (philos[i].times_nourished < table->number_of_meals)
		{
			pthread_mutex_unlock(&philos[i].meal_lock);
			return (0);
		}
		pthread_mutex_unlock(&philos[i].meal_lock);
		i++;
	}
	return (1);
}

static bool	set_all_nourished(t_table *table)
{
	pthread_mutex_lock(&table->moments_nourished_lock);
	table->all_nourished = 1;
	pthread_mutex_unlock(&table->moments_nourished_lock);
	return (1);
}

static bool	let_the_final_bell_toll(t_table *table, t_philo *philos,
		size_t current_time, int i)
{
	size_t	now;
	size_t	last_meal_time;

	pthread_mutex_lock(&philos[i].meal_lock);
	last_meal_time = philos[i].last_meal;
	pthread_mutex_unlock(&philos[i].meal_lock);
	if (current_time - last_meal_time > (size_t)table->time_to_die)
	{
		now = get_time() - table->start_time;
		print_routine(now, table, philos[i].id, "died");
		set_someone_died(table);
		return (false);
	}
	return (true);
}

void	*monitor(void *arg)
{
	t_philo	*philos;
	t_table	*table;
	size_t	current_time;
	int		i;

	philos = (t_philo *)arg;
	table = philos[0].table;
	while (1)
	{
		i = 0;
		if (all_philos_nourished(philos, table))
		{
			if (set_all_nourished(table))
				return (NULL);
		}
		while (i < table->n_philos)
		{
			current_time = get_time();
			if (!let_the_final_bell_toll(table, philos, current_time, i))
				return (NULL);
			i++;
		}
	}
}
