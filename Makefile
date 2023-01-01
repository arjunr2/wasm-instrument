.PHONY: all clean dir

CC=g++
CXX=g++

CFLAGS = -g
CPPFLAGS = -g

SRC_C = common.c
SRC_CPP = ir.cpp main.cpp parse.cpp

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

