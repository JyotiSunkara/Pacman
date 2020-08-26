#include <GL/gl.h>
#include <GL/glut.h>

#include <cmath>
#include <cstdlib>

#include "character.h"

void Pacman::setDest(Direction newDest) {
    this->initDest = Destination;
    this->Destination = newDest;
    rollingStatus = 0;
    inMotion = true;
}
