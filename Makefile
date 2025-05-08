CC := gcc
CFLAGS := -g  
LFAGS := -lreadline -lhistory

all: main

main: ./src/main.c
	$(CC) $(CFLAGS) -o ./src/main ./src/main.c $(LFAGS)

run: main
	./src/main

clean:
	rm -f ./src/main

