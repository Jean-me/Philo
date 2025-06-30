/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mesasaki <mesasaki@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/30 15:37:38 by mesasaki          #+#    #+#             */
/*   Updated: 2025/06/30 15:37:41 by mesasaki         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

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

typedef struct s_table
{
	int				n_philos;
	int				time_to_die;
	int				time_to_eat;
	int				time_to_sleep;
	int				number_of_meals;
	pthread_t		monitor_thread;
	pthread_mutex_t	forks[PHILO_MAX];
	pthread_mutex_t	died;
	int				someone_died;
	int				all_nourished;
	size_t			start_time;
	pthread_mutex_t	moments_nourished_lock;

}					t_table;

typedef struct s_philo
{
	t_table			*table;
	int				id;
	size_t			last_meal;
	int				dead_or_alive;
	pthread_mutex_t	*left_fork;
	pthread_mutex_t	*right_fork;
	pthread_t		thread;
	pthread_mutex_t	meal_lock;
	int				times_nourished;
}					t_philo;

void				*routine(void *arg);
void				*monitor(void *arg);

int					valid_or_not(int c, char **arg);
void				arg_init(char **arg, t_table *table);
void				philo_init(t_philo *phi, char **arg, t_table *table);
void				mutex_init(t_table *table);

size_t				get_time(void);
int					ft_atoi(const char *string);
int					ft_isdigit(int c);
int					is_number(char *str);
int					over_int(char *str);

void				eating(t_philo *philo, int id);
void				putting_down_forks(t_table *table,
						pthread_mutex_t *first_fork,
						pthread_mutex_t *second_fork);
void				sleeping(t_philo *philo, int id);
void				thinking(t_philo *philo, int id);
int					picking_up_forks(int id, t_table *table,
						pthread_mutex_t *first_fork,
						pthread_mutex_t *second_fork);

bool				get_all_nourished(t_table *table);
void				zzz(size_t duration, t_table *table);
int					get_someone_died(t_table *table);
