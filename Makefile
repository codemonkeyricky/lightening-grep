CXX 		:= g++
# CXX 		:= clang++-3.6

CXXFLAGS 	:= -mavx2

DFLAGS 		:= -g -O0
RFLAGS 		:= -O3
LDFLAGS		:= -lpthread

all : SearcherBFAVX2.cpp main.cpp cQueue.cpp cFileFinder.cpp
	$(CXX) $(CXXFLAGS) $(DFLAGS) $^ $(LDFLAGS) -std=gnu++11 -o gg_debug
	$(CXX) $(CXXFLAGS) $(RFLAGS) $^ $(LDFLAGS) -std=gnu++11 -o gg
