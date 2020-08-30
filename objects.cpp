#include <GL/gl.h>
#include <GL/glut.h>

#include <cmath>
#include <cstdlib>

#include "objects.h"

Objects::Objects(int positionX, int positionY, int code) {
		posX = positionX;
		posY = positionY;
		type = code;

		active = true;
}

void Objects::lists() {
    glPushMatrix();
    glTranslatef(posX, posY, 0);

    switch(type) {
        case STAR:
            if(!active) return;
            glNewList(STAR, GL_COMPILE);
            glColor3f(0, 1, 1);
            glBegin(GL_TRIANGLES);
                glVertex3f(-1.0f, 0.0f, 0.0f);
                glVertex3f( 0.0, 5.0f, 0.0f);
                glVertex3f( 1.0f, 0.0f, 0.0f);
            glEnd();
            glRotatef(72, 0.0f, 0.0f, 1.0f);

            glBegin(GL_TRIANGLES);
                glVertex3f(-1.0f, 0.0f, 0.0f);
                glVertex3f( 0.0, 5.0f, 0.0f);
                glVertex3f( 1.0f, 0.0f, 0.0f);
            glEnd();

            glRotatef(72, 0.0f, 0.0f, 1.0f);

            glBegin(GL_TRIANGLES);
                glVertex3f(-1.0f, 0.0f, 0.0f);
                glVertex3f( 0.0, 5.0f, 0.0f);
                glVertex3f( 1.0f, 0.0f, 0.0f);
            glEnd();

            glRotatef(72, 0.0f, 0.0f, 1.0f);

            glBegin(GL_TRIANGLES);
                glVertex3f(-1.0f, 0.0f, 0.0f);
                glVertex3f( 0.0, 5.0f, 0.0f);
                glVertex3f( 1.0f, 0.0f, 0.0f);
            glEnd();
            
            glRotatef(72, 0.0f, 0.0f, 1.0f);

            glBegin(GL_TRIANGLES);
                glVertex3f(-1.0f, 0.0f, 0.0f);
                glVertex3f( 0.0, 5.0f, 0.0f);
                glVertex3f( 1.0f, 0.0f, 0.0f);
            glEnd();
            glEndList();
            break;

        case COIN: {
        if(!active) return;

        int sides = 60;
        int length = 20;
        glNewList(COIN, GL_COMPILE);
        glBegin(GL_TRIANGLE_FAN);
			glNormal3f(0, 0, 1.0);
			glColor3f(1, 1, 0);
			glVertex3f(0, 0, -1.0);
			glColor3f(1, 1, 0);
			for(int i = 0; i < sides; i++) {
                float sideAngle = 90 + (i * 2.0 * 180.0)/ sides;
                sideAngle = (sideAngle * M_PI)/ 180.0;
                float x = length * cos(sideAngle);
                float y = length * sin(sideAngle);
                glVertex3f(x, y, -1);
		    }
			glVertex3f(0, length, -1);	
			glEnd();
		glEndList();


            break;
        }
        case BOMB: {
        if(!active) return;

        int sides = 60;
        int length = 20;
        glNewList(BOMB, GL_COMPILE);
        glBegin(GL_TRIANGLE_FAN);
			glNormal3f(0, 0, 1.0);
			glColor3f(0.5, 0.5, 0.5);
			glVertex3f(0, 0, -1.0);
			glColor3f(0, 0, 0);
			for(int i = 0; i < sides; i++) {
                float sideAngle = 90 + (i * 2.0 * 180.0)/ sides;
                sideAngle = (sideAngle * M_PI)/ 180.0;
                float x = length * cos(sideAngle);
                float y = length * sin(sideAngle);
                glVertex3f(x, y, -1);
		    }
			glVertex3f(0, length, -1);	
		glEnd();

        glColor3f(1, 0, 0);
		glPushMatrix();
		glTranslatef(-0.3, length, 0);
        glBegin(GL_QUADS);
			glVertex3f(1, 0, -1.0);
			glVertex3f(0, 0, -1.0);
			glVertex3f(0, 0.5, -1.0);
			glVertex3f(1, 0.5, -1.0);
		glPopMatrix();
		glEndList();
        break;
        }
    }

    glPopMatrix();

}