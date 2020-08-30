#include <GL/gl.h>
#include <GL/glut.h>

#include <iostream>
#include <ctime>
#include <sys/time.h>
#include <cstring>

#include "pacman.h"
#include "pathfinder.h"
#include "objects.h"
#include "enemy.h"

extern bool lightMode;
extern int batteryTime;
void display();
void reviewpoint();
void drawMaze();
void displayText(float x, float y, float z, char *string);

static Cell *cell, *enemyCell;
static int width, height;				// Maze size
static int startingX, startingY;		// Start
static int goalX, goalY;				// Goal
static int positionX, positionY;		// Start

static double R = 33.0/255.0, G =  33.0/255.0, B = 222.0/255.0;		// Background

static int *chosen;						// Connected cells array
static bool work;						/* Maze generate/ Pause */
static int state = 0;					/* Making maze/ Finding path/ End */
static PathFinder* pacmanPath = NULL;	// Path finder object
static Ghost* enemyPath = NULL;	// Enemy finder object

static bool overview = false;
static bool autoMode = false;
static int userInputLastDirection = -1;

static int viewLeft, viewRight, viewBottom, viewUp;	// View points
static int ViewZoomFactor;
static int changeX, changeY;
static int timefactor;		// controls duration

int score = 0;
Objects* oneStar = NULL;
Objects* oneBomb = NULL;
Objects* oneBattery = NULL;
int flagStar = 1;
int flagBomb = 1;




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

static inline Cell & enemyXY(int x, int y) {
	return enemyCell[y * width + x];
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
		for(int i = 0; i < width*height; i++) {
			cell[i].isOpen = false;
			enemyCell[i].isOpen = false;
		}
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
			enemyXY(x, y).road[up] = true;			
			// goal
			goalX = x = rand()%width;
			goalY = y = 0;
			cellXY(x, y).road[down] = true;
			enemyXY(x, y).road[down] = true;			
		} else {
			// starting point
			startingX = x = width - 1;
			startingY = y = rand()%height;
			cellXY(x, y).road[right] = true;
			enemyXY(x, y).road[right] = true;			
			// goal
			goalX = x = 0;
			goalY = y = rand()%height;
			cellXY(x, y).road[left] = true;
			enemyXY(x, y).road[left] = true;			
		}

		chosen = new int [height * width];

		// choose the first cell randomly
		x = rand()%width;
		y = rand()%height;
		cellXY(x, y).isOpen = true;
		enemyXY(x, y).isOpen = true;
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
			enemyXY(x, y + 1).isOpen = true;

			cellXY(x, y + 1).road[ down ] = true;
			enemyXY(x, y + 1).road[ down ] = true;
			cellXY(x, y).road[ up ] = true;
			enemyXY(x, y).road[ up ] = true;

			chosen[length] = width*(y + 1) + x;
			length++;
			cellOpen = true;
			break;

		case down:
			if(y == 0 || cellXY(x, y - 1).isOpen == true)
				continue;

			cellXY(x, y - 1).isOpen = true;
			enemyXY(x, y - 1).isOpen = true;

			cellXY(x, y - 1).road[ up ] = true;
			enemyXY(x, y - 1).road[ up ] = true;
			cellXY(x, y).road[ down ] = true;
			enemyXY(x, y).road[ down ] = true;

			chosen[length] = width*(y - 1) + x;
			length++;
			cellOpen = true;
			break;

		case right:
			if(x == width-1 || cellXY(x + 1, y).isOpen == true)
				continue;

			cellXY(x + 1,  y).isOpen = true;
			enemyXY(x + 1,  y).isOpen = true;

			cellXY(x + 1,  y).road[ left ] = true;
			enemyXY(x + 1,  y).road[ left ] = true;
			cellXY(x,  y).road[ right ] = true;
			enemyXY(x,  y).road[ right ] = true;

			chosen[length] = width * y + x + 1;
			length++;
			cellOpen = true;
			break;

		case left:
			if(x == 0 || cellXY(x - 1,  y).isOpen == true)
				continue;

			cellXY(x - 1,  y).isOpen = true;
			enemyXY(x - 1,  y).isOpen = true;

			cellXY(x - 1,  y).road[ right ] = true;
			enemyXY(x - 1,  y).road[ right ] = true;
			cellXY(x,  y).road[ left ] = true;
			enemyXY(x,  y).road[ left ] = true;

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

	// gluOrtho2D(viewLeft, viewRight, viewBottom, viewUp);
	glOrtho(viewLeft, viewRight, viewBottom, viewUp, -15, 15);


	glMatrixMode (GL_MODELVIEW);
	glLoadIdentity();
}



