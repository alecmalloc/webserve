NAME    = webserv
CC      = c++
FLAGS   = -Wall -Wextra -Werror -std=c++98
OUT     = $(NAME)

# Directories
SRC_DIR = src
INC_DIR = inc
OBJ_DIR = obj

# Find all .cpp files in the src directory
SOURCES = $(wildcard $(SRC_DIR)/*.cpp)

# Convert the .cpp files to .o files and place them in obj directory
OBJS    = $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SOURCES))

# Include path
INC     = -I$(INC_DIR)

# Compile all object files and link into the final executable
all: $(OUT)

$(OUT): $(OBJS)
	$(CC) -g $(OBJS) -o $(OUT)

# Pattern rule for object files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(OBJ_DIR)
	$(CC) $(FLAGS) $(INC) -c $< -o $@

clean:
	rm -rf $(OBJ_DIR)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re