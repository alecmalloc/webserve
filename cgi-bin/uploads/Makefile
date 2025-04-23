NAME ?= fractol

BUILD_DIR ?= ./obj
SRC_DIRS ?= ./src
BONUS_DIRS ?= ./src/bonus

LIB = libft 
LIBS = $(addprefix -L ,$(LIB))

SRCS := $(shell find $(SRC_DIRS) -name *.c)
BONUS_SRCS := $(shell find $(BONUS_DIRS) -name *.c)

OBJS := $(subst $(SRC_DIRS), $(BUILD_DIR), $(SRCS:.c=.o))
BONUS_OBJS := $(subst $(BONUS_DIRS), $(BUILD_DIR), $(BONUS_SRCS:.c=.o))

DEPS := $(OBJS:.o=.d)
BONUS_DEPS := $(BONUS_OBJS:.o=.d)

INC_DIRS := $(shell find $(SRC_DIRS) -type d)
INC_FLAGS := $(addprefix -I,$(INC_DIRS))

CC = cc
CFLAGS ?= $(INC_FLAGS) -Wall -Werror -Wextra -MMD -MP

LD = cc
LDFLAGS = $(LIBS)
LINKS = -lft -lmlx -lXext -lX11 -lm

HIDE = @

all: $(NAME)

$(NAME): $(OBJS)
	@make -C $(LIB)
	@echo "\nLinking:"
	$(LD) $(LDFLAGS) -o $@ $(OBJS) $(LINKS)
	@echo "..\n"

$(BUILD_DIR)/%.o: $(SRC_DIRS)/%.c
	$(HIDE) mkdir -p $(@D)
	$(HIDE) $(CC) $(CFLAGS) -c $< -o $@
	@echo "Compiling $< ..\n"

bonus: $(BONUS_OBJS)
	@make -C $(LIB)
	@echo "\nLinking:"
	$(LD) $(LDFLAGS) -o $(NAME) $(OBJS) $(LINKS)
	@echo "..\n"

$(BUILD_DIR)/%.o: $(BONUS_DIRS)/%.c
	$(HIDE) mkdir -p $(@D)
	$(HIDE) $(CC) $(CFLAGS) -c $< -o $@
	@echo "Compiling $< ..\n"

.PHONY: clean fclean re debug

clean:
	$(HIDE) $(RM) -r $(BUILD_DIR)
	@echo "removing obj/ ..\n"

fclean: clean
	$(HIDE) $(RM) $(NAME)
	@echo "removing $(NAME) ..\n"

re: fclean all

-include $(DEPS)
