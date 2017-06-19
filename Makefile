CXX 		:= g++

CFLAGS 		:= -Igrep/api -std=gnu++11
LDFLAGS		:= -Lgrep -lgrep -lpthread 
LDFLAGS_D   := -Lgrep -lgrep_d -lpthread

all : gg_g.o gg.o
	make -C grep
	$(CXX) $(CFLAGS) gg.o -o gg $(LDFLAGS)           # release
	$(CXX) $(CFLAGS) gg_g.o -o gg_debug $(LDFLAGS_D) # debug

gg_g.o : gg.cpp
	$(CXX) $(CFLAGS) -O0 -g -c -o gg_g.o $<

gg.o : gg.cpp
	$(CXX) $(CFLAGS) -O3 -c -o gg.o $<

rc : rc.cpp 
	make -C grep 
	$(CXX) $(CFLAGS) -O0 -g -c -o rc_g.o $<
	$(CXX) $(CFLAGS) rc_g.o -o rc $(LDFLAGS_D) # debug

install : 
	cp gg /usr/local/bin
