.PHONY: all clean dir

CC=gcc
CXX=g++

CFLAGS = -O3 -Iinc -I.
CPPFLAGS = -O0 -g -std=c++2a -Iinc -I. -pthread

SRC_C = $(notdir $(wildcard src/*.c))
SRC_CPP = $(notdir $(wildcard src/*.cpp))
ROUTINES_CPP = $(notdir $(wildcard api/routines/*.cpp) $(wildcard api/*.cpp))

SRC_O = $(addprefix build/, $(SRC_C:.c=.o) $(SRC_CPP:.cpp=.o))
ROUTINES_O = $(addprefix build/, $(ROUTINES_CPP:.cpp=.o))

INSTLIB = build/libinstrument.a


all: dir instrument $(INSTLIB)

dir:
	mkdir -p build


build/%.o: src/%.c
	$(CC) $(CFLAGS) -c -o $@ $<

build/%.o: src/%.cpp
	$(CXX) $(CPPFLAGS) -c -o $@ $<

build/%.o: api/routines/%.cpp
	$(CXX) $(CPPFLAGS) -c -o $@ $<

build/%.o: api/%.cpp
	$(CXX) $(CPPFLAGS) -c -o $@ $<

build/%.o: %.cpp
	$(CXX) $(CPPFLAGS) -c -o $@ $<


# ---------- CORE FILES ------------- #
instrument: $(SRC_O) build/main.o $(ROUTINES_O)
	$(CXX) $(CPPFLAGS) -o $@ $^ -lm
	cp $@ build/$@

# ---------- LIBRARY ------------- #
$(INSTLIB): $(SRC_O) $(ROUTINES_O)
	ar rcs $@ $^


# --------- MICRO TESTS --------- #
# Test LEB encoding/decoding 
TEST_CPP = micro/test-leb.cpp src/common.cpp
# Test Library
INSTLIB_MAIN = micro/test-instlib.c

microtests: dir build/test-instlib build/test-leb

build/test-leb: $(TEST_CPP)
	$(CXX) $(CPPFLAGS) -o $@ $^

build/test-instlib: $(INSTLIB_MAIN) $(INSTLIB) 
	$(CC) $(CFLAGS) $^ -lpthread -lstdc++ -o $@
	
#-------------------------------------#

build-test:
	cd tests; ./build.sh

clean-test:
	cd tests; rm *.wasm

gen-insns:
	python3 utility/generate_insns.py > instructions.h

clean:
	rm -rf build

