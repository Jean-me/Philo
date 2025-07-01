/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   print_ro.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mesasaki <mesasaki@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/01 22:45:22 by mesasaki          #+#    #+#             */
/*   Updated: 2025/07/01 23:28:20 by mesasaki         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

void	print_routine(size_t now, t_table *table, int id, const char *action)
{
	pthread_mutex_lock(&table->print_mutex);
	if (!get_someone_died(table))
		printf("%zu %d %s\n", now, id, action);
	pthread_mutex_unlock(&table->print_mutex);
}
