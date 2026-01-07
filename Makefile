SRC_DIR := src
CFLAGS:= -Iinclude
LIBFLAGS := -lreadline
SOURCES := $(shell find src -name '*.c')

build/target: $(SOURCES)
	mkdir -p build
	gcc  $(CFLAGS) $^ $(LIBFLAGS) -o $@

run:build/target
	./build/target

clean:
	rm -rf build

