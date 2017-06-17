CXX 		:= g++
# CXX 		:= clang++-3.6

CXXFLAGS 	:= -march=native

DFLAGS 		:= -g -O0
RFLAGS 		:= -g -O3
LDFLAGS		:= -lpthread # -lstdc++fs

all : main.cpp cPatternFinder.cpp cFileFinder.cpp cPrinter.cpp cGrep.cpp cQueueLockless.cpp
	$(CXX) $(CXXFLAGS) $(DFLAGS) $^ $(LDFLAGS) -std=gnu++11 -o gg_debug
	$(CXX) $(CXXFLAGS) $(RFLAGS) $^ $(LDFLAGS) -std=gnu++11 -o gg

install : 
	cp gg /usr/local/bin
