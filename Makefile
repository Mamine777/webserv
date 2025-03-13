# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: mokariou <mokariou>                        +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/03/11 12:46:57 by mokariou          #+#    #+#              #
#    Updated: 2025/03/13 10:24:09 by mokariou         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

SRC_PATH = src/
OBJ_PATH = obj/
INC_PATH = inc/
OUT_PATH = bin/

SRC_FILES = Http.cpp Request.cpp Response.cpp cgi.cpp main.cpp server.cpp utils_resp.cpp\
			Config/Config.cpp Config/ParseConfig.cpp

SRC = $(addprefix ${SRC_PATH}, ${SRC_FILES})
OBJ = $(patsubst $(SRC_PATH)%.cpp, $(OBJ_PATH)%.o, ${SRC})
FINAL = bin/webserv

OBJ_DIRS := $(sort $(dir ${OBJ})) 
FLAGS = -Wall -Wextra -Werror -std=c++98 -g3 -fsanitize=address

all: ${FINAL}

${FINAL}: ${OBJ} | ${OUT_PATH}
	c++ ${FLAGS} -I ${INC_PATH} -o $@ $^

${OBJ_PATH}%.o: ${SRC_PATH}%.cpp | $(OBJ_DIRS)
	c++ ${FLAGS} -I ${INC_PATH} -c $< -o $@

$(OBJ_DIRS):

	mkdir -p $@

${OUT_PATH}:
	mkdir -p ${OUT_PATH}

clean:
	rm -rf ${OBJ_PATH}

fclean: clean
	rm -rf ${OUT_PATH}

re: fclean all

.PHONY: all clean fclean re
