CXX 		:= g++

CFLAGS 		:= -O3 -Igrep
LDFLAGS		:= -Lgrep -lgrep -lpthread 

all : main.o
	$(CXX) $(CFLAGS) $^ -std=gnu++11 -o gg $(LDFLAGS) 

main.o : main.cpp 
	$(CXX) -Igrep -std=gnu++11 -c -o main.o main.cpp

install : 
	cp gg /usr/local/bin
