UNAME := $(shell uname -s)
CC = g++
OBJ = pacman.o character.o
LDFLAGS = -lGL -lGLU -lglut



pacman : $(OBJ)
	$(CC) -o pacman $(OBJ) $(LDFLAGS)

pacman.o : pacman.cpp cell.h 
	$(CC) -c -g pacman.cpp

character.o : character.cpp character.h cell.h
	$(CC) -c -g character.cpp

clean :
	rm $(OBJ)
