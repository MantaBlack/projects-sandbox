CC = gcc
CXX = g++
RM = rm -f
CPPFLAGS = -g
LDFLAGS  = -g
LDLIBS   = -lsfml-graphics -lsfml-window -lsfml-system

# when make is called without arguments, it will use the first target (this one)
all: app

# check whether object files have changed and recompile the app
app: MyEntity.o entity.o
	$(CXX) $(LDFLAGS) entity.o MyEntity.o $(LDLIBS)

# check whether source files have changed and recompile object
entity.o: entity.cpp MyEntity.hpp
	$(CXX) $(CPPFLAGS) -c entity.cpp

# check whether source files have changed and recompile object
MyEntity.o: MyEntity.cpp MyEntity.hpp
	$(CXX) $(CPPFLAGS) -c MyEntity.cpp

clean:
	$(RM) *.o *.exe

##################################################################################

# # check whether object files have changed and recompile the app
# app: MyEntity.o entity.o
# 	g++ -g -o app entity.o MyEntity.o -lsfml-graphics -lsfml-window -lsfml-system -lGL

# # check whether source files have changed and recompile object
# entity.o: entity.cpp MyEntity.hpp
# 	g++ -g -c entity.cpp

# # check whether source files have changed and recompile object
# MyEntity.o: MyEntity.cpp MyEntity.hpp
# 	g++ -g -c MyEntity.cpp