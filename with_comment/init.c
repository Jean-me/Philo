#include "philo.h"

int	valid_or_not(int c, char **arg)
{
	int	i;
	int	num;

	i = 1; // 0番目はプログラム名なので1からスタート
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
		table->number_of_meals = ft_atoi(arg[5]); // オプションの食事回数
	table->someone_died = 0;                      // 誰かが死んだかどうかを管理するフラグを初期化
	table->all_nourished = 0;
	// 全員が指定回数食事を終えたかどうかを管理するフラグを初期化
}

void	philo_init(t_philo *phi, char **arg, t_table *table)
{
	int	i;

	i = 0;
	while (i < ft_atoi(arg[1])) // Philoの数だけID,table,lastmealを初期化
	{
		{
			phi[i].id = i + 1; // IDは1から始まる
			phi[i].dead_or_alive = 1;
			phi[i].table = table; // 各フィロにテーブルの情報を渡す
			//生きている
			phi[i].left_fork = &table->forks[i];
			// 左のフォーク
			phi[i].right_fork = &table->forks[(i + 1) % ft_atoi(arg[1])];
			// 右のフォーク
			phi[i].last_meal = table->start_time;        // 最後に食べた時間を0に初期化
			phi[i].times_nourished = 0;                  // 食事の回数を初期化
			pthread_mutex_init(&phi[i].meal_lock, NULL); // lastmeal時間更新の
			i++;
		}
	}
}

void	mutex_init(t_table *table)
{
	int j;

	j = 0;
	while (j < table->n_philos) // philoの数だけフォーク初期
	{
		pthread_mutex_init(&table->forks[j], NULL);
		j++;
	}
	pthread_mutex_init(&table->moments_nourished_lock, NULL);
	// 食事の回数を管理するミューテックスを初期化
	pthread_mutex_init(&table->died, NULL);
	// 誰かが死んだかどうかを管理するミューテックスを初期化
}