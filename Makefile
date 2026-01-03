SRC_DIR := src
CFLAGS:= -Iinclude
SOURCES := $(wildcard $(SRC_DIR)/*.c)

build/target: $(SOURCES)
	mkdir -p build
	gcc $(CFLAGS) $^ -o $@

run:build/target
	./build/target

clean:
	rm -rf build

