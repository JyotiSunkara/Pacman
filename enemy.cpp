#include <GL/gl.h>
#include <GL/glut.h>

#include <cmath>
#include <cstdlib>

#include "enemy.h"
#include "pathfinder.h"

void Ghost::setDest(Direction newDest) {
    this->initDest = Dest;
    this->Dest = newDest;
    rollingStatus = 0;
    ismoving = true;
}

Ghost::Ghost(int positionX, int positionY, int mazeWidth, int mazeHeight) {
    oldX = currentX = 20.0 + 10.0 * positionX;
    oldY = currentY = 20.0 + 10.0 * positionY;
    recursionStack = new int[mazeWidth * mazeHeight * 4];
    stackTop = -1;

    // Initializing status factor
    ismoving = false;
    walkStatus = 0;
    eyeStatus = 0;
    rollingStatus = 0;
    goalceremonyStatus = 0;
    degree_7 = sin(7 * atan(-1) / 180);
    
    lists();
    initDest = Dest = RIGHT;
}

void Ghost::lists(){
	glNewList(Face, GL_COMPILE);
		glBegin(GL_TRIANGLE_FAN);
			glEdgeFlag(GL_TRUE);
			glColor3f(1, 127.0/255.0, 80.0/255.0);
			glVertex2f(0, 0);

			glColor3f(1, 1, 0);
			for(int i = 0; i < 3; i++) {
                float sideAngle = 90 + (i * 2.0 * 180.0)/ 3;
                sideAngle = (sideAngle * M_PI)/ 180.0;
                float x = 3 * cos(sideAngle);
                float y = 3 * sin(sideAngle);
                glVertex2f(x, y);
		    }
			glVertex2f(0, 3);	
		glEnd();
	glEndList();

	glNewList(Mouth, GL_COMPILE);
		glBegin(GL_TRIANGLE_FAN);
			glEdgeFlag(GL_TRUE);

			glVertex2f(0, 0);

            float start = 3.0 * 3/4.0;
            for(int i = start - 5; i < start + 6; i++) {
                float sideAngle = 90 + (i * 2.0 * 180.0)/ 3;
                sideAngle = (sideAngle * M_PI)/ 180.0;
                float x = 3 * cos(sideAngle);
                float y = 3 * sin(sideAngle);
                glVertex2f(x, y);
            }
		glEnd();
	glEndList();

	glNewList(Eye, GL_COMPILE);
		int eyeLength = 5;
		int eyeSides = 60;
		int eyePos = 0;
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


void Ghost::Move() {
	double movingfactor = 28.5 * fabs(sin(degree_7 * walkStatus) - sin(degree_7 * (walkStatus - 1)));
	// length of leg * abs_value(sin(7_deg * walkStatus) - sin(7_deg * (walkStatus-1)))
	// movement of one frame of the animation

	if(rollingStatus == ROLL_FACT){
		switch(Dest) {
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
		currentX = oldX + ((Dest == RIGHT) ? 10 : -10);
		oldX = currentX;
		ismoving = false;
	} else if(abs(oldY - currentY) >= 10.0) {
		currentY = oldY + ((Dest == UP) ? 10 : -10);
		oldY = currentY;
		ismoving = false;
	}

	if(rollingStatus == ROLL_FACT) {	// if it is rotating then do not move the face, mouth and eye
		walkStatus++;
		if(walkStatus > 5)
			walkStatus = -4;
		eyeStatus++;
		if(eyeStatus >= 30)
			eyeStatus = 0;
	}
}

void Ghost::Draw() {
	glTranslatef(50, 70, 0);

	// draw body
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
	if(rollingStatus < ROLL_FACT){
		switch (initDest) {
		case LEFT:
			if(Dest == RIGHT)
				rotateAngle += 180.0/ROLL_FACT*rollingStatus;
			else if(Dest == UP)
				rotateAngle += -90.0/ROLL_FACT*rollingStatus;
			else if(Dest == DOWN)
				rotateAngle += 90.0/ROLL_FACT*rollingStatus;
			break;
		case RIGHT:
			if(Dest == LEFT)
				rotateAngle += 180.0/ROLL_FACT*rollingStatus;
			else if(Dest == UP)
				rotateAngle += 90.0/ROLL_FACT*rollingStatus;
			else if(Dest == DOWN)
				rotateAngle += -90.0/ROLL_FACT*rollingStatus;
			break;
		case UP:
			if(Dest == DOWN)
				rotateAngle += 180.0/ROLL_FACT*rollingStatus;
			else if(Dest == LEFT)
				rotateAngle += 90.0/ROLL_FACT*rollingStatus;
			else if(Dest == RIGHT)
				rotateAngle += -90.0/ROLL_FACT*rollingStatus;
			break;
		case DOWN:
			if(Dest == LEFT)
				rotateAngle += -90.0/ROLL_FACT*rollingStatus;
			else if(Dest == UP)
				rotateAngle += 180.0/ROLL_FACT*rollingStatus;
			else if(Dest == RIGHT)
				rotateAngle += 90.0/ROLL_FACT*rollingStatus;
			break;
		}
	}

	/* Draw Face */
	// glTranslatef(20,15,0);
	glRotatef(rotateAngle, 0, 0, 1);
	// glTranslatef(-20, -15, 0);
	glCallList(Face);

	glPushMatrix();

	/* Draw eye */
	glTranslatef(0, 20, 0);
	glColor3f(0, 0, 0);
	if(eyeStatus >= 24) glScalef(1, (30 - eyeStatus -1)/6.0, 1);
	glCallList(Eye);

	/* Draw Mouth */
	glPopMatrix();
	glPushMatrix();
	glColor3f(0.0, 0.0, 0.0);
	if(getGoal == true) glRotatef(5 * goalceremonyStatus, 0, 0, 1);
	// else glRotatef(abs(walkStatus * 5), 0, 0, 1);
		//if(walkStatus >= 0) glRotatef(5*walkStatus, 0, 0, 1);
		//else glRotatef(walkStatus * -5 , 0, 0, 1);
	glCallList(Mouth);
	glPopMatrix();
}

void Ghost::UpdateStatus() {
	if (ismoving) Move();
	if (rollingStatus < ROLL_FACT) {
		rollingStatus++;
		if (initDest == Dest) rollingStatus = ROLL_FACT;
		if (rollingStatus == ROLL_FACT) initDest = Dest;
	}
	if(getGoal == true) goalceremonyStatus++;
}