// The space bar toggles making maze or pause
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
		batteryTime -= 1;
		
		break;
	case 'a':
		if (changeX > width * -5) changeX -= 5;	// scroll the maze left
		batteryTime -= 1;
		
		break;
	case 's':
		if (changeY > height * -5) changeY -= 5;	// scroll the maze down
		batteryTime -= 1;
		
		break;
	case 'd':
		if (changeX < width * 5) changeX += 5;	// scroll the maze right
		batteryTime -= 1;
		
		break;
	}
}

void pathFinding() {
	static int oldTime;
	int currTime = getTime();
	static PathFinder finder(::startingX, ::startingY, ::width, ::height);
	static int x = ::startingX;
	static int y = ::startingY;
	::positionX = x;
	::positionY = y;

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
		score = score + 100;
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
		::positionX = x;
		::positionY = y;
	} else {
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

		::positionX = x;
		::positionY = y;
	}
}



void enemyHunting() {
	static int oldTime;
	int currTime = getTime();
	static Ghost enemy(::goalX, ::goalY, ::width, ::height);
	static int x = ::goalX;
	static int y = ::goalY;

	if(currTime - oldTime > timefactor)
		oldTime = currTime;
	else return;

	if (enemyPath == NULL) {
		enemyPath = &enemy;	// To use in other functions
		enemy.SetBodyColor(1.0, 1.0, 0.0);
	}

	enemy.UpdateStatus();
	if(enemy.isMoving()) {
		return;
	}

	if(x == ::positionX && y == ::positionY) {	// if get Pacman
		::state = 5;
		enemyPath->setGetgoal();
		return;
	}

	
		enemyXY(x, y).isOpen = true;	// visit starting position
		if(enemy.isstackEmpty() || enemy.getStacktop() < NOT_ANY_DIRECTION) {
			if (enemyXY(x,  y).road[down] == true && y > 0 && enemyXY(x, y-1).isOpen == false) {
				enemy.setDest(Ghost::DOWN);
				y--;
				enemyXY(x, y).isOpen = true;
				enemy.stackPush(down);
				return;
			} else enemy.stackPush(NOTDOWN);
		}
		if(enemy.getStacktop() == NOTDOWN) {
			if(enemyXY(x, y).road[left] == true && x > 0 && enemyXY(x-1, y).isOpen == false) {
				enemy.setDest(Ghost::LEFT);
				x--;
				enemyXY(x, y).isOpen = true;
				enemy.stackPush(left);
				return;
			}
			else enemy.stackPush(NOTLEFT);
		}
		if(enemy.getStacktop() == NOTLEFT) {
			if(enemyXY(x, y).road[right] == true && x < ::width-1 && enemyXY(x+1, y).isOpen == false) {
				enemy.setDest(Ghost::RIGHT);
				x++;
				enemyXY(x, y).isOpen = true;
				enemy.stackPush(right);
				return;
			}
			else enemy.stackPush(NOTRIGHT);
		}
		if(enemy.getStacktop() == NOTRIGHT) {
			if(enemyXY(x, y).road[up] == true && y < ::height-1 && enemyXY(x, y+1).isOpen == false) {
				enemy.setDest(Ghost::UP);
				y++;
				enemyXY(x, y).isOpen = true;
				enemy.stackPush(up);
				return;
			}
			else enemy.stackPush(NOTUP);
		}
	
		int tempDest;
		enemy.stackPop();
		enemy.stackPop();
		enemy.stackPop();
		enemy.stackPop();
		tempDest = enemy.stackPop();
		if(tempDest == down) y++, enemy.setDest(Ghost::UP);
		else if(tempDest == left) x++, enemy.setDest(Ghost::RIGHT);
		else if(tempDest == right) x--, enemy.setDest(Ghost::LEFT);
		else if(tempDest == up) y--, enemy.setDest(Ghost::DOWN);
	
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
	batteryTime -= 1;
		userInputLastDirection = right;
		break;
	case GLUT_KEY_LEFT:
	batteryTime -= 1;
		userInputLastDirection = left;
		break;
	case GLUT_KEY_DOWN:
	batteryTime -= 1;
		userInputLastDirection = down;
		break;
	case GLUT_KEY_UP:
	batteryTime -= 1;
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

	// gluOrtho2D(viewLeft, viewRight, viewBottom, viewUp);
	glOrtho(viewLeft, viewRight, viewBottom, viewUp, -15, 15);

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
		if(autoMode == false)
			enemyHunting();
		break;
	case 2:
		std::cout << "\nReached the end!\nScore is " << score << "\nBattery is " << batteryTime <<"\n"; 
	case 3:
		
		exit(0);
		break;
	}
	display();
}

