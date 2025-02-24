# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: mokariou <mokariou@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/02/22 14:51:14 by mokariou          #+#    #+#              #
#    Updated: 2025/02/23 18:41:35 by mokariou         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME =  webserv
CC = c++
GFLAGS = -Wall -Wextra -Werror -std=c++98 #-pedantic -fsanitize=address

OBJ_DIR = obj/
SRC = ConfigParse.cpp main.cpp server.cpp Request.cpp Response.cpp utils_resp.cpp cgi.cpp

SRCS = $(SRC)
OBJ = $(patsubst %.c, $(OBJ_DIR)%.o, $(SRCS))

all: $(NAME)

$(NAME): $(OBJ)
			$(CC) $(GFLAGS) $(OBJ) -o $(NAME)


$(OBJ_DIR)%.o: %.c
			@mkdir -p $(OBJ_DIR)
			@mkdir -p $(dir $@)
			$(CC) $(GFLAGS) -c $< -o $@
clean:
		rm -rf $(OBJ_DIR)
fclean: clean
		rm -f $(NAME)
re: fclean all