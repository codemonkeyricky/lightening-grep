CXX 		:= g++

CFLAGS 		:= -O3 -Igrep
LDFLAGS		:= -Lgrep -lgrep -lpthread 
LDFLAGS_D   := -Lgrep -lgrep_d -lpthread 

all : main_g.o main.o
	make -C grep
	$(CXX) $(CFLAGS) main.o -std=gnu++11 -o gg $(LDFLAGS)           # release
	$(CXX) $(CFLAGS) main_g.o -std=gnu++11 -o gg_debug $(LDFLAGS_D) # debug

main_g.o : main.cpp 
	$(CXX) -Igrep -O0 -std=gnu++11 -c -o main_g.o main.cpp

main.o : main.cpp 
	$(CXX) -Igrep -O3 -std=gnu++11 -c -o main.o main.cpp

install : 
	cp gg /usr/local/bin
