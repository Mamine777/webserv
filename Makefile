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

NAME = webserv
CC = c++
GFLAGS = -Wall -Wextra -Werror -std=c++98 -g3 -fsanitize=address

OBJ_DIR = obj/
CFG_DIR = src/Config/

SRC =	src/main.cpp src/server.cpp src/Request.cpp src/Response.cpp src/cgi.cpp \
		src/utils_resp.cpp $(CFG_DIR)Config.cpp $(CFG_DIR)ParseConfig.cpp

OBJ = $(patsubst src/%.cpp, $(OBJ_DIR)%.o, $(SRC))

all: $(NAME)

$(NAME): $(OBJ)
	$(CC) $(GFLAGS) $(OBJ) -o $(NAME)

$(OBJ_DIR)%.o: src/%.cpp
	@mkdir -p $(dir $@)
	$(CC) $(GFLAGS) -c $< -o $@

clean:
	rm -rf $(OBJ_DIR)

fclean: clean
	rm -f $(NAME)

re: fclean all
