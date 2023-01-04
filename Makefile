.PHONY: all clean dir

CC=gcc
CXX=g++

CFLAGS =
CPPFLAGS = -g

SRC_C = opcode_table.c
SRC_CPP = common.cpp ir.cpp main.cpp parse.cpp encode.cpp inst_internal.cpp

SRC_O = $(addprefix build/, $(SRC_C:.c=.o) $(SRC_CPP:.cpp=.o))

all: dir instrument

dir:
	mkdir -p build

build/%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

build/%.o: %.cpp
	$(CXX) $(CPPFLAGS) -c -o $@ $<

instrument: $(SRC_O)
	$(CXX) $(CPPFLAGS) -o $@ $^ -lm
	cp $@ build/$@

build-test:
	cd tests; ./build.sh

clean-test:
	cd tests; rm *.wasm

clean:
	rm -rf build

