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
		usleep(100); // 100マイクロ秒待機
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

static void	eating(t_philo *philo, int id)
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

static void	putting_down_forks(t_table *table, pthread_mutex_t *first_fork,
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

static void	sleeping(t_philo *philo, int id)
{
	size_t	now;

	now = get_time() - philo->table->start_time; // 現在の時間を取得
	if (get_someone_died(philo->table) || get_all_nourished(philo->table))
		return ;
	printf("%zu %d is sleeping\n", now, id);
	zzz(philo->table->time_to_sleep, philo->table); // Sleeping time
}

static void	thinking(t_philo *philo, int id)
{
	size_t	now;

	if (get_someone_died(philo->table) || get_all_nourished(philo->table))
		return ;
	now = get_time() - philo->table->start_time; // 現在の時間を取得
	printf("%zu %d is thinking\n", now, id);
}

static int	picking_up_forks(int id, t_table *table,
		pthread_mutex_t *first_fork, pthread_mutex_t *second_fork)
{
	size_t	now;

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
void	*routine(void *arg)
{
	t_philo	*philo;
	int		id;
	int		ret;

	philo = (t_philo *)arg;
	id = philo->id;
	while (!get_someone_died(philo->table) && !get_all_nourished(philo->table))
	// フィロが生きている間ループ
	{
		if (philo->id % 2 == 0) // 偶数のフィロは左のフォークを先に取る
		{
			ret = picking_up_forks(id, philo->table, philo->left_fork,
									philo->right_fork); // first_fork,
		}
		else // 奇数のフィロは右のフォークを先に取る
		{
			ret = picking_up_forks(id, philo->table, philo->right_fork,
					philo->left_fork);
		}
		if (ret)
			return (NULL); // フォークを取るのに失敗した場合は終了
		eating(philo, id);
		putting_down_forks(philo->table, philo->right_fork, philo->left_fork);
		sleeping(philo, id);
		thinking(philo, id);
	}
	return (NULL); // スレッド終了
}
