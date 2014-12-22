CPP=g++
CPPFLAGS=-std=c++11

all:raii
raii: raii.cc
	$(CPP) $(CPPFLAGS) raii.cc -o raii
