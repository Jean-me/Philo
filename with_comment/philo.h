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
	pthread_mutex_t meal_lock; // lastmeal時間更新のロック
	int times_nourished;       // 食事の回数（オプション）
}		t_philo;

void	*routine(void *arg);
void	*monitor(void *arg);

int		valid_or_not(int c, char **arg);
void	arg_init(char **arg, t_table *table);
void	philo_init(t_philo *phi, char **arg, t_table *table);
void	mutex_init(t_table *table);

size_t	get_time(void);
int		ft_atoi(const char *string);
int		ft_isdigit(int c);
int		is_number(char *str);
int		over_int(char *str);

void	eating(t_philo *philo, int id);
void	putting_down_forks(t_table *table, pthread_mutex_t *first_fork,
			pthread_mutex_t *second_fork);
void	sleeping(t_philo *philo, int id);
void	thinking(t_philo *philo, int id);
int		picking_up_forks(int id, t_table *table, pthread_mutex_t *first_fork,
			pthread_mutex_t *second_fork);

bool	get_all_nourished(t_table *table);
void	zzz(size_t duration, t_table *table);
int		get_someone_died(t_table *table);
