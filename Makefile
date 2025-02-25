# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: mokariou <mokariou@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/02/22 14:51:14 by mokariou          #+#    #+#              #
#    Updated: 2025/02/25 18:53:30 by mokariou         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME =  webserv
CC = c++
GFLAGS = -Wall -Wextra -Werror -std=c++98 -g3 -fsanitize=address

OBJ_DIR = obj/
CFG_DER = Config/
SRC =	ConfigParse.cpp main.cpp server.cpp Request.cpp Response.cpp cgi.cpp \
		utils_resp.cpp $(CFG_DER)Config.cpp $(CFG_DER)ParseConfig.cpp

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