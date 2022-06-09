# Piotr Dobiech 316625

CC = gcc
CFLAGS = -std=gnu17 -Wall -Wextra -Werror -pedantic -g
LDFLAGS =

BINARY_NAME := webserver
SRCS := utils.c io.c server.c main.c
OBJS := $(SRCS:%.c=%.o)

$(BINARY_NAME): $(OBJS)
	$(CC) $(OBJS) -o $@ $(LDFLAGS)

%.o: %.c
	$(CC) -c $(CFLAGS) $< -o $@

clean:
	rm $(OBJS)

cleandist:
	rm $(OBJS) $(BINARY_NAME)
