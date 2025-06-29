#include "philo.h"

bool	get_all_nourished(t_table *table)
{
	bool	result;

	pthread_mutex_lock(&table->moments_nourished_lock);
	result = table->all_nourished;
	pthread_mutex_unlock(&table->moments_nourished_lock);
	return (result);
}

void	zzz(size_t duration, t_table *table)
{
	size_t	start;

	start = get_time();
	while (!table->someone_died && get_time() - start < duration)
	{
		usleep(100);
	}
}

int	get_someone_died(t_table *table)
{
	int	result;

	pthread_mutex_lock(&table->died);
	result = table->someone_died;
	pthread_mutex_unlock(&table->died);
	return (result);
}

void	*routine(void *arg)
{
	t_philo	*philo;
	int		id;
	int		ret;

	philo = (t_philo *)arg;
	id = philo->id;
	while (!get_someone_died(philo->table) && !get_all_nourished(philo->table))
	{
		if (philo->id % 2 == 0)
			ret = picking_up_forks(id, philo->table, philo->left_fork,
					philo->right_fork);
		else
			ret = picking_up_forks(id, philo->table, philo->right_fork,
					philo->left_fork);
		if (ret)
			return (NULL);
		eating(philo, id);
		putting_down_forks(philo->table, philo->right_fork, philo->left_fork);
		sleeping(philo, id);
		thinking(philo, id);
	}
	return (NULL);
}
