# philo
NAME = philo
SRCS = philo.c
OBJS = $(SRCS:%.c=%.o)
# Compiler
CC = cc
CFLAGS = -Wall -Wextra -Werror

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