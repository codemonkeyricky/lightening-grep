.PHONY : clean 

# CXX 		:= g++
CPPFLAGS 	:= -march=native -std=gnu++11 -O3

SOURCES     = $(shell echo *.cpp)
HEADERS     = $(shell echo *.hpp)
OBJECTS     = $(SOURCES:.cpp=.o)

TARGET      = libgrep.a

DFLAGS 		:= -g -O0
RFLAGS 		:= -g -O3
LDFLAGS		:= -lpthread -shared

all : $(TARGET) 

clean : 
	rm -f $(OBJECTS) $(TARGET) 

$(TARGET) : $(OBJECTS)
	# $(CXX) $(CFLAGS) $(OBJECTS) -o $@ $(LDFLAGS)
	ar rvf $(TARGET) $(OBJECTS)
