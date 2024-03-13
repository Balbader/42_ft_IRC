########
# NAME #
########
NAME 				:=	./ircserv

###############
# INGREDIENTS #
###############
OBJECTSDIR 			:= 	objects
HEADERS_FOLDER 		:= 	headers

SRCS_FOLDER 		:=	srcs
SRC_FILES 			:=	\
						Client.cpp \
						Server.cpp \
						commands.cpp \
						getSocket.cpp \
						messagesHandling.cpp \
						pollHandling.cpp \
						requestHandling.cpp \
						Channel.cpp \
						utils.cpp \
						PartCommand.cpp \
						JoinCommand.cpp \
						KickCommand.cpp \
						PrivmsgCommand.cpp \
						Request.cpp \
						File.cpp \

HEADERS_FILES 		:=	headers/Client.hpp \
						headers/Server.hpp \
						headers/Channel.hpp \
						headers/Request.hpp \
						headers/File.hpp \

MAIN 				:=	main.cpp

OBJS 				:=	$(addprefix $(OBJECTSDIR)/, $(SRC_FILES:.cpp=.o))
MAIN_OBJ 			:=	$(addprefix $(OBJECTSDIR)/, $(MAIN:.cpp=.o))

CC 					:=	c++
FLAGS 				:= 	-Wall -Wextra -Werror -g3
CPP_STANDARD 		:=	-std=c++98

RM 					:=	rm -rf
RMDIR 				:=	rm -rf
MKDIR 				:= 	mkdir -p
MAKE 				:=	make -C

ECHO 				:=	/bin/echo
ERRIGNORE			:=	2>/dev/null

##########
# COLORS #
##########
RED 				:= 	\033[1;31m
GREEN				:=	\033[1;32m
YELLOW 				:=	\033[1;33m
BLUE				:=	\033[1;34m
RESET				:=	\033[0m

###########
# RECIPES #
###########

all: $(NAME)
	@echo "$(BLUE)Compiling is DONE $(RESET)"

head:
	@echo "$(BLUE)Making ft_irc Server $(RESET)"

$(NAME): head $(OBJS) $(MAIN_OBJ) $(HEADERS_FILES)
	@$(CC) $(CPP_STANDARD) $(OBJECTSDIR)/main.o $(OBJS) -o $@ -g

$(OBJECTSDIR)/%.o: $(SRCS_FOLDER)/%.cpp $(HEADERS_FILES)
	@$(MKDIR) $(dir $@)
	@printf "$(YELLOW)Compiling$(RESET) $<:\r\t\t\t\t\t\t\t..."
	@$(CC) $(CPP_STANDARD) $(FLAGS) -o $@ -c $< -g
	@echo "\r\t\t\t\t\t\t\t$(GREEN){DONE}$(RESET)"

$(OBJECTSDIR)/%.o: main.cpp $(HEADERS_FILES)
	@printf "$(YELLOW)Compiling$(RESET) $<:\r\t\t\t\t\t\t\t..."
	@$(CC) $(CPP_STANDARD) $(FLAGS) -o $@ -c $< -g
	@echo "\r\t\t\t\t\t\t\t$(GREEN){DONE}$(RESET)"

clean:
	@echo "$(RED)deleting$(RESET): " $(OBJECTSDIR)
	@$(RMDIR) $(OBJECTSDIR)


fclean: clean
	@echo "$(RED)deleting$(RESET): " $(NAME)
	@$(RM) $(NAME) $(ERRIGNORE)

re: fclean all
