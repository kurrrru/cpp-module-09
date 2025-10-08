NAME = btc

# sources
TOOLBOXSRCS = ../toolbox/StepMark.cpp \
			../toolbox/color.cpp \
			../toolbox/string.cpp
SRCS = main.cpp \
	calendar_system/EthiopianCalendar.cpp \
	calendar_system/FrenchRepublicanCalendar.cpp \
	calendar_system/GregorianCalendar.cpp \
	calendar_system/JulianCalendar.cpp \
	calendar_system/NonProlepticGregorianCalendar.cpp \
	Date.cpp \
	BitcoinExchange.cpp \
	string.cpp \
	${TOOLBOXSRCS}


OBJS = $(SRCS:.cpp=.o)

# compiler
CXX = c++
CXXFLAGS = -Wall -Wextra -Werror -std=c++98 -pedantic -I..

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
