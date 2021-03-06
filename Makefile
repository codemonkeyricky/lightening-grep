CXX 		:= g++

CFLAGS 		:= -Igrep/api -std=gnu++11
LDFLAGS		:= -Lgrep -lgrep -lpthread 
LDFLAGS_D   := -Lgrep -lgrep_d -lpthread

all : gg rc 

gg : gg_g.o gg.o
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
	$(CXX) $(CFLAGS) rc_g.o -o rc_debug $(LDFLAGS_D) # debug
	$(CXX) $(CFLAGS) -O3 -g -c -o rc.o $<
	$(CXX) $(CFLAGS) rc.o -o rc $(LDFLAGS) # debug
	# cp rc /usr/local/bin

ff : ff.cpp
	$(CXX) $(CFLAGS) -O3 -o ff $<  -march=native -Wall

install : 
	cp gg /usr/local/bin
	cp rc /usr/local/bin

clean: 
	git clean -fdx
