NAME = rpn

# sources
TOOLBOXSRCS = ../toolbox/StepMark.cpp \
			../toolbox/color.cpp
SRCS = main.cpp \
    Rational.cpp \
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

# STACK
STACK := 
push = $(eval STACK := $(1) $(STACK))

define do_op
    $(eval STACK := $(shell \
        stack="$(STACK)"; \
        op="$(1)"; \
        \
        b=$$(echo $$stack | cut -d' ' -f1); \
        stack_after_b=$$(echo $$stack | cut -d' ' -f2-); \
        a=$$(echo $$stack_after_b | cut -d' ' -f1); \
        rest_of_stack=$$(echo $$stack_after_b | cut -d' ' -f2-); \
        \
        if [ -z "$$a" ] || [ -z "$$b" ]; then \
            echo "$(STACK)"; \
        else \
            result=$$(($$a $$op $$b)); \
            echo "$$result $$rest_of_stack"; \
        fi \
    ))
endef

process_token = $(if $(filter $(1),+ - * /), $(call do_op,$(1)), $(call push,$(1)))

RPN_EXPR ?= 5 3 + 8 2 - *

.PHONY: solve
solve:
	$(eval STACK :=)
	$(eval $(foreach token,$(RPN_EXPR),$(call process_token,$(token))))
	@echo "計算する式: $(RPN_EXPR)"
	@echo "計算結果: $(firstword $(STACK))"