#include <GL/gl.h>
#include <GL/glut.h>

#include <iostream>
#include <ctime>
#include <sys/time.h>
#include <cstring>

#include "pacman.h"
#include "pathfinder.h"

void display();
void reviewpoint();
void drawMaze();

static Cell *cell;
static int width, height;				// Maze size
static int startingX, startingY;		// Start
static int goalX, goalY;				// Goal
static double R = 33.0/255.0, G =  33.0/255.0, B = 222.0/255.0;		// Background

static int *chosen;						// Connected cells array
static bool work;						/* Maze generate/ Pause */
static int state = 0;					/* Making maze/ Finding path/ End */
static PathFinder* pacmanPath = NULL;	// Path finder object
static bool overview = false;
static bool autoMode = false;
static int userInputLastDirection = -1;

static int viewLeft, viewRight, viewBottom, viewUp;	// View points
static int ViewZoomFactor;
static int changeX, changeY;
static int timefactor;		// controls duration

static int getTime() {
	static time_t initSec = 0;
	timeval tv;
	gettimeofday(&tv, NULL);
	if (initSec == 0) {
		initSec = tv.tv_sec;
	}
	return (tv.tv_sec - initSec) * 1000 + tv.tv_usec / 1000;
}

static inline Cell & cellXY(int x, int y) {
	return cell[y * width + x];
}



// Generate the maze
void generateMaze() {

	int x, y;	// Position of the current cell
	int dest;	// Direction of to be connected cell
	static int length = 0;	// size of the array chosen(store the visited(connected) cells)
	int tmp;
	int currTime;
	static int oldTime = 0;

	if(length == width * height) {

		state = 1; // Maze generation done
		for(int i = 0; i < width*height; i++)
			cell[i].isOpen = false;
		return;
	}

	// Time
	currTime = getTime();
	if(currTime - oldTime > timefactor*1)
		oldTime = currTime;
	else return;


	if(length == 0) {
		// make start point and goal
		dest = rand()%2 + 1;
		if(dest == down) {
			// starting point
			startingX = x = rand()%width;
			startingY = y = height - 1;
			cellXY(x, y).road[up] = true;
			// goal
			goalX = x = rand()%width;
			goalY = y = 0;
			cellXY(x, y).road[down] = true;
		} else {
			// starting point
			startingX = x = width - 1;
			startingY = y = rand()%height;
			cellXY(x, y).road[right] = true;
			// goal
			goalX = x = 0;
			goalY = y = rand()%height;
			cellXY(x, y).road[left] = true;
		}

		chosen = new int [height * width];

		// choose the first cell randomly
		x = rand()%width;
		y = rand()%height;
		cellXY(x, y).isOpen = true;
		chosen[0] = width*y + x;	// store the first visited cell

		length = length + 1;
	}

	bool cellOpen = false;
	while (!cellOpen) {
		tmp = chosen[ rand()%length ];	// randomly choose a cell which is visited
		x = tmp % width;
		y = tmp / width;

		dest = rand()%4;	 
		/* Choose a direction to connect a nearby cell
		 If the cell on the direction has visited or is out of range
		 Then select a cell and a directon again
		 Else, connect two cells and store the new cell (the cell on the direction)
		 in the array chosen(visited cells) */
		switch(dest) {

		case up:
			if(y == height-1 || cellXY(x, y + 1).isOpen == true)
				continue;

			cellXY(x, y + 1).isOpen = true;

			cellXY(x, y + 1).road[ down ] = true;
			cellXY(x, y).road[ up ] = true;

			chosen[length] = width*(y + 1) + x;
			length++;
			cellOpen = true;
			break;

		case down:
			if(y == 0 || cellXY(x, y - 1).isOpen == true)
				continue;

			cellXY(x, y - 1).isOpen = true;

			cellXY(x, y - 1).road[ up ] = true;
			cellXY(x, y).road[ down ] = true;

			chosen[length] = width*(y - 1) + x;
			length++;
			cellOpen = true;
			break;

		case right:
			if(x == width-1 || cellXY(x + 1, y).isOpen == true)
				continue;

			cellXY(x + 1,  y).isOpen = true;

			cellXY(x + 1,  y).road[ left ] = true;
			cellXY(x,  y).road[ right ] = true;

			chosen[length] = width * y + x + 1;
			length++;
			cellOpen = true;
			break;

		case left:
			if(x == 0 || cellXY(x - 1,  y).isOpen == true)
				continue;

			cellXY(x - 1,  y).isOpen = true;

			cellXY(x - 1,  y).road[ right ] = true;
			cellXY(x,  y).road[ left ] = true;

			chosen[length] = width * y + x - 1;
			length++;
			cellOpen = true;
			break;
		}
	}
}

void reshape(int w, int h) {
	int size = (width > height)? width : height;
	double move = (width > height)? (width-height)/2.0 : (height-width)/2.0;

	glViewport(0, 0, w, h);
	glMatrixMode (GL_PROJECTION);
	glLoadIdentity ();

	if(state == 0) {
		if(width == size) {
			viewLeft = 0.0;
			viewRight = 20.0 + size * 10;
			viewBottom = 0.0 - move * 10;
			viewUp = size * 10.0 + 20.0 - move * 10;
		}
		else{
			viewLeft = 0.0 - move*10;
			viewRight = 20 + size*10 - move*10;
			viewBottom = 0.0;
			viewUp = size * 10 + 20;
		}
	}

	gluOrtho2D(viewLeft, viewRight, viewBottom, viewUp);

	glMatrixMode (GL_MODELVIEW);
	glLoadIdentity();
}



// the space bar toggles making maze or pause
void keyFunc(unsigned char key, int x, int y) {
	switch (key) {
	case ' ':
		work = !work;
		break;
	case '-':
		if (timefactor < 50) timefactor += 5;	// work faster
		break;
	case '+':
		timefactor -= 5;	// work slower
		if(timefactor < 0) timefactor = 0;
		break;
	case 0x7f:	// initialzing all factors, (delete key)
		overview = false;
		ViewZoomFactor = 20;
		changeX = 0;
		changeY = 0;
		timefactor = INIT_TIMEFACTOR;
		reviewpoint();
		display();
		break;
	case 'w':
		if (changeY < height * 5) changeY += 5;	// scroll the maze up
		break;
	case 'a':
		if (changeX > width * -5) changeX -= 5;	// scroll the maze left
		break;
	case 's':
		if (changeY > height * -5) changeY -= 5;	// scroll the maze down
		break;
	case 'd':
		if (changeX < width * 5) changeX += 5;	// scroll the maze right
		break;
	}
}

void pathFinding() {
	static int oldTime;
	int currTime = getTime();
	static PathFinder finder(::startingX, ::startingY, ::width, ::height);
	static int x = ::startingX;
	static int y = ::startingY;

	if(currTime - oldTime > timefactor)
		oldTime = currTime;
	else return;

	if (pacmanPath == NULL) {
		pacmanPath = &finder;	// To use in other functions
		finder.SetBodyColor(1.0, 1.0, 0.0);
	}

	finder.UpdateStatus();
	if(finder.isMoving()) {
		return;
	}

	if(x == ::goalX && y == ::goalY) {	// if get the goal
		::state++;
		pacmanPath->setGetgoal();
		return;
	}

	if (autoMode) {
		cellXY(x, y).isOpen = true;	// visit starting position
		if(finder.isstackEmpty() || finder.getStacktop() < NOT_ANY_DIRECTION) {
			if(cellXY(x,  y).road[down] == true && y > 0 && cellXY(x, y-1).isOpen == false) {
				finder.setDest(PathFinder::DOWN);
				y--;
				cellXY(x, y).isOpen = true;
				finder.stackPush(down);
				return;
			}
			else finder.stackPush(NOTDOWN);
		}
		if(finder.getStacktop() == NOTDOWN) {
			if(cellXY(x, y).road[left] == true && x > 0 && cellXY(x-1, y).isOpen == false) {
				finder.setDest(PathFinder::LEFT);
				x--;
				cellXY(x, y).isOpen = true;
				finder.stackPush(left);
				return;
			}
			else finder.stackPush(NOTLEFT);
		}
		if(finder.getStacktop() == NOTLEFT) {
			if(cellXY(x, y).road[right] == true && x < ::width-1 && cellXY(x+1, y).isOpen == false) {
				finder.setDest(PathFinder::RIGHT);
				x++;
				cellXY(x, y).isOpen = true;
				finder.stackPush(right);
				return;
			}
			else finder.stackPush(NOTRIGHT);
		}
		if(finder.getStacktop() == NOTRIGHT) {
			if(cellXY(x, y).road[up] == true && y < ::height-1 && cellXY(x, y+1).isOpen == false) {
				finder.setDest(PathFinder::UP);
				y++;
				cellXY(x, y).isOpen = true;
				finder.stackPush(up);
				return;
			}
			else finder.stackPush(NOTUP);
		}
	
		int tempDest;
		finder.stackPop();
		finder.stackPop();
		finder.stackPop();
		finder.stackPop();
		tempDest = finder.stackPop();
		if(tempDest == down) y++, finder.setDest(PathFinder::UP);
		else if(tempDest == left) x++, finder.setDest(PathFinder::RIGHT);
		else if(tempDest == right) x--, finder.setDest(PathFinder::LEFT);
		else if(tempDest == up) y--, finder.setDest(PathFinder::DOWN);
	}
	else {
		if (userInputLastDirection > -1) {
			switch (userInputLastDirection) {
			case up:
				if(cellXY(x, y).road[up] == true && y < ::height-1 && cellXY(x, y+1).isOpen == false) {
					finder.setDest(PathFinder::UP);
					y++;
				}
				break;
			case down:
				if(cellXY(x,  y).road[down] == true && y > 0 && cellXY(x, y-1).isOpen == false) {
					finder.setDest(PathFinder::DOWN);
					y--;
				}
				break;
			case right:
				if(cellXY(x, y).road[right] == true && x < ::width-1 && cellXY(x+1, y).isOpen == false) {
					finder.setDest(PathFinder::RIGHT);
					x++;
				}
				break;
			case left:
				if(cellXY(x, y).road[left] == true && x > 0 && cellXY(x-1, y).isOpen == false) {
					finder.setDest(PathFinder::LEFT);
					x--;
				}
				break;
			}
			userInputLastDirection = -1;
		}
	}
}

