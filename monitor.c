#include "philo.h"

void	set_someone_died(t_table *table)
{
	pthread_mutex_lock(&table->died);
	table->someone_died = 1; // 誰かが死んだと設定
	pthread_mutex_unlock(&table->died);
}

static int	all_philos_nourished(t_philo *philos, t_table *table)
{
	int	i;

	i = 0;                           // フィロの配列のインデックスは0から始まるので1からスタート
	if (table->number_of_meals == 0) // 食事の回数が指定されていない場合は常にfalse
		return (0);
	while (i < table->n_philos) // 全てのフィロが指定回数食事を終えたか確認
	{
		pthread_mutex_lock(&table->moments_nourished_lock);
		if (philos[i].times_nourished < table->number_of_meals)
		{
			pthread_mutex_unlock(&table->moments_nourished_lock);
			return (0); // まだ食事を終えていないフィロがいる場合はfalse
		}
		pthread_mutex_unlock(&table->moments_nourished_lock);
		i++;
	}
	return (1); // 全てのフィロが指定回数食事を終えた場合はtrue
}

static bool	set_all_nourished(t_table *table)
{
	pthread_mutex_lock(&table->moments_nourished_lock);
	table->all_nourished = 1; // 全員が指定回数食事を終えたと設定
	pthread_mutex_unlock(&table->moments_nourished_lock);
	return (1); // 成功
}

static bool	let_the_final_bell_toll(t_table *table, t_philo *philos,
		size_t current_time, int i)
{
	size_t	now;

	if (current_time - philos[i].last_meal > (size_t)table->time_to_die)
	{
		printf("%zu %zu\n", current_time, philos[i].last_meal);
		now = get_time() - table->start_time; // 現在の時間を取得
		printf("%zu %d died\n", now, philos[i].id);
		set_someone_died(table); // 誰かが死んだと設定
		return (false);          // 死亡した場合はfalseを返す
	}
	return (true); // 死亡していない場合はtrueを返す
}

void	*monitor(void *arg)
{
	t_philo *philos;
	t_table *table;
	size_t current_time;
	int i;

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
				return (NULL); // 誰かが死んだ場合は監視スレッドを終了
			i++;
		}
	}
}