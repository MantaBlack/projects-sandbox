CC = gcc
CXX = g++
RM = rm -f
CPPFLAGS = -g
LDFLAGS  = -g
LDLIBS   = -lsfml-graphics -lsfml-window -lsfml-system -lGL

# when make is called without arguments, it will use the first target (this one)
all: main

# check whether object files have changed and recompile the main
main: ParticleSystem.o main.o
	$(CXX) $(LDFLAGS) -o main main.o ParticleSystem.o $(LDLIBS)

# check whether source files have changed and recompile object
main.o: main.cpp ParticleSystem.hpp
	$(CXX) $(CPPFLAGS) -c main.cpp

# check whether source files have changed and recompile object
ParticleSystem.o: ParticleSystem.cpp ParticleSystem.hpp
	$(CXX) $(CPPFLAGS) -c ParticleSystem.cpp

clean:
	$(RM) *.o main