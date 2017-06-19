.PHONY : clean 

# CXX 		:= g++
CPPFLAGS 	:= -march=native -std=gnu++11 -Iapi -O3
CPPFLAGS_D 	:= -march=native -std=gnu++11 -Iapi -O0 -g

SOURCES     = $(shell echo src/*.cpp)
OBJECTS     = $(SOURCES:.cpp=.o)
OBJECTS_D   = $(SOURCES:.cpp=_g.o)

TARGET      = libgrep.a
TARGET_D    = libgrep_d.a

DFLAGS 		:= -g -O0
RFLAGS 		:= -g -O3
LDFLAGS		:= -lpthread -shared

all : $(TARGET) $(TARGET_D) 
# all : $(TARGET_D) 

clean : 
	rm -f $(OBJECTS) $(TARGET) 

$(OBJECTS) : %.o : %.cpp
	$(CXX) -c $(CPPFLAGS) $< -o $@

$(OBJECTS_D) : %_g.o : %.cpp
	$(CXX) -c $(CPPFLAGS_D) $< -o $@

$(TARGET) : $(OBJECTS)
	ar rvf $(TARGET) $(OBJECTS)

$(TARGET_D) : $(OBJECTS_D)
	ar rvf $(TARGET_D) $(OBJECTS_D)
