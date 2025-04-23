name    = megaphone
OBJS    = $(name).o
SOURCE  = $(name).cpp
OUT     = $(name)
CC      = c++
FLAGS   = -Wall -Wextra -Werror

all: $(OBJS)
	$(CC) -g $(OBJS) -o $(OUT)

$(name).o: $(name).cpp
	$(CC) $(FLAGS) -c $(name).cpp -std=c++98

clean:
	rm -f $(OBJS) $(OUT)

fclean:
	rm -f $(name) *.o