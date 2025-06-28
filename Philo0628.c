#include <limits.h>
#include <pthread.h>
#include <stdbool.h>
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
	int number_of_meals; // 食事の回数（オプション）
	pthread_mutex_t forks[PHILO_MAX];
	pthread_t monitor_thread; //監視用スレッド
	pthread_mutex_t died;     // 誰かが死んだかどうかを管理するミューテックス
	int someone_died;
	int all_nourished; // 全員が指定回数食事を終えたかどうかを管理するフラグ
	size_t start_time; // 開始時間
	pthread_mutex_t moments_nourished_lock;
	// 食事の回数を管理するミューテックス

}			t_table;

typedef struct s_philo //固有？
{
	t_table *table; //全員が同じルールをみて動ける
	int id;
	size_t last_meal;            //最後に食べた時間
	int dead_or_alive;           //生きているかどうか
	pthread_mutex_t *left_fork;  // 左のフォーク（mutex）
	pthread_mutex_t *right_fork; // 右のフォーク（mutex）
	pthread_t thread;
	pthread_mutex_t meal_lock; // lastmeal時間更新のロック
	int times_nourished;       // 食事の回数（オプション）
}			t_philo;

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
		usleep(100); // 100マイクロ秒待機
	}
}

void	set_someone_died(t_table *table)
{
	pthread_mutex_lock(&table->died);
	table->someone_died = 1; // 誰かが死んだと設定
	pthread_mutex_unlock(&table->died);
}

// void set_all_nourished(t_table *table)
// {
//  pthread_mutex_lock(&table->moments_nourished_lock);
//  table->all_nourished = 1; // 全員が指定回数食事を終えたと設定
//  pthread_mutex_unlock(&table->moments_nourished_lock);
// }

int	get_someone_died(t_table *table)
{
	int	result;

	pthread_mutex_lock(&table->died);
	result = table->someone_died;
	pthread_mutex_unlock(&table->died);
	return (result);
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
int	get_all_nourished(t_table *table)
{
	int	result;

	pthread_mutex_lock(&table->moments_nourished_lock);
	result = table->all_nourished;
	pthread_mutex_unlock(&table->moments_nourished_lock);
	return (result);
}

static int	all_philos_nourished(t_philo *philos, t_table *table)
{
	int	i;

	i = 1;                           // フィロの配列のインデックスは0から始まるので1からスタート
	if (table->number_of_meals == 0) // 食事の回数が指定されていない場合は常にfalse
		return (0);
	while (i <= table->n_philos) // 全てのフィロが指定回数食事を終えたか確認
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

static void	putting_down_forks(int id, t_table *table,
		pthread_mutex_t *first_fork, pthread_mutex_t *second_fork)
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

void	*routine(void *arg) // argはphiloそれぞれのアドレス
{
	t_philo *philo;
	int id;
	int ret;

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
		putting_down_forks(id, philo->table, philo->right_fork,
			philo->left_fork);

		sleeping(philo, id);
		thinking(philo, id);
	}
	return (NULL); // スレッド終了
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
		while (i < table->n_philos)
		{
			current_time = get_time();
			if (current_time - philos[i].last_meal > table->time_to_die)
			{
				printf("%zu %zu\n", current_time, philos[i].last_meal);
				now = get_time() - table->start_time; // 現在の時間を取得
				printf("%zu %d died\n", now, philos[i].id);
				set_someone_died(table); // 誰かが死んだと設定
				return (NULL);           // 監視スレッドは終了
			}
			i++;
		}
		if (all_philos_nourished(philos, table)) // 全員が指定回数食事を終えたか確認
		{
			pthread_mutex_lock(&table->moments_nourished_lock);
			table->all_nourished = 1; // 全員が指定回数食事を終えたと設定
			pthread_mutex_unlock(&table->moments_nourished_lock);
			printf("All philosophers have nourished\n");
			return (NULL); // 監視スレッドは終了
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