void goalceremony() {
	static int count = 0;
	static int oldTime = 0;
	int currTime = getTime();

	if(currTime - oldTime < timefactor * 0.2) return;

	oldTime = currTime;
	count++;
	if(count > 300) state++;
	pacmanPath->UpdateStatus();
}

void specialKeyFunc(int key, int x, int y) {
	switch (key) {
	case GLUT_KEY_RIGHT:
		userInputLastDirection = right;
		break;
	case GLUT_KEY_LEFT:
		userInputLastDirection = left;
		break;
	case GLUT_KEY_DOWN:
		userInputLastDirection = down;
		break;
	case GLUT_KEY_UP:
		userInputLastDirection = up;
		break;
	case GLUT_KEY_PAGE_DOWN:
		if (ViewZoomFactor < ((width>height)? width * 5 : height * 5)) ViewZoomFactor += 5;	// zoom in
		break;
	case GLUT_KEY_PAGE_UP:
		if (ViewZoomFactor > 0) ViewZoomFactor -= 5;	// zoom out
		break;
	case GLUT_KEY_INSERT:	// initializing viewing
		ViewZoomFactor = 20;
		changeX = 0;
		changeY = 0;
		break;
	case GLUT_KEY_HOME:
		overview = true;	// set the whole maze can be seen
		break;
	case GLUT_KEY_END:
		overview = false;	// zoomed maze
		break;
	}

	reviewpoint();
	display();
}

void reviewpoint() {
	if(pacmanPath == NULL) return;	// if finder does not exist

	if(overview == true) {
		double move = (width-height)/2.0;
		if(width >= height) {
			viewLeft = 0.0;
			viewRight = 20 + width*10;
			viewBottom = 0.0 - move*10;
			viewUp = width * 10 + 20 - move*10;
		}
		else{
			viewLeft = 0.0 + move*10;
			viewRight = 20 + height*10 + move*10;
			viewBottom = 0.0;
			viewUp = height * 10 + 20;
		}
	}

	else {
		viewLeft = pacmanPath->CurrentX() - (ViewZoomFactor+10)+ changeX;
		viewRight = pacmanPath->CurrentX() + ViewZoomFactor+ changeX;
		viewBottom = pacmanPath->CurrentY() - (ViewZoomFactor+10)+ changeY;
		viewUp = pacmanPath->CurrentY() + ViewZoomFactor+ changeY;
	}
	glMatrixMode (GL_PROJECTION);
	glLoadIdentity ();

	gluOrtho2D(viewLeft, viewRight, viewBottom, viewUp);

	glMatrixMode (GL_MODELVIEW);
	glLoadIdentity();
}

