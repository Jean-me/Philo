# philo
NAME = philo
SRCS = philo.c routine.c utils.c monitor.c ft_atoi.c init.c routine_actions.c
OBJS = $(SRCS:%.c=%.o)
# Compiler
CC = cc
CFLAGS = -Wall -Wextra -Werror -g

# Rules
all: $(NAME)

$(NAME):	$(OBJS) 
	$(CC) $(OBJS) -o $(NAME)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	$(RM) $(OBJS)

fclean:	clean
	$(RM) $(NAME)

re:		fclean all

.PHONY:	all clean fclean re