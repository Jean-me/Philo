#include "philo.h"





void	set_someone_died(t_table *table)
{
	pthread_mutex_lock(&table->died);
	table->someone_died = 1; // 誰かが死んだと設定
	pthread_mutex_unlock(&table->died);
}



int	ft_isdigit(int c)
{
	return (c >= '0' && c <= '9');
}

int	is_number(char *str)
{
	if (*str == '-' || *str == '+')
		str++;
	if (!*str)
		return (NOT_NUMBER);
	while (*str)
	{
		if (!ft_isdigit(*str))
			return (NOT_NUMBER);
		str++;
	}
	return (NUMBER);
}
int	over_int(char *str)
{
	int				minus;
	unsigned long	result;

	result = 0;
	minus = 0;
	if (*str == '+' || *str == '-')
	{
		if (*str == '-')
			minus = -1;
		str++;
	}
	while (*str >= '0' && *str <= '9')
	{
		result = 10 * result + (*str - '0');
		if ((unsigned long)INT_MAX < result && minus == 0)
			return (NOT_NUMBER);
		if ((unsigned long)INT_MAX + 1 < result && minus == -1)
			return (NOT_NUMBER);
		str++;
	}
	return (NUMBER);
}
int	space_or_not(char c)
{
	if (c == ' ' || c == '\f' || c == '\n' || c == '\r' || c == '\t'
		|| c == '\v')
		return (1);
	else
		return (0);
}

int	ft_atoi(const char *string)
{
	int				minus;
	unsigned long	result;

	minus = 1;
	result = 0;
	while (space_or_not(*string) == 1)
		string++;
	if (*string == '+' || *string == '-')
	{
		if (*string == '-')
			minus = -1;
		string++;
	}
	while (*string >= '0' && *string <= '9')
	{
		if (result * 10 + (*string - '0') > ((unsigned long)LONG_MAX)
			&& minus == 1)
			return ((int)LONG_MAX);
		if (((unsigned long)LONG_MIN - (*string - '0')) / 10 < result
			&& minus == -1)
			return ((int)LONG_MIN);
		result = 10 * result + (*string - '0');
		string++;
	}
	return (minus * (int)result);
}
int	valid_or_not(int c, char **arg) //ここでまとめる
{
	int i;
	int num;

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
	int	j;

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





void	*monitor(void *arg) // 監視スレッド
{
	t_philo *philos;
	t_table *table;
	size_t current_time;
	int i;
	size_t now;

	i = 0;
	philos = (t_philo *)arg; // 引数からフィロの配列を取得
	table = philos[0].table; // テーブル情報を取得

	i = 0;
	while (1)
	{
		i = 0;
		if (all_philos_nourished(philos, table)) // 全員が指定回数食事を終えたか確認
		{
			pthread_mutex_lock(&table->moments_nourished_lock);
			table->all_nourished = 1; // 全員が指定回数食事を終えたと設定
			pthread_mutex_unlock(&table->moments_nourished_lock);
			printf("All philosophers have nourished\n");
			return (NULL); // 監視スレッドは終了
		}
		while (i < table->n_philos)
		{
			current_time = get_time();
			if (current_time - philos[i].last_meal > (size_t)table->time_to_die)
			{
				printf("%zu %zu\n", current_time, philos[i].last_meal);
				now = get_time() - table->start_time; // 現在の時間を取得
				printf("%zu %d died\n", now, philos[i].id);
				set_someone_died(table); // 誰かが死んだと設定
				return (NULL);           // 監視スレッドは終了
			}
			i++;
		}
	}
}

int	make_threads(t_philo *philos, t_table *table)
{
	int	i;

	table->start_time = get_time(); // 最後に食べた時間を現在の時間に設定
	i = 0;
	while (i < table->n_philos)
	{
		philos[i].table = table; // 各フィロにテーブルの情報を渡す
		if (pthread_create(&philos[i].thread, NULL, routine, &philos[i]) != 0)
			return (1); // スレッドの作成に失敗した場合
		i++;
	}
	usleep(100); // スレッドが開始するまで少し待つ
	if (pthread_create(&table->monitor_thread, NULL, monitor, philos) != 0)
		//監視者
		return (3);
	i = 0;
	while (i < table->n_philos)
	{
		if (pthread_join(philos[i].thread, NULL) != 0) // 各スレッドの終了を待つ
			return (2);
		i++;
	}
	pthread_join(table->monitor_thread, NULL); // 監視スレッドの終了を待つ
	i = 0;
	while (i < table->n_philos) // スレッドを終了した後、フォークのミューテックスを破棄
	{
		pthread_mutex_destroy(&table->forks[i]);
		i++;
	}
	pthread_mutex_destroy(&table->died); // 誰かが死んだかどうかを管理するミューテックスを破棄
	return (0);                          // 成功
}

int	main(int argc, char **argv) // numPhilo, die, eat, sleep,
{
	t_table table;
	t_philo philos[PHILO_MAX];
	int res;

	if (argc < 5 || argc > 6) // 引数の数が5か6以外の場合はエラー
		return (1);
	if (valid_or_not(argc, argv))
		return (1);
	//入力が数字のみか、INTMAX超えてないか確認の関数➺リターンがNUMBERなら、atoiで数値に➺200？
	arg_init(argv, &table);
	table.start_time = get_time();    // 開始時間を現在の時間に設定
	philo_init(philos, argv, &table); // Philoの数だけID,table,lastmealを初期化する関数
	// ストラクチャに代入する関数↑
	mutex_init(&table);
	// mutexを初期化する関数
	res = make_threads(philos, &table); // threadsを作成する関数 intreturnどうすんの
	return (res);                       // 成功なら0,失敗なら1,2,3を返す
}
