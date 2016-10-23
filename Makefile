CXX 		:= g++-5
# CXX 		:= clang++-3.6

CXXFLAGS 	:= -mavx2

DFLAGS 		:= -g -O0
RFLAGS 		:= -O3
LDFLAGS		:= -lpthread

all : cSearcherNative.cpp main.cpp cQueue.cpp cFileFinder.cpp SearcherRabinKarpAVX2.cpp cPrinter.cpp cGrep.cpp 
	$(CXX) $(CXXFLAGS) $(DFLAGS) $^ $(LDFLAGS) -std=gnu++11 -o gg_debug
	$(CXX) $(CXXFLAGS) $(RFLAGS) $^ $(LDFLAGS) -std=gnu++11 -o gg
