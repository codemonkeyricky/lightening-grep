CXX 		:= g++

CXXFLAGS 	:= -mavx2

DFLAGS 		:= -g -O0
RFLAGS 		:= -O3

all : SearcherBFAVX2.cpp main.cpp cQueue.cpp
	$(CXX) $(CXXFLAGS) $(DFLAGS) $^ -std=gnu++11 -o gg_debug
	$(CXX) $(CXXFLAGS) $(RFLAGS) $^ -std=gnu++11 -o gg
