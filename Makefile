.PHONY: all clean dir

CC=gcc
CXX=g++

CFLAGS = -I./inc -I.
CPPFLAGS = -g -std=c++2a -I./inc -I.

SRC_C = $(notdir $(wildcard src/*.c))
SRC_CPP = $(notdir $(wildcard src/*.cpp) main.cpp routines.cpp)


SRC_O = $(addprefix build/, $(SRC_C:.c=.o) $(SRC_CPP:.cpp=.o))

all: dir instrument

dir:
	mkdir -p build

build/%.o: src/%.c
	$(CC) $(CFLAGS) -c -o $@ $<

build/%.o: src/%.cpp
	$(CXX) $(CPPFLAGS) -c -o $@ $<

build/%.o: %.cpp
	$(CXX) $(CPPFLAGS) -c -o $@ $<


instrument: $(SRC_O)
	$(CXX) $(CPPFLAGS) -o $@ $^ -lm
	cp $@ build/$@

build-test:
	cd tests; ./build.sh

clean-test:
	cd tests; rm *.wasm

gen-insns:
	python3 utility/generate_insns.py > instructions.h

clean:
	rm -rf build

