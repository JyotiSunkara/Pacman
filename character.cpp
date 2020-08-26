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

Pacman::Pacman(int xPosition, int yPosition, int mazeWidth, int mazeHeight) {
    oldX = currentX = 20.0 + 10.0 * xPosition;
    oldY = currentY = 20.0 + 10.0 * yPosition;

    recursionStack = new int[mazeWidth * mazeHeight * 4]; // Worst case size
    stackTop = -1;

    // Status initialization
    inMotion = false;
    walkStatus = 0;
    eyeStatus = 0;
    rollingStatus = 0;
    goalceremonyStatus = 0;
    degree_7 = sin(7 * atan(-1) / 180);

    lists();
    initDest = Destination = RIGHT;
}


void Pacman::lists() {

    int faceLength = 5;
    int faceSides = 28;
    glNewList(Face, GL_COMPILE);
		glBegin(GL_TRIANGLE_FAN);
			glEdgeFlag(GL_TRUE);
			glVertex2f(0, 0);
			for(int i = 0; i < faceSides; i++) {
                float sideAngle = 90 + (i * 2.0 * 180.0)/ faceSides;
                sideAngle = (sideAngle * M_PI)/ 180.0;
                float x = faceLength * cos(sideAngle);
                float y = faceLength * sin(sideAngle);
                glVertex2f(x, y);
		    }
			glVertex2f(0, faceLength);	
		glEnd();
    glEndList();

    glNewList(Mouth, GL_COMPILE);
		glBegin(GL_TRIANGLE_FAN);
			glEdgeFlag(GL_TRUE);

			glVertex2f(0, 0);

            float start = 3.0 * faceSides/4.0;
            for(int i = start - 2; i < start + 3; i++) {
                float sideAngle = 90 + (i * 2.0 * 180.0)/ faceSides;
                sideAngle = (sideAngle * M_PI)/ 180.0;
                float x = faceLength * cos(sideAngle);
                float y = faceLength * sin(sideAngle);
                glVertex2f(x, y);
            }
		glEnd();
    glEndList();

    int eyeLength = 1;
    int eyeSides = 28;
    int eyePos = 0;
    glNewList(Eye, GL_COMPILE);
		glBegin(GL_POLYGON);
			glEdgeFlag(GL_TRUE);
			glVertex2f(eyePos, eyePos);
			for(int i = 0; i < eyeSides; i++) {
                float sideAngle = 90 + (i * 2.0 * 180.0)/ eyeSides;
                sideAngle = (sideAngle * M_PI)/ 180.0;
                float x = eyePos + eyeLength * cos(sideAngle);
                float y = eyePos + eyeLength * sin(sideAngle);
                glVertex2f(x, y);
		    }	
			glVertex2f(0, eyeLength);	
		glEnd();
    glEndList();
}

void Pacman::Move() {
	double movingfactor = 28.5 * fabs(sin(degree_7 * walkStatus) - sin(degree_7 * (walkStatus - 1)));
    if (rollingStatus == ROLL_FACTOR) {
        switch(Destination) {
		case UP:
			currentY += movingfactor;
			break;
		case DOWN:
			currentY -= movingfactor;
			break;
		case LEFT:
			currentX -= movingfactor;
			break;
		case RIGHT:
			currentX += movingfactor;
			break;
        }
    }

   if(abs(oldX - currentX) >= 10.0) {
		currentX = oldX + ((Destination == RIGHT) ? 10 : -10);
		oldX = currentX;
		inMotion = false;
	}
	else if(abs(oldY - currentY) >= 10.0) {
		currentY = oldY + ((Destination == UP) ? 10 : -10);
		oldY = currentY;
		inMotion = false;
	}

	if(rollingStatus == ROLL_FACTOR) {	// if it is rotating then do not move the leg, arm and eye
		walkStatus++;
		if( walkStatus > 5 )
			walkStatus = -4;
	}
}

void Pacman::Draw() {
	glTranslatef(30, 50, 0);

	// Draw face
	double rotateAngle = 0;
	switch (initDest) {
	case LEFT:
		rotateAngle = 180.0;
		break;
	case UP:
		rotateAngle = 90.0;
		break;
	case DOWN:
		rotateAngle = -90.0;
		break;
	}

	if( rollingStatus < ROLL_FACTOR   ){
		switch (initDest) {
		case LEFT:
			if(Destination == RIGHT )
				rotateAngle += 180.0 / ROLL_FACTOR * rollingStatus;
			else if(Destination == UP )
				rotateAngle += -90.0 / ROLL_FACTOR * rollingStatus;
			else if(Destination == DOWN )
				rotateAngle += 90.0 / ROLL_FACTOR * rollingStatus;
			break;
		case RIGHT:
			if(Destination == LEFT )
				rotateAngle += 180.0 / ROLL_FACTOR * rollingStatus;
			else if(Destination == UP )
				rotateAngle += 90.0 / ROLL_FACTOR * rollingStatus;
			else if(Destination == DOWN )
				rotateAngle += -90.0 / ROLL_FACTOR * rollingStatus;
			break;
		case UP:
			if(Destination == DOWN )
				rotateAngle += 180.0 / ROLL_FACTOR * rollingStatus;
			else if(Destination == LEFT )
				rotateAngle += 90.0 / ROLL_FACTOR * rollingStatus;
			else if(Destination == RIGHT )
				rotateAngle += -90.0 / ROLL_FACTOR * rollingStatus;
			break;
		case DOWN:
			if(Destination == LEFT )
				rotateAngle += -90.0 / ROLL_FACTOR * rollingStatus;
			else if(Destination == UP )
				rotateAngle += 180.0 / ROLL_FACTOR * rollingStatus;
			else if(Destination == RIGHT )
				rotateAngle += 90.0 / ROLL_FACTOR * rollingStatus;
			break;
		}
	}

	glTranslatef(20, 15, 0);
	glRotatef(rotateAngle, 0, 0, 1);
	glTranslatef(-20, -15, 0);
	glColor3f(1.0, 1.0, 0);
	glCallList(Face);

	glPushMatrix();

	// draw eye 
	glTranslatef( 23, 30, 0 );
	glCallList(Eye);

	// Draw mouth
	glPopMatrix();
	glPushMatrix();
	glColor3f(0.0, 0.0, 0.0);
	glTranslatef(35, 20, 0);
	glRotatef(abs(walkStatus * 5), 0, 0, 1);
	glCallList(Mouth);
	glPopMatrix();
}

void Pacman::UpdateStatus() {
	if (inMotion) Move();
	if (rollingStatus < ROLL_FACTOR) {
		rollingStatus++;
		if (initDest == Destination) rollingStatus = ROLL_FACTOR;
		if (rollingStatus == ROLL_FACTOR) initDest = Destination;
	}

	if(getGoal == true) goalceremonyStatus++;
}