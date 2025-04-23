
NAME = webserv

BUILD_DIR = ./obj
SRC_DIR = ./src
INCL_DIR = ./inc

LIB = 
LIBS = $(addprefix -L, $(LIB))

SRCS = $(filter-out %_bonus.cpp, $(shell find $(SRC_DIR) -name *.cpp))
OBJS = $(subst $(SRC_DIR), $(BUILD_DIR), $(SRCS:.cpp=.o))

BONUS_SRCS = $(shell find $(SRC_DIR) -name *-bonus.cpp))
BONUS_OBJS = $(subst $(SRC_DIR), $(BUILD_DIR), $(BONUS_SRCS:.cpp=.o))

DEPS = $(OBJS:.o=.d)

INC_DIRS = $(shell find $(INCL_DIR) -type d)
INC_FLAGS = $(addprefix -I, $(INC_DIRS))

FOLDERS = uploads session

CC = c++
CPPFLAGS = $(INC_FLAGS) -Wall -Werror -Wextra -MMD -MP -g3 -std=c++98

LD = c++
LDFLAGS = $(LIBS) -g
LINKS =

all: $(NAME) $(FOLDERS)

$(FOLDERS):
	mkdir -p $@

$(NAME): $(OBJS)
 ifdef $(LIB)
	make -C $(LIB)
 endif
	$(LD) $(LDFLAGS) -o $@ $(OBJS) $(LINKS)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	mkdir -p $(@D)
	$(CC) $(CPPFLAGS) -c $< -o $@

bonus: $(BONUS_NAME)

$(BONUS_NAME): $(BONUS_OBJS)
 ifdef $(LIB)
	make -C $(LIB)
 endif
	$(LD) $(LDFLAGS) -o $@ $(BONUS_OBJS) $(BONUS_LINKS)

$(BUILD_DIR)/%_bonus.o: $(SRC_DIR)/%_bonus.cpp
	mkdir -p $(@D)
	$(CC) $(CPPFLAGS) -c $< -o $@

clean:
	$(RM) -r $(BUILD_DIR) $(FOLDERS)

fclean: clean
	$(RM) $(NAME)


re: fclean all

.PHONY: clean fclean re all bonus
