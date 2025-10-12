NAME = PmergeMe

# sources
TOOLBOXSRCS = ../toolbox/StepMark.cpp \
			../toolbox/color.cpp \
			../toolbox/string.cpp
SRCS = main.cpp \
    detail/utils.cpp \
	clock/clock.cpp \
	utils/test.cpp \
	benchmark/benchmark.cpp \
	${TOOLBOXSRCS}


OBJS = $(SRCS:.cpp=.o)

# compiler
CXX = c++
CXXFLAGS = -Wall -Wextra -Werror -std=c++98 -I.. -O3 # -pedantic ... for int64

# rules
.PHONY: all
all: $(NAME)

$(NAME): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(NAME) $(OBJS)

.PHONY: clean
clean:
	$(RM) $(OBJS)

.PHONY: fclean
fclean: clean
	$(RM) $(NAME)

.PHONY: clean-log
clean-log:
	$(RM) *.log

.PHONY: re
re: fclean all

.PHONY: run
run: re
	./$(NAME)