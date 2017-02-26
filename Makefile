.PHONY: all clean

CXX	:= c++
CXXFLAGS := -g -Wall
LDFLAGS := -lGdi32 -mwindows

OBJECTS	:= ENTER.o
OBJECTS := $(addprefix obj/, $(OBJECTS))

all : obj $(OBJECTS) 
	$(CXX) $(LDFLAGS) $(OBJECTS) -o run

obj :
	md obj

obj/%.o : src/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

obj/%.o : res/%.rc
	windres $< -o $@

clean :
	rd /S /Q obj