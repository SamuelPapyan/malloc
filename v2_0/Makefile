# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: spapyan <spapyan@student.42.fr>            +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2026/02/28 16:15:46 by spapyan           #+#    #+#              #
#    Updated: 2026/02/28 16:15:46 by spapyan          ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

ifeq ($(HOSTTYPE),)
	HOSTTYPE := $(shell uname -m)_$(shell uname -s)
endif

NAME = libft_malloc_$(HOSTTYPE).so
LINK = libft_malloc.so

SRCS_DIR = srcs
INCS_DIR = includes
OBJS_DIR = objs

CC = gcc
CFLAGS = -Wall -Wextra -Werror -fPIC
INCFLAGS = -I$(INCS_DIR)
LDFLAGS = -shared

SRCS = $(wildcard $(SRCS_DIR)/*.c)
OBJS = $(SRCS:$(SRCS_DIR)/%.c=$(OBJS_DIR)/%.o)

all: $(NAME)

$(NAME): $(OBJS)
	$(CC) $(LDFLAGS) -o $(NAME) $(OBJS)
	ln -sf $(NAME) $(LINK)
	@echo "libft_malloc.so generate successfully."
	

$(OBJS_DIR)/%.o: $(SRCS_DIR)/%.c
	@mkdir -p $(OBJS_DIR)
	$(CC) $(CFLAGS) $(INCFLAGS) -c $< -o $@

clean:
	rm -rf $(OBJS_DIR)
	@echo "Object files removed successfully."

fclean: clean
	rm -f $(NAME) $(LINK)
	@echo "libft_malloc.so removed successfully."

re: fclean all

.PHONY: all clean fclean re