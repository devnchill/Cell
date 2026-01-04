SRC_DIR := src
CFLAGS:= -Iinclude
SOURCES := $(shell find src -name '*.c')

build/target: $(SOURCES)
	mkdir -p build
	gcc $(CFLAGS) $^ -o $@

run:build/target
	./build/target

clean:
	rm -rf build