void idle() {
	if(work == false) return;
	switch (state) {
	case 0:
		generateMaze();
		break;
	case 1:
		pathFinding();
		reviewpoint();
		break;
	case 2:
		// goalceremony();
		break;
	case 3:
		exit(0);
		break;
	}
	display();
}

int main(int argc, char ** argv) {
	using namespace std;

	if (argc > 1) {
		if (strcmp(argv[1], "--help") == 0) {
			cout << "usage: " << argv[0] << " [--auto | --custom | -levels]" << endl;
			return 0;
		}
	}

	srand((unsigned)time(NULL));


/*
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
*/

	width = 10;
	height = 10;
	/* help message */
	cout << endl;
	cout << "Space bar  : Generate Maze" << endl;
	cout << "W A S D    : Scroll the maze" << endl;
	cout << "Arrow keys : Move the character" << endl;
    cout << "Page up    : Zoom in" << endl;
	cout << "Page down  : Zoom out" << endl;
	cout << "Home key   : Over view the maze" << endl;
	cout << "End key    : Closed view the maze" << endl;
    cout << "Del key    : Reset zoom and scroll" << endl;

	cell = new Cell[width * height];

	timefactor = INIT_TIMEFACTOR;
	changeX = 0;
	changeY = 0;
	ViewZoomFactor = 20;

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutInitWindowSize (500, 500);
	glutInitWindowPosition (100, 100);
	glutCreateWindow ("Pacman");
	glutReshapeFunc(reshape);


	// Lighting set up
	glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE);
	GLfloat ambientColor[] = {1.0f, 1.0f, 1.0f, 1.0f};
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambientColor);

	glEnable(GL_COLOR_MATERIAL);
	glEnable(GL_LIGHTING);
	

	glutDisplayFunc(display);
	glutIdleFunc(idle);
	glutSpecialFunc(specialKeyFunc);
	glutKeyboardFunc(keyFunc);

	glutMainLoop();

	return 0;
}

void display() {
	double x;

	glClearColor(0, 0, 0, 0.0);
	glClear(GL_COLOR_BUFFER_BIT);


	glColor3f(R, G, B);
	// Draw square with many little squares
	glBegin(GL_QUADS);
		glNormal3f(0.0, 0.0, 1.0);
		const GLfloat kqDelta = 1;
		for (int i = -100; i < 200; ++i) {
			for (int j = -100; j < 200; ++j) {
				glVertex3f(j * kqDelta, i * kqDelta, -0.2);
				glVertex3f((j + 1) * kqDelta, i * kqDelta, -0.2);
				glVertex3f((j + 1) * kqDelta, (i + 1) * kqDelta, -0.2);
				glVertex3f(j * kqDelta, (i + 1) * kqDelta, -0.2);
			}
		}
	glEnd();
	glFlush();

	glColor3f(1 - R, 1 - G, 1 - B);	// The color is the negative of background color

	// draw default(unmaden) maze
	glLoadIdentity();
    glLineWidth(2.0);

	// glBegin(GL_QUADS);

	// for(x = 1 ; x < width+2 ; x++) {
	// 	// Top
	// 	glVertex3f(x * 10, 10.0, 0);
	// 	glVertex3f(x * 10, height * 10 + 10.0, 0);
	// 	glVertex3f(x * 10 + 0.5, height * 10 + 10.0, 0);
	// 	glVertex3f(x * 10 + 0.5, 10.0, 0);

	// 	// Right
	// 	glVertex3f(x * 10 + 0.5, 10.0, 0);
	// 	glVertex3f(x * 10 + 0.5, height * 10 + 10.0, 0);
	// 	glVertex3f(x * 10 + 0.5, height * 10 + 10.0, -0.2);
	// 	glVertex3f(x * 10 + 0.5, 10.0, -0.2);

	// 	//Left
	// 	glVertex3f(x * 10, 10.0, 0);
	// 	glVertex3f(x * 10, height * 10 + 10.0, 0);
	// 	glVertex3f(x * 10, height * 10 + 10.0, -0.2);
	// 	glVertex3f(x * 10, 10.0, -0.2);
	// }

	// glEnd();

	// glBegin(GL_QUADS);

	// for(x = 1 ; x < height+2; x++) {
	// 	// Top
	// 	glVertex3f(10.0 , x * 10, 0);
	// 	glVertex3f(width * 10 + 10.0 , x * 10, 0);
	// 	glVertex3f(width * 10 + 10.0 , x * 10 + 0.5, 0);
	// 	glVertex3f(10.0 , x * 10 + 0.5, 0);


	// 	// Down
	// 	glVertex3f(10.0 , x * 10, 0);
	// 	glVertex3f(width * 10 + 10.0 , x * 10, 0);
	// 	glVertex3f(width * 10 + 10.0 , x * 10, -0.2);
	// 	glVertex3f(10.0 , x * 10, -0.2);

	// 	// Up
	// 	glVertex3f(10.0 , x * 10 + 0.5, 0);
	// 	glVertex3f(width * 10 + 10.0 , x * 10 + 0.5, 0);
	// 	glVertex3f(width * 10 + 10.0 , x * 10 + 0.5, -0.2);
	// 	glVertex3f(10.0 , x * 10 + 0.5, -0.2);
	// }

	// glEnd();

	drawMaze();

	if(pacmanPath != NULL) {
		const double SHIFTFACTOR_X = -10.0;
		const double SHIFTFACTOR_Y = -11.5;

		glLoadIdentity();
		glTranslatef(pacmanPath->CurrentX() + SHIFTFACTOR_X, pacmanPath->CurrentY() + SHIFTFACTOR_Y, 0);
		glScalef(0.1, 0.1, 1);
		pacmanPath->Draw();
	}

	glutSwapBuffers();
}



