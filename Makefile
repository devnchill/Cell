CC := gcc
CFLAGS := -g  

all: main

main: ./src/main.c
	$(CC) $(CFLAGS) -o ./src/main ./src/main.c

run: main
	./src/main

clean:
	rm -f ./src/main

