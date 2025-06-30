
#include "philo.h"

void	print_routine(size_t now, t_table *table, int id, const char *action)
{
	pthread_mutex_lock(&table->print_mutex);
	printf("%zu %d %s\n", now, id, action);
	pthread_mutex_unlock(&table->print_mutex);
}