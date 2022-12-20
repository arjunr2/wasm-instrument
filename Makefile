.PHONY: all clean dir bench

CC=gcc
CXX=g++

CFLAGS = -g
CPPFLAGS = -g

SRC_C = weerun.c common.c ir.c test.c weedis.c
#SRC_CPP = xbyak_c_api.cpp

SRC_O = $(addprefix build/, $(SRC_C:.c=.o) $(SRC_CPP:.cpp=.o))

all: dir weerun

dir:
	mkdir -p build

test: weerun
	./weerun -test

build/%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

build/%.o: %.cpp
	$(CXX) $(CPPFLAGS) -c -o $@ $<

weerun: $(SRC_O)
	$(CXX) $(CPPFLAGS) -o weerun $^ -lm
	cp $@ build/$@

build-test: weerun
	cd tests; ./build.sh

clean:
	rm -f weerun
	rm -rf build