void drawWall(int x, int y, int wall) {

	
	glBegin(GL_QUADS);

	// if(overview) {
	// } else {
	// }

	glColor3f(1 - R, 1 - G, 1 - B);
	

	switch(wall) {

	case up:

		// glVertex2f((x + 1) * 10.0 + 0.02, (y + 2) * 10.0);
		// glVertex2f((x + 2) * 10.0 - 0.02, (y + 2) * 10.0);

		// Top
		glVertex3f((x + 1) * 10.0 + 0.02, (y + 2) * 10.0, 0.0);
		glVertex3f((x + 2) * 10.0 - 0.02, (y + 2) * 10.0, 0.0);
		glVertex3f((x + 2) * 10.0 - 0.02, (y + 2) * 10.0 + 0.5, 0.0);
		glVertex3f((x + 1) * 10.0 + 0.02, (y + 2) * 10.0 + 0.5, 0.0);

		// Up
		glVertex3f((x + 1) * 10.0 + 0.02, (y + 2) * 10.0 + 0.5, 0.0);
		glVertex3f((x + 2) * 10.0 - 0.02, (y + 2) * 10.0 + 0.5, 0.0);
		glVertex3f((x + 2) * 10.0 - 0.02, (y + 2) * 10.0 + 0.5, -0.2);
		glVertex3f((x + 1) * 10.0 + 0.02, (y + 2) * 10.0 + 0.5, -0.2);
		
		// Down
		glVertex3f((x + 1) * 10.0 + 0.02, (y + 2) * 10.0, 0.0);
		glVertex3f((x + 2) * 10.0 - 0.02, (y + 2) * 10.0, 0.0);
		glVertex3f((x + 2) * 10.0 - 0.02, (y + 2) * 10.0, -0.2);
		glVertex3f((x + 1) * 10.0 + 0.02, (y + 2) * 10.0, -0.2);

		break;

	case down:
		// glVertex2f((x + 1) * 10.0 + 0.02, (y + 1) * 10.0);
		// glVertex2f((x + 2) * 10.0 - 0.02, (y + 1) * 10.0);
		// Top
		glVertex3f((x + 1) * 10.0 + 0.02, (y + 1) * 10.0, 0.0);
		glVertex3f((x + 2) * 10.0 - 0.02, (y + 1) * 10.0, 0.0);
		glVertex3f((x + 2) * 10.0 - 0.02, (y + 1) * 10.0 + 0.5, 0.0);
		glVertex3f((x + 1) * 10.0 + 0.02, (y + 1) * 10.0 + 0.5, 0.0);

		// Up
		glVertex3f((x + 1) * 10.0 + 0.02, (y + 1) * 10.0 + 0.5, 0.0);
		glVertex3f((x + 2) * 10.0 - 0.02, (y + 1) * 10.0 + 0.5, 0.0);
		glVertex3f((x + 2) * 10.0 - 0.02, (y + 1) * 10.0 + 0.5, -0.2);
		glVertex3f((x + 1) * 10.0 + 0.02, (y + 1) * 10.0 + 0.5, -0.2);

		// Down
		glVertex3f((x + 1) * 10.0 + 0.02, (y + 1) * 10.0 + 0.5, 0.0);
		glVertex3f((x + 2) * 10.0 - 0.02, (y + 1) * 10.0 + 0.5, 0.0);
		glVertex3f((x + 2) * 10.0 - 0.02, (y + 1) * 10.0 + 0.5, -0.2);
		glVertex3f((x + 1) * 10.0 + 0.02, (y + 1) * 10.0 + 0.5, 0.2);
		break;

	case right:
		// glVertex2f((x + 2) * 10.0, (y + 1) * 10.0 + 0.02);
		// glVertex2f((x + 2) * 10.0, (y + 2) * 10.0 - 0.02);

		glVertex3f((x + 2) * 10.0, (y + 1) * 10.0 + 0.02, 0.0);
		glVertex3f((x + 2) * 10.0, (y + 2) * 10.0 - 0.02, 0.0);
		glVertex3f((x + 2) * 10.0 + 0.5, (y + 2) * 10.0 - 0.02, 0.0);
		glVertex3f((x + 2) * 10.0 + 0.5, (y + 1) * 10.0 + 0.02, 0.0);

		// Left
		glVertex3f((x + 2) * 10.0, (y + 1) * 10.0 + 0.02, 0.0);
		glVertex3f((x + 2) * 10.0, (y + 2) * 10.0 - 0.02, 0.0);
		glVertex3f((x + 2) * 10.0, (y + 2) * 10.0 - 0.02, -0.2);
		glVertex3f((x + 2) * 10.0, (y + 1) * 10.0 + 0.02, -0.2);

		// Right
		glVertex3f((x + 2) * 10.0 + 0.5, (y + 1) * 10.0 + 0.02, 0.0);
		glVertex3f((x + 2) * 10.0 + 0.5, (y + 2) * 10.0 - 0.02, 0.0);
		glVertex3f((x + 2) * 10.0 + 0.5, (y + 2) * 10.0 - 0.02, -0.2);
		glVertex3f((x + 2) * 10.0 + 0.5, (y + 1) * 10.0 + 0.02, -0.2);
		break;

	case left:
		glVertex3f((x + 1) * 10.0, (y + 1) * 10.0 + 0.02, 0.0);
		glVertex3f((x + 1) * 10.0, (y + 2) * 10.0 - 0.02, 0.0);
		glVertex3f((x + 1) * 10.0 + 0.5, (y + 2) * 10.0 - 0.02, 0.0);
		glVertex3f((x + 1) * 10.0 + 0.5, (y + 1) * 10.0 + 0.02, 0.0);

		glVertex3f((x + 1) * 10.0, (y + 1) * 10.0 + 0.02, 0.0);
		glVertex3f((x + 1) * 10.0, (y + 2) * 10.0 - 0.02, 0.0);
		glVertex3f((x + 1) * 10.0, (y + 2) * 10.0 - 0.02, -0.2);
		glVertex3f((x + 1) * 10.0, (y + 1) * 10.0 + 0.02, -0.2);

		glVertex3f((x + 1) * 10.0 + 0.5, (y + 1) * 10.0 + 0.02, 0.0);
		glVertex3f((x + 1) * 10.0 + 0.5, (y + 2) * 10.0 - 0.02, 0.0);
		glVertex3f((x + 1) * 10.0 + 0.5, (y + 2) * 10.0 - 0.02, -0.2);
		glVertex3f((x + 1) * 10.0 + 0.5, (y + 1) * 10.0 + 0.02, -0.2);
		break;
	}

	glEnd();
}



void drawMaze() {

	int i;
	int x, y;

	for(i = 0 ; i < width * height ; i++) {
		x = i % width;
		y = i / width;

		if(cell[i].road[right] == false)	drawWall(x, y, right);
		if(cell[i].road[up] == false)        drawWall(x, y, up);
		if(cell[i].road[down] == false)		 drawWall(x, y, down);
		if(cell[i].road[left] == false)		 drawWall(x, y, left);
	}
}