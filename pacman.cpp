#include <GL/gl.h>
#include <GL/glut.h>


#include <iostream>
#include <ctime>
#include <sys/time.h>
#include <cstring>
using namespace std;

#include "cell.h"

static Cell *cell;
static int width, height;	// Maze size
static double R = 25.0/255.0, G = 25.0/255.0, B = 166.0/255.0;		// Background
/* Making maze, Finding path, End */
static int state = 0;		        // Current state
static int timefactor;		        // Controls duration
static bool work = false;			        // Making maze/ Pause
static int startingX, startingY;	// Starting position
static int goalX, goalY;	        // Goal position
static int *chosen;			        // Pointer to the array of connected cells


static int ViewZoomFactor;  // Zoom control
static int viewLeft, viewRight, viewBottom, viewUp; // View points
static int changeX, changeY;


void drawMaze();
void generateMaze();
void eraseWall( int x, int y, int wall);
void reshape(int width, int height);
void display();
void stateMachine();
void keyFunc(unsigned char key, int x, int y);

static int getTime() {
	static time_t initSec = 0;
	timeval tval;
	gettimeofday(&tval, NULL);
	if (initSec == 0) {
		initSec = tval.tv_sec;
	}

	return (tval.tv_sec - initSec) * 1000 + tval.tv_usec / 1000;
}

static inline Cell & cellXY(int x, int y) {
	return cell[y * width + x];
}

int main(int argc, char ** argv) {
    if (argc > 1) {
		if (strcmp(argv[1], "--help") == 0) {
			cout << "usage: " << argv[0] << " [--auto]" << endl;
			return 0;
		}
	}

	srand((unsigned)time(NULL));

	// Input the size of the maze 
	while(true) {
		cout << "Please input the width of maze (5 - 50)" << endl;
		cin >> width;
		if(width > 50 || width < 5)	cout << "Out of range!" << endl;
		else break;
	}

	while(true) {
		cout << "Please input the height of maze (5 - 50)" << endl;
		cin >> height;
		if(height > 50 || height < 5)	cout << "Out of range!" << endl;
		else break;
	}

    // Manual guide
	cout << endl;
	cout << "Space bar : Generate Maze" << endl;
	cout << "W A S D   : Scroll the maze" << endl;
	cout << "Arrow keys : Move the character" << endl;
    cout << "Page Up   : Zoom in" << endl;
	cout << "Page Down : Zoom out" << endl;
	cout << "Home key  : Over view the maze" << endl;
	cout << "End key   : Closed view the maze" << endl;
    cout << "Del key   : Reset zoom and scroll" << endl;

	cell = new Cell[width * height];

    // Background colour
	// R = (rand()%256) / 255.0;
	// G = (rand()%256) / 255.0;
	// B = (rand()%256) / 255.0;

	timefactor = INIT_TIMEFACTOR;

    glutInit(&argc, argv);
	glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGB );
	glutInitWindowSize ( 500, 500 );
	glutInitWindowPosition (100, 100);
	glutCreateWindow ("Pacman");

    // Callbacks
	glutReshapeFunc(reshape);
	glutDisplayFunc(display);
    glutIdleFunc(stateMachine);
	// glutSpecialFunc(specialKeyFunc);
	glutKeyboardFunc(keyFunc);

	glutMainLoop();

    return 0;
}

void reshape(int w, int h) { 
	int size = (width > height)? width : height;
	double move = (width > height)? (width - height) / 2.0 : (height - width) / 2.0;

	glViewport(0, 0, w, h);
	glMatrixMode (GL_PROJECTION);
	glLoadIdentity();

	if( state == 0 ) 
    {
		if( width == size ){
			viewLeft = 0.0;
			viewRight = 20 + size * 10;
			viewBottom = 0.0 - move * 10;
			viewUp = size * 10 + 20 - move * 10;
		} else {
			viewLeft = 0.0 - move * 10;
			viewRight = 20 + size * 10 - move * 10;
			viewBottom = 0.0;
			viewUp = size * 10 + 20;
		}
	}

	gluOrtho2D(viewLeft, viewRight, viewBottom, viewUp);

	glMatrixMode (GL_MODELVIEW);
	glLoadIdentity();
}

void display() {
    glClearColor(R, G, B, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);

    glColor3f(1 - R, 1 - G, 1 - B); // Negative of background
    glLoadIdentity();
    glLineWidth(0.5);
    glBegin(GL_LINES);
    double i;
    for (i = 1.0; i < width + 2.0; i++) {
        glVertex2f(i * 10.0, 10.0);
        glVertex2f(i * 10.0, height * 10.0 + 10.0);
    }

    for (i = 1.0; i < height + 2.0; i++) {
        glVertex2f( 10.0 , i * 10.0 );
		glVertex2f( width * 10.0 + 10.0 , i * 10.0 );
    }
    glEnd();

    drawMaze();
    glutSwapBuffers();
    
}

