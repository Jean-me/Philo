#include <limits.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <unistd.h>
#define NOT_NUMBER 1
#define NUMBER 0
#define PHILO_MAX 200

typedef struct s_table //共有？
{
	int n_philos;
	int time_to_die;
	int time_to_eat;
	int time_to_sleep;
	pthread_mutex_t forks[PHILO_MAX];
	pthread_t monitor_thread; //監視用スレッド
	int someone_died;
	size_t start_time; // 開始時間

}		t_table;

typedef struct s_philo //固有？
{
	t_table *table; //全員が同じルールをみて動ける
	int id;
	size_t last_meal;            //最後に食べた時間
	int dead_or_alive;           //生きているかどうか
	pthread_mutex_t *left_fork;  // 左のフォーク（mutex）
	pthread_mutex_t *right_fork; // 右のフォーク（mutex）
	pthread_t thread;
}		t_philo;

size_t	get_time(void)
{
	struct timeval	time;

	if (gettimeofday(&time, NULL) == -1)
		write(2, "gettimeofday error\n", 20);
	return (time.tv_sec * 1000 + time.tv_usec / 1000);
}
void	zzz(size_t duration, t_table *table)
{
	size_t	start;

	start = get_time();
	while (!table->someone_died && get_time() - start < duration)
	{
		usleep(1); // 100マイクロ秒待機
	}
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
	//入力が数字のみか、INTMAX超えてないか確認の関数➺リターンがNUMBERなら、atoiで数値に➺200？
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
}

void	philo_init(t_philo *phi, char **arg, t_table *table)
{
	int	i;

	i = 0;
	while (i < ft_atoi(arg[1])) // Philoの数だけID,table,lastmealを初期化
	{
		{
			phi[i].id = i;
			phi[i].dead_or_alive = 1;
			phi[i].table = table; // 各フィロにテーブルの情報を渡す
			//生きている
			phi[i].left_fork = &table->forks[i];
			// 左のフォーク
			phi[i].right_fork = &table->forks[(i + 1) % ft_atoi(arg[1])];
			// 右のフォーク
			phi[i].last_meal = 0; // 最後に食べた時間を0に初期化
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
}

void	*routine(void *arg) // argはphiloそれぞれのアドレス
{
	t_philo *philo;
	int id;

	philo = (t_philo *)arg;
	id = philo->id;

	while (!philo->table->someone_died) // フィロが生きている間ループ
	{
		// Picking up forks
		if (philo->id % 2 == 0) // 偶数のフィロは左のフォークを先に取る
		{
			pthread_mutex_lock(philo->left_fork);
			printf("%d has taken a fork.\n", id);
			pthread_mutex_lock(philo->right_fork);
			printf("%d has taken a fork.\n", id);
		}
		else // 奇数のフィロは右のフォークを先に取る
		{
			pthread_mutex_lock(philo->right_fork);
			printf("%d has taken a fork.\n", id);
			pthread_mutex_lock(philo->left_fork);
			printf("%d has taken a fork.\n", id);
		}
		// Eating
		philo->last_meal = get_time(); // 最後に食べた時間を更新
		printf("%d is eating.\n", id);
		zzz(philo->table->time_to_eat, philo->table); // Eating time
		// Putting down forks
		pthread_mutex_unlock(philo->right_fork);
		pthread_mutex_unlock(philo->left_fork);
		// Sleeping
		printf("%d is sleeping.\n", id);
		zzz(philo->table->time_to_sleep, philo->table); // Sleeping time
														// Thinking
		printf("%d is thinking.\n", id);
		zzz(philo->table->time_to_sleep, philo->table); // Thinking time
	}
	return (NULL); // スレッド終了
}

void	*monitor(void *arg) // 監視スレッド
{
	t_philo *philos;
	t_table *table;
	size_t current_time;
	int i;

	i = 0;
	philos = (t_philo *)arg; // 引数からフィロの配列を取得
	table = philos[0].table; // テーブル情報を取得

	i = 0;
	while (1)
	{
		i = 0;
		while (i < table->n_philos)
		{
			if (philos[i].last_meal == 0)
			{
				i++;
				continue ;
			}
			current_time = get_time();
			if (current_time - philos[i].last_meal > table->time_to_die)
			{
				printf("%d died.\n", philos[i].id);
				table->someone_died = 1; // 誰かが死んだ
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
	usleep(1000); // スレッドが開始するまで少し待つ
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
	return (0); // 成功
}

int	main(int argc, char **argv) // numPhilo, die, eat, sleep
{
	t_table table;
	t_philo philos[PHILO_MAX];

	if (argc != 5) // 引数の数が正しいか確認
		return (1);
	if (valid_or_not(argc, argv))
		return (1);
	//入力が数字のみか、INTMAX超えてないか確認の関数➺リターンがNUMBERなら、atoiで数値に➺200？
	arg_init(argv, &table);
	philo_init(philos, argv, &table); // Philoの数だけID,table,lastmealを初期化する関数
	// ストラクチャに代入する関数↑
	mutex_init(&table);
	// mutexを初期化する関数
	make_threads(philos, &table); // threadsを作成する関数 intreturnどうすんの
	return (0);
}
