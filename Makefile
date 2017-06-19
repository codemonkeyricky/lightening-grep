CXX 		:= g++

CFLAGS 		:= -Igrep/api -std=gnu++11
LDFLAGS		:= -Lgrep -lgrep -lpthread 
LDFLAGS_D   := -Lgrep -lgrep_d -lpthread 

all : main_g.o main.o
	make -C grep
	$(CXX) $(CFLAGS) main.o -o gg $(LDFLAGS)           # release
	$(CXX) $(CFLAGS) main_g.o -o gg_debug $(LDFLAGS_D) # debug

main_g.o : main.cpp
	$(CXX) $(CFLAGS) -O0 -c -o main_g.o $<

main.o : main.cpp
	$(CXX) $(CFLAGS) -O3 -c -o main.o $<

install : 
	cp gg /usr/local/bin
