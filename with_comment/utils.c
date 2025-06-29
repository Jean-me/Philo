#include "philo.h"

size_t	get_time(void)
{
	struct timeval	time;

	if (gettimeofday(&time, NULL) == -1)
		write(2, "gettimeofday error\n", 20);
	return (time.tv_sec * 1000 + time.tv_usec / 1000);
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
	int minus;
	unsigned long result;

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