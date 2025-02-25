SRC_PATH = src/
OBJ_PATH = obj/
INC_PATH = includes/
OUT_PATH = bin/

SRC_FILES = Header.cpp Http.cpp MessageException.cpp Request.cpp Response.cpp Server.cpp StaticHandler.cpp test.cpp \
			AutoIndexHandler.cpp

SRC = $(addprefix ${SRC_PATH}, ${SRC_FILES})
OBJ = $(patsubst $(SRC_PATH)%.cpp, $(OBJ_PATH)%.o, ${SRC})
FINAL = bin/webserv

FLAGS = -Wall -Wextra -Werror -std=c++98

all: ${FINAL}

${FINAL}: ${OBJ} | ${OUT_PATH}
	c++ ${FLAGS} -I ${INC_PATH} -o $@ $^

${OBJ_PATH}%.o: ${SRC_PATH}%.cpp | ${OBJ_PATH}
	c++ ${FLAGS} -I ${INC_PATH} -c $< -o $@

${OBJ_PATH}:
	mkdir -p ${OBJ_PATH}

${OUT_PATH}:
	mkdir -p ${OUT_PATH}

clean:
	rm -rf ${OBJ_PATH}

fclean: clean
	rm -rf ${OUT_PATH}

re: fclean all

.PHONY: all clean fclean re
