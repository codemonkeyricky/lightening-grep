CXX 		:= g++

CFLAGS 		:= -Igrep/api -std=gnu++11
LDFLAGS		:= -Lgrep -lgrep -lpthread 
LDFLAGS_D   := -Lgrep -lgrep_d -lpthread 

all : gg_g.o gg.o
	make -C grep
	$(CXX) $(CFLAGS) gg.o -o gg $(LDFLAGS)           # release
	$(CXX) $(CFLAGS) gg_g.o -o gg_debug $(LDFLAGS_D) # debug

gg_g.o : gg.cpp
	$(CXX) $(CFLAGS) -O0 -c -o gg_g.o $<

gg.o : gg.cpp
	$(CXX) $(CFLAGS) -O3 -c -o gg.o $<

install : 
	cp gg /usr/local/bin