int main(int argc, char ** argv) {
	using namespace std;
	width = 10;
	height = 10;

	if (argc > 1) {
		if (strcmp(argv[1], "--help") == 0) {
			cout << "usage: " << argv[0] << " [--auto | --custom | -light]" << endl;
			return 0;
		} else if (strcmp(argv[1], "--auto") == 0) {
			autoMode = true;
		} else if (strcmp(argv[1], "--light") == 0) {
			lightMode = true;
		} else if (strcmp(argv[1], "--custom") == 0) {
		
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
	enemyCell = new Cell[width * height];

	Objects Star(rand()%(height - 2) * 10.0 + 15.0, rand()%(width - 2) * 10.0 + 15.0, 100);
	// cout << "A: " << rand()%(height - 2) * 10.0 + 15.0 << "\n";
	if (oneStar == NULL) {
		oneStar = &Star;	// To use in other functions
	}

	// std::cout << "Star: " << oneStar->CurrentX() << " " << oneStar->CurrentY() << "\n";


	Objects Bomb(rand()%(height - 2) * 10.0 + 15.0, rand()%(width - 2) * 10.0 + 15.0, 101);
	// cout << "B: " << rand()%(height - 2) * 10.0 - 5.0 << "\n";
	if (oneBomb == NULL) {
		oneBomb = &Bomb;	// To use in other functions
	}

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
	// glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
	GLfloat ambientColor[] = {1.0f, 1.0f, 1.0f, 1.0f};
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambientColor);

	glEnable(GL_COLOR_MATERIAL);
	glEnable(GL_LIGHTING);
	glEnable(GL_DEPTH_TEST);

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
	glClearDepth(-15.0);


	glColor3f(R, G, B);
	// Draw square with many little squares
	glBegin(GL_QUADS);
		glNormal3f(0.0, 0.0, 1.0);
		const GLfloat kqDelta = 1;
		for (int i = 0; i < 500; ++i) {
			for (int j = 0; j < 500; ++j) {
				glVertex3f(j * kqDelta, i * kqDelta, -1.0);
				glVertex3f((j + 1) * kqDelta, i * kqDelta, -1.0);
				glVertex3f((j + 1) * kqDelta, (i + 1) * kqDelta, -1.0);
				glVertex3f(j * kqDelta, (i + 1) * kqDelta, -1.0);
			}
		}
	glEnd();
	glFlush();

	glColor3f(1 - R, 1 - G, 1 - B);	// The color is the negative of background color

	// draw default(unmaden) maze
	glLoadIdentity();
    glLineWidth(2.0);

	char buffer[20];
	snprintf(buffer, 20, "Score: %d", score);
	displayText(11, 112, 0, buffer);

	if(lightMode) {
		if(batteryTime < 0) {
			glDisable(GL_LIGHT0);
			std::cout << "\nYou exhausted your battery!\n";
			exit(0);
		}
	
		snprintf(buffer, 20, "Battery: %d", batteryTime);
		displayText(90, 112, 0, buffer);
	}

	drawMaze();
	// std::cout << "Hey: " << ::positionX << " " << ::positionY << "\n";
	if(oneStar->getActive() == true && oneStar->CurrentX() == ::positionX * 10.0 + 15.0 && oneStar->CurrentY() == ::positionY * 10.0 + 15.0) {
		oneStar->setUsed();
		flagStar = 0;
	}

	if(!flagStar && oneStar->getActive() == false) {
		score += 10;
		flagStar = 1;
	}

	if(oneBomb->getActive() == true && oneBomb->CurrentX() == ::positionX * 10.0 + 15.0 && oneBomb->CurrentY() == ::positionY * 10.0 + 15.0) {
		oneBomb->setUsed();
		flagBomb = 0;
	}

	if(!flagBomb && oneBomb->getActive() == false) {
		score -= 10;
		flagBomb = 1;
	}

	oneStar->lists();
	oneBomb->lists();

	if(pacmanPath != NULL) {
		const double SHIFTFACTOR_X = -10.0;
		const double SHIFTFACTOR_Y = -11.5;

		glLoadIdentity();
		glTranslatef(pacmanPath->CurrentX() + SHIFTFACTOR_X, pacmanPath->CurrentY() + SHIFTFACTOR_Y, 0);
		glScalef(0.1, 0.1, 1);
		pacmanPath->Draw();
	}


	if(enemyPath != NULL) {
		const double SHIFTFACTOR_X = -10.0;
		const double SHIFTFACTOR_Y = -11.5;

		glLoadIdentity();
		glTranslatef(enemyPath->CurrentX() + SHIFTFACTOR_X, enemyPath->CurrentY() + SHIFTFACTOR_Y, 0);
		glScalef(0.1, 0.1, 1);
		enemyPath->Draw();
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

		// Top
		glNormal3f(0.0, 0.0, 1.0);
		glVertex3f((x + 1) * 10.0, (y + 2) * 10.0, 14.0);
		glVertex3f((x + 2) * 10.0, (y + 2) * 10.0, 14.0);
		glVertex3f((x + 2) * 10.0, (y + 2) * 10.0 + 0.5, 14.0);
		glVertex3f((x + 1) * 10.0, (y + 2) * 10.0 + 0.5, 14.0);

		// Up
		glNormal3f(0.0, 1.0, 0.0);
		glVertex3f((x + 1) * 10.0, (y + 2) * 10.0 + 0.5, 14.0);
		glVertex3f((x + 2) * 10.0, (y + 2) * 10.0 + 0.5, 14.0);
		glVertex3f((x + 2) * 10.0, (y + 2) * 10.0 + 0.5, -14.0);
		glVertex3f((x + 1) * 10.0, (y + 2) * 10.0 + 0.5, -14.0);
		
		// Down
		glNormal3f(0.0, -1.0, 0.0);
		glVertex3f((x + 1) * 10.0, (y + 2) * 10.0, 14.0);
		glVertex3f((x + 2) * 10.0, (y + 2) * 10.0, 14.0);
		glVertex3f((x + 2) * 10.0, (y + 2) * 10.0, -14.0);
		glVertex3f((x + 1) * 10.0, (y + 2) * 10.0, -14.0);
		

		break;

	case down:
		
	// Top
		glNormal3f(0.0, 0.0, 1.0);
		glVertex3f((x + 1) * 10.0, (y + 1) * 10.0, 14.0);
		glVertex3f((x + 2) * 10.0, (y + 1) * 10.0, 14.0);
		glVertex3f((x + 2) * 10.0, (y + 1) * 10.0 + 0.5, 14.0);
		glVertex3f((x + 1) * 10.0, (y + 1) * 10.0 + 0.5, 14.0);

		// Up
		glNormal3f(0.0, 1.0, 0.0);
		glVertex3f((x + 1) * 10.0, (y + 1) * 10.0 + 0.5, 14.0);
		glVertex3f((x + 2) * 10.0, (y + 1) * 10.0 + 0.5, 14.0);
		glVertex3f((x + 2) * 10.0, (y + 1) * 10.0 + 0.5, -14.0);
		glVertex3f((x + 1) * 10.0, (y + 1) * 10.0 + 0.5, -14.0);

		// Down
		glNormal3f(0.0, -1.0, 0.0);
		glVertex3f((x + 1) * 10.0, (y + 1) * 10.0, 14.0);
		glVertex3f((x + 2) * 10.0, (y + 1) * 10.0, 14.0);
		glVertex3f((x + 2) * 10.0, (y + 1) * 10.0, -14.0);
		glVertex3f((x + 1) * 10.0, (y + 1) * 10.0, -14.0);	
		break;

	case right:
		
		glNormal3f(0.0, 0.0, 1.0);
		glVertex3f((x + 2) * 10.0, (y + 1) * 10.0, 14.0);
		glVertex3f((x + 2) * 10.0, (y + 2) * 10.0, 14.0);
		glVertex3f((x + 2) * 10.0 + 0.5, (y + 2) * 10.0, 14.0);
		glVertex3f((x + 2) * 10.0 + 0.5, (y + 1) * 10.0, 14.0);

		// Left
		glNormal3f(-1.0, 0.0, 0.0);
		glVertex3f((x + 2) * 10.0, (y + 1) * 10.0, 14.0);
		glVertex3f((x + 2) * 10.0, (y + 2) * 10.0, 14.0);
		glVertex3f((x + 2) * 10.0, (y + 2) * 10.0, -14.0);
		glVertex3f((x + 2) * 10.0, (y + 1) * 10.0, -14.0);

		// Right
		glNormal3f(1.0, 0.0, 0.0);
		glVertex3f((x + 2) * 10.0 + 0.5, (y + 1) * 10.0, 14.0);
		glVertex3f((x + 2) * 10.0 + 0.5, (y + 2) * 10.0, 14.0);
		glVertex3f((x + 2) * 10.0 + 0.5, (y + 2) * 10.0, -14.0);
		glVertex3f((x + 2) * 10.0 + 0.5, (y + 1) * 10.0, -14.0);
		break;

	case left:
		glNormal3f(0.0, 0.0, 1.0);
		glVertex3f((x + 1) * 10.0, (y + 1) * 10.0, 14.0);
		glVertex3f((x + 1) * 10.0, (y + 2) * 10.0, 14.0);
		glVertex3f((x + 1) * 10.0 + 0.5, (y + 2) * 10.0, 14.0);
		glVertex3f((x + 1) * 10.0 + 0.5, (y + 1) * 10.0, 14.0);

		// Left
		glNormal3f(-1.0, 0.0, 0.0);
		glVertex3f((x + 1) * 10.0, (y + 1) * 10.0, 14.0);
		glVertex3f((x + 1) * 10.0, (y + 2) * 10.0, 14.0);
		glVertex3f((x + 1) * 10.0, (y + 2) * 10.0, -14.0);
		glVertex3f((x + 1) * 10.0, (y + 1) * 10.0, -14.0);

		// Right
		glNormal3f(1.0, 0.0, 0.0);
		glVertex3f((x + 1) * 10.0 + 0.5, (y + 1) * 10.0, 14.0);
		glVertex3f((x + 1) * 10.0 + 0.5, (y + 2) * 10.0, 14.0);
		glVertex3f((x + 1) * 10.0 + 0.5, (y + 2) * 10.0, -14.0);
		glVertex3f((x + 1) * 10.0 + 0.5, (y + 1) * 10.0, -14.0);
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

		if(cell[i].road[right] == false) drawWall(x, y, right);
		if(cell[i].road[up] == false) drawWall(x, y, up);
		if(cell[i].road[down] == false)	drawWall(x, y, down);
		if(cell[i].road[left] == false)	drawWall(x, y, left);
	}
}

void displayText(float x, float y, float z, char *string) {
	glDisable(GL_LIGHTING);
	glColor3f(255.0/255.0, 215.0/255.0, 0);

  	glRasterPos3f(x, y, z);
	for (char* c = string; *c != '\0'; c++) {
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *c);  // Updates the position
	}

	glEnable(GL_LIGHTING);
}
    