void drawMaze() {

	int i;
	int x, y;

	for( i = 0 ; i < width * height ; i++ ){
		x = i % width;
		y = i / width;

		if(cell[i].road[right]) {
            eraseWall(x, y, right);
        }

		if(cell[i].road[up]) {
            eraseWall(x, y, up);
        }

		if(cell[i].road[down]) {
            eraseWall(x, y, down);
        }

		if(cell[i].road[left]) {
            eraseWall(x, y, left);
        }
	}
}

void eraseWall(int x, int y, int wall) {

	glColor3f(R, G, B);
	glBegin(GL_LINES);

	switch(wall) {

	case up:
		glVertex2f((x + 1) * 10.0 + 0.1, (y + 2) * 10.0);
		glVertex2f((x + 2) * 10.0 - 0.1, (y + 2) * 10.0);
		break;

	case down:
		glVertex2f((x + 1) * 10.0 + 0.1, (y + 1) * 10.0);
		glVertex2f((x + 2) * 10.0 - 0.1, (y + 1) * 10.0);
		break;

	case right:
        // cout << x << " " << y << endl;
		glVertex2f((x + 2) * 10.0, (y + 1) * 10.0);
		glVertex2f((x + 2) * 10.0, (y + 2) * 10.0);
		break;

	case left:
		glVertex2f((x + 1) * 10.0, (y + 1) * 10.0);
		glVertex2f((x + 1) * 10.0, (y + 2) * 10.0);
		break;
	}

	glEnd();
}

// generate the maze
void generateMaze() {

    int x, y;   // Position of the current cell
    int dest;   // Direction of to be connected cell
    static int length = 0; // Number of visited/connected cells
    int temp;
    int currTime;
    static int oldTime = 0;

    if(length == width * height) {

        state = 1; // Maze building done
        for (int i = 0; i < width * height; i++) {
            cell[i].isOpen = false;
        }        

        return;
    }

    currTime = getTime();
    if (currTime - oldTime > timefactor) {
        oldTime = currTime;
    } else {
        return;
    }

    // Make start and end
    if (length == 0) {
        dest = rand() % 2 + 1;

        if (dest = down) {
            // Start 
            startingX = x = rand() % width;
            startingY = y = height - 1;
            cellXY(x, y).road[up] = true;

            // Goal
            goalX = x = rand() % width;
            goalY = y = 0;
            cellXY(x, y).road[down] = true;
            
        } else {
            // Start
			startingX = x = width - 1;
			startingY = y = rand() % height;
			cellXY(x, y).road[right] = true;
			
            // Goal
			goalX = x = 0;
			goalY = y = rand() % height;
			cellXY(x, y).road[left] = true;
        }

        chosen = new int [height * width];

        x = rand() % width;
        y = rand() % height;
        cellXY(x, y).isOpen = true;
        chosen[0] = width * y + x; // First cell stored

        length = length + 1;
        
    }

    bool cellOpen = false;
    while(!cellOpen) {
        temp = chosen[rand() % length]; // Choose a visited cell
        x = temp % width;
        y = temp / width;

        dest = rand() % 4;
        /* Choose a direction to connect a nearby cell
		 If the cell on the direction has visited or is out of range
		 Then select a cell and a directon again
		 Else, connect two cells and store the new cell (the cell on the direction)
		 in the array chosen(visited cells) */

        switch (dest) {

        case up:
            if( y == height-1 || cellXY(x, y + 1).isOpen == true)
				continue;

			cellXY(x, y + 1).isOpen = true;

			cellXY(x, y + 1).road[ down ] = true;
			cellXY(x, y).road[ up ] = true;

			chosen[length] = width*( y + 1 ) + x;
			length = length + 1;
			cellOpen = true;
            break;
        
        case down:
			if( y == 0 || cellXY(x, y - 1).isOpen == true )
				continue;

			cellXY(x, y - 1).isOpen = true;

			cellXY(x, y - 1).road[ up ] = true;
			cellXY(x, y).road[ down ] = true;

			chosen[length] = width*(y - 1) + x;
			length = length  + 1;
			cellOpen = true;
			break;

		case right:
			if( x == width-1 || cellXY(x + 1, y).isOpen == true )
				continue;

			cellXY(x + 1,  y).isOpen = true;

			cellXY(x + 1,  y).road[ left ] = true;
			cellXY(x,  y).road[ right ] = true;

			chosen[length] = width * y + x + 1;
			length = length  + 1;
			cellOpen = true;
			break;

		case left:
			if( x == 0 || cellXY(x - 1,  y).isOpen == true )
				continue;

			cellXY(x - 1,  y).isOpen = true;

			cellXY(x - 1,  y).road[ right ] = true;
			cellXY(x,  y).road[ left ] = true;

			chosen[length] = width * y + x - 1;
			length = length  + 1;
			cellOpen = true;
			break;
        
        default:
            break;
        }
    }
}
    
void stateMachine() {
	if(work == false) return;

	switch (state) {
	case 0:
		generateMaze();
		break;
	case 1:
        // Auto Mode
		break;
	case 2:
		// Win mode
		break;
	case 3:
		// Exit
        exit(0);
		break;
	}

	display();
}
    
void keyFunc( unsigned char key, int x, int y ){
	switch (key) {
	case ' ':
		work = !work;
		break;
    }
}
