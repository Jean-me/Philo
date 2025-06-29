#include "philo.h"

void	eating(t_philo *philo, int id)
{
	size_t	now;

	now = get_time() - philo->table->start_time; // 現在の時間を取得
	if (get_someone_died(philo->table) || get_all_nourished(philo->table))
		return ;                                  // 指定回数食事を終えた場合は終了
	pthread_mutex_lock(&philo->meal_lock);       // フィロが食べるかどうかのフラグをロック
	philo->last_meal = get_time();               // 最後に食べた時間を更新
	philo->times_nourished++;                    // 食事の回数を増やす
	pthread_mutex_unlock(&philo->meal_lock);     // フィロが食べるかどうかのフラグをアンロック
	now = get_time() - philo->table->start_time; // 現在の時間を取得
	printf("%zu %d is eating\n", now, id);
	zzz(philo->table->time_to_eat, philo->table);
}

void	putting_down_forks(t_table *table, pthread_mutex_t *first_fork,
		pthread_mutex_t *second_fork)
{
	if (table->someone_died || get_all_nourished(table))
	{
		pthread_mutex_unlock(first_fork);
		pthread_mutex_unlock(second_fork);
		return ;
	}
	pthread_mutex_unlock(first_fork);
	pthread_mutex_unlock(second_fork);
}

void	sleeping(t_philo *philo, int id)
{
	size_t	now;

	now = get_time() - philo->table->start_time; // 現在の時間を取得
	if (get_someone_died(philo->table) || get_all_nourished(philo->table))
		return ;
	printf("%zu %d is sleeping\n", now, id);
	zzz(philo->table->time_to_sleep, philo->table); // Sleeping time
}

void	thinking(t_philo *philo, int id)
{
	size_t	now;

	if (get_someone_died(philo->table) || get_all_nourished(philo->table))
		return ;
	now = get_time() - philo->table->start_time; // 現在の時間を取得
	printf("%zu %d is thinking\n", now, id);
}

int	picking_up_forks(int id, t_table *table, pthread_mutex_t *first_fork,
		pthread_mutex_t *second_fork)
{
	size_t now;

	if (get_someone_died(table) || get_all_nourished(table))
		return (1);
	pthread_mutex_lock(first_fork);
	if (get_someone_died(table))
	{
		pthread_mutex_unlock(first_fork);
		return (1);
	}
	now = get_time() - table->start_time; // 現在の時間を取得
	printf("%zu %d has taken a fork\n", now, id);
	if (table->n_philos == 1) // フィロが1人の場合、フォークを取るのに失敗
		return (1);
	pthread_mutex_lock(second_fork);
	if (get_someone_died(table))
	{
		pthread_mutex_unlock(first_fork);
		pthread_mutex_unlock(second_fork);
		return (1);
	}
	now = get_time() - table->start_time; // 現在の時間を取得
	printf("%zu %d has taken a fork\n", now, id);
	return (0); // フォークを取るのに成功した場合
}