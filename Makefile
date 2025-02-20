SRC = Header.cpp MessageException.cpp Request.cpp Response.cpp Server.cpp http.cpp test.cpp 

OBJ = ${SRC:.cpp=.o}

NAME = webserv

CPPFLAGS = -Wall -Wextra -Werror -g3
STDFLAG = -std=c++98

all: ${NAME}

.cpp.o:
	c++ ${CPPFLAGS} ${STDFLAG} -c $< -o ${<:.cpp=.o}

clean:
	rm -f ${OBJ}

fclean: clean
	rm -f ${NAME}

re : fclean all

${NAME}: ${OBJ}
	c++ ${CPPFLAGS} ${STDFLAG} ${OBJ} -o ${NAME}

.PHONY: all clean fclean re