########
# NAME #
########
NAME		:=	IRC

###############
# INGREDIENTS #
###############
INC_DIR		:=	./inc

SRCS_DIR	:=	./srcs
SRCS		:=	\
				main.cpp
SRCS		:=	$(SRCS:%=$(SRCS_DIR)/%)

BUILD_DIR	:=	.build
OBJS		:=	$(SRCS:$(SRCS_DIR)/%.cpp=$(BUILD_DIR)/%.o)
DEPS		:=	$(OBJS:.o=.d)

CC			:=	c++ -o
CFLAGS		:=	-Wall -Wextra -Werror -std=c++98
IFLAGS		:=	$(addprefix -I, $(INC_DIR))

RM			:=	rm -rf
DIR_DUP		=	mkdir -p $(dir $@)

##########
# COLORS #
##########
RED			:=	\033[0;31m
GREEN		:=	\033[0;32m
YELLOW		:=	\033[0;33m
RESET		:=	\033[0;0m

###########
# RECIPES #
###########
all: $(NAME)

$(NAME): $(OBJS)
	@echo "[" "$(YELLOW)..$(RESET)" "] | Compiling $(NAME)..."
	@$(CC) $(CFLAGS) $(OBJS) -o $(NAME)
	@echo "[" "$(GREEN)OK$(RESET)" "] | $(NAME) ready!"

$(BUILD_DIR)/%.o: $(SRCS_DIR)/%.cpp
	@$(DIR_DUP)
	@$(CC) $(CFLAGS) $(IFLAGS) -c -o $@ $<

-include $(DEPS)

clean:
	@echo "[" "$(YELLOW)..$(RESET)" "] | Removing object files...$(RESET)"
	@$(RM) $(BUILD_DIR) $(DEPS)
	@$(RM) $(OBJS)
	@echo "[" "$(GREEN)OK$(RESET)" "] | Object files removed."

fclean: clean
	@echo "[" "$(YELLOW)..$(RESET)" "] | Removing binary files...$(RESET)"
	@$(RM) $(NAME)
	@echo "[" "$(GREEN)OK$(RESET)" "] | Binary files removed."

re:
	@$(MAKE) fclean
	@$(MAKE) all

.PHONY: all clean fclean re
