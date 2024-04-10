NAME = ircserv

HEADER = inc
SRC_DIR = srcs
OBJ_DIR = obj

MAIN = $(SRC_DIR)/main.cpp
SRCS = $(addprefix $(SRC_DIR)/, Server.cpp EventHandler.cpp) 		\
       $(addprefix $(SRC_DIR)/, ConnectionManager.cpp ChannelManager.cpp) 			\
       $(addprefix $(SRC_DIR)/, Commands.cpp CommandExecutor.cpp ) 		\
       $(addprefix $(SRC_DIR)/, IO.cpp Replies.cpp ServerUtils.cpp)	\
       $(addprefix $(SRC_DIR)/, Channel.cpp Client.cpp Logger.cpp utils.cpp)

OBJS = $(SRCS:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)
OBJ_MAIN = $(MAIN:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)

CPPFLAGS = -Wall -Wextra -Werror -std=c++98 -g
CC = c++

RM = rm -rf
MKDIR_P = mkdir -p


all:	$(OBJ_DIR) $(NAME)

$(OBJ_DIR):
	$(MKDIR_P) $(OBJ_DIR)

$(NAME):    $(OBJS) $(OBJ_MAIN)
	$(CC) $^ -o $@

$(OBJ_DIR)/%.o:	$(SRC_DIR)/%.cpp
	$(CC) -c $(CPPFLAGS) -I$(HEADER)/ $< -o $@

$(OBJ_MAIN).o:	$(SRC_DIR).cpp
	$(CC) -c $(CPPFLAGS) -I$(HEADER)/ $< -o $@

clean:		
	$(RM) $(OBJ_DIR)

fclean:	clean
	$(RM) $(NAME)

re:	fclean all

.PHONY: all clean fclean re
