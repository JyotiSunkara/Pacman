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
    if(active == false) return;
    if(type == STAR) {
            glPushMatrix();

           
            glTranslatef(posX, posY, 0);

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
            glPopMatrix();

          
    } else if (type == COIN) {
        int sides = 60;
        int length = 20;
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
        
    } else if(type == BOMB) {
       
        glPushMatrix();

        glTranslatef(posX, posY, 0);
        int bombSides = 60;
        int bombLength = 4;
        glBegin(GL_TRIANGLE_FAN);
			glNormal3f(0, 0, 1.0);
			glColor3f(0.5, 0.5, 0.5);
			glVertex3f(0, 0, 0);
			glColor3f(0, 0, 0);
			for(int i = 0; i < bombSides; i++) {
                float sideAngle = 90 + (i * 2.0 * 180.0)/ bombSides;
                sideAngle = (sideAngle * M_PI)/ 180.0;
                float x = bombLength * cos(sideAngle);
                float y = bombLength * sin(sideAngle);
                glVertex3f(x, y, 0);
		    }
			glVertex3f(0, bombLength, 0);	
		glEnd();

        glColor3f(1, 0, 0);
		glTranslatef(-1, bombLength, 0);
        glBegin(GL_QUADS);
			glVertex3f(3, 0, -1.0);
			glVertex3f(0, 0, -1.0);
			glVertex3f(0, 1, -1.0);
			glVertex3f(3, 1, -1.0);
        glEnd();
        glPopMatrix();
        
    }

   

}