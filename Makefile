CXX 		:= g++
# CXX 		:= clang++-3.6

CXXFLAGS 	:= -march=native

DFLAGS 		:= -g -O0
RFLAGS 		:= -O3
LDFLAGS		:= -lpthread

all : cSearcherNative.cpp main.cpp cQueue.cpp cFileFinder.cpp cPrinter.cpp cGrep.cpp 
	$(CXX) $(CXXFLAGS) $(DFLAGS) $^ $(LDFLAGS) -std=gnu++11 -o gg_debug
	$(CXX) $(CXXFLAGS) $(RFLAGS) $^ $(LDFLAGS) -std=gnu++11 -o gg
