.PHONY: all clean dir

CC=gcc
CXX=g++

CFLAGS = -Iinc -I.
CPPFLAGS = -O3 -g -std=c++2a -Iinc -I.

SRC_C = $(notdir $(wildcard src/*.c))
SRC_CPP = $(notdir $(wildcard src/*.cpp) main.cpp $(wildcard routines/*.cpp))


SRC_O = $(addprefix build/, $(SRC_C:.c=.o) $(SRC_CPP:.cpp=.o))

all: dir instrument

dir:
	mkdir -p build

build/%.o: src/%.c
	$(CC) $(CFLAGS) -c -o $@ $<

build/%.o: src/%.cpp
	$(CXX) $(CPPFLAGS) -c -o $@ $<

build/%.o: routines/%.cpp
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

