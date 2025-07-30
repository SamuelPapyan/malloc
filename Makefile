ifeq ($(HOSTTYPE),)
HOSTTYPE := $(shell uname -m)_$(shell uname -s)
endif

NAME = libft_malloc_$(HOSTTYPE).so
NAME_LINK = libft_malloc.so
CC = gcc

SRC_DIR = src
OBJ_DIR = obj
INC_DIR = include

LIBFT_DIR = libft
LIBFT_LIB = $(LIBFT_DIR)/libft.a
LIBFT_INC = -I$(LIBFT_DIR)
LDFLAGS = -shared

CFLAGS = -Wall -Wextra -Werror -fPIC -Iinclude $(LIBFT_INC)

SRCS = $(wildcard $(SRC_DIR)/*.c)
OBJS = $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(SRCS))

LIBS = $(LIBFT_LIB)

all: $(LIBFT_LIB) $(NAME)

$(NAME): $(OBJS)
	$(CC) $(OBJS) -o $@ $(LIBS) $(LDFLAGS)
	ln -s $(NAME) $(NAME_LINK)

$(LIBFT_LIB):
	$(MAKE) -C $(LIBFT_DIR)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

clean:
	$(MAKE) -C $(LIBFT_DIR) clean
	rm -rf $(OBJ_DIR)

fclean: clean
	$(MAKE) -C $(LIBFT_DIR) fclean
	rm -f $(NAME)
	rm -f $(NAME_LINK)

re: fclean all

.PHONY: all clean fclean re