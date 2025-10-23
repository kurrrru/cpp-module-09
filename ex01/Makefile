NAME = RPN

# sources
TOOLBOXSRCS = ../toolbox/StepMark.cpp \
			../toolbox/color.cpp \
            ../toolbox/string.cpp
SRCS = main.cpp \
    Rational.cpp \
	RPN.cpp \
    BigInt_basic.cpp \
    BigInt_calculation.cpp \
    BigInt_comparison.cpp \
    BigInt_conversion.cpp \
	${TOOLBOXSRCS}

OBJS = $(SRCS:.cpp=.o)

# compiler
CXX = c++
CXXFLAGS = -Wall -Wextra -Werror -std=c++98 -pedantic -I..

# tester
TEST_PY = rpn.py

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
	$(RM) $(NAME) $(TEST_PY)

.PHONY: clean-log
clean-log:
	$(RM) *.log

.PHONY: re
re: fclean all

.PHONY: py_create
py_create: $(TEST_PY)

$(TEST_PY):
	@echo "#!/usr/bin/env python3" > $(TEST_PY)
	@echo "import sys" >> $(TEST_PY)
	@echo "" >> $(TEST_PY)
	@echo "def rpn(expression):" >> $(TEST_PY)
	@echo "    stack = []" >> $(TEST_PY)
	@echo "    tokens = expression.split()" >> $(TEST_PY)
	@echo "    for token in tokens:" >> $(TEST_PY)
	@echo "        if token in ('+', '-', '*', '/'):" >> $(TEST_PY)
	@echo "            b = stack.pop()" >> $(TEST_PY)
	@echo "            a = stack.pop()" >> $(TEST_PY)
	@echo "            if token == '+':" >> $(TEST_PY)
	@echo "                stack.append(a + b)" >> $(TEST_PY)
	@echo "            elif token == '-':" >> $(TEST_PY)
	@echo "                stack.append(a - b)" >> $(TEST_PY)
	@echo "            elif token == '*':" >> $(TEST_PY)
	@echo "                stack.append(a * b)" >> $(TEST_PY)
	@echo "            elif token == '/':" >> $(TEST_PY)
	@echo "                stack.append(a / b)" >> $(TEST_PY)
	@echo "        else:" >> $(TEST_PY)
	@echo "            stack.append(int(token))" >> $(TEST_PY)
	@echo "    return stack[0]" >> $(TEST_PY)
	@echo "" >> $(TEST_PY)
	@echo "if __name__ == '__main__':" >> $(TEST_PY)
	@echo "    if len(sys.argv) != 2:" >> $(TEST_PY)
	@echo "        print('Usage: python RPN.py \"<expression>\"')" >> $(TEST_PY)
	@echo "        sys.exit(1)" >> $(TEST_PY)
	@echo "    expression = sys.argv[1]" >> $(TEST_PY)
	@echo "    try:" >> $(TEST_PY)
	@echo "        result = rpn(expression)" >> $(TEST_PY)
	@echo "    except (IndexError, ValueError, ZeroDivisionError) as e:" >> $(TEST_PY)
	@echo "        print(f\"Error evaluating expression: {e}\")" >> $(TEST_PY)
	@echo "        sys.exit(1)" >> $(TEST_PY)
	@echo "    print(result)" >> $(TEST_PY)
	@chmod +x $(TEST_PY)
	@echo "Generated $(TEST_PY) and made it executable."

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
	@echo "RPN: $(RPN_EXPR)"
	@echo "Result: $(firstword $(STACK))"
