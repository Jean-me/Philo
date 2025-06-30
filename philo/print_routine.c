	
#include "philo.h"
void print_routine(t_philo *philo, int id, const char *action)
{    
    printf("%s %zu %d\n",action, now, id);
    printf("%zu %d is eating\n", now, id);
}