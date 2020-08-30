UNAME := $(shell uname -s)
CC = g++
OBJ = pacman.o pathfinder.o enemy.o objects.o
LDFLAGS = -lGL -lGLU -lglut

# Mac OS
ifeq ($(UNAME), Darwin)
	LDFLAGS = -framework OpenGL -framework glut
endif

pacman : $(OBJ)
	$(CC) -o pacman $(OBJ) $(LDFLAGS)

pacman.o : pacman.cpp pacman.h pathfinder.h
	$(CC) -c -g pacman.cpp

pathfinder.o : pathfinder.cpp pathfinder.h pacman.h
	$(CC) -c -g pathfinder.cpp

enemy.o : enemy.cpp enemy.h pacman.h
	$(CC) -c -g enemy.cpp

objects.o : objects.cpp objects.h pacman.h
	$(CC) -c -g objects.cpp

clean :
	rm $(OBJ)
