/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mesasaki <mesasaki@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/30 15:38:32 by mesasaki          #+#    #+#             */
/*   Updated: 2025/07/02 01:47:24 by mesasaki         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

static void	mutex_destroy(t_table *table, t_philo *philos)
{
	int	i;

	i = 0;
	while (i < table->n_philos)
	{
		pthread_mutex_destroy(&table->forks[i]);
		pthread_mutex_destroy(&philos[i].meal_lock);
		i++;
	}
	pthread_mutex_destroy(&table->died);
	pthread_mutex_destroy(&table->moments_nourished_lock);
	pthread_mutex_destroy(&table->print_mutex);
}

static int	make_threads(t_philo *philos, t_table *table)
{
	int	i;

	table->start_time = get_time();
	i = 0;
	while (i < table->n_philos)
	{
		philos[i].table = table;
		if (pthread_create(&philos[i].thread, NULL, routine, &philos[i]) != 0)
			return (1);
		i++;
	}
	usleep(100);
	if (pthread_create(&table->monitor_thread, NULL, monitor, philos) != 0)
		return (3);
	i = 0;
	while (i < table->n_philos)
	{
		if (pthread_join(philos[i].thread, NULL) != 0)
			return (2);
		i++;
	}
	pthread_join(table->monitor_thread, NULL);
	mutex_destroy(table, philos);
	return (0);
}

int	main(int argc, char **argv)
{
	t_table	table;
	t_philo	philos[PHILO_MAX];
	int		res;

	if (argc < 5 || argc > 6)
	{
		write(2, "Error: Wrong number of arguments.\n", 34);
		return (1);
	}
	if (valid_or_not(argc, argv))
	{
		write(2, "Error: All arguments must be positive integers.\n", 49);
		write(2, "number_of_philosophers must be between 1 and 200.\n", 51);
		return (1);
	}
	arg_init(argv, &table);
	table.start_time = get_time();
	philo_init(philos, argv, &table);
	mutex_init(&table);
	res = make_threads(philos, &table);
	return (res);
}
