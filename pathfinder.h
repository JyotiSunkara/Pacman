#ifndef PATHFINDER_H
#define PATHFINDER_H

#define Face  1
#define Mouth  2
#define Eye  3

#define ROLL_FACT	10

class PathFinder {

private:
	/* position and direction for path finding */
	double currentX;
	double currentY;
	double oldX;
	double oldY;
	int Dest;
	int initDest;
	bool getGoal;

	/* animation */
	void lists(int positionX, int positionY);	// call list of the finder
	bool ismoving;
	double degree_7;	// a degree of 7, used calculating movement of one frame of animation
	int walkStatus;	// for waking animation
	int eyeStatus;		// for face animation
	int rollingStatus;	// for rotate animation
	int goalceremonyStatus;	// for goal ceremony
	double bodyColorR;
	double bodyColorG;
	double bodyColorB;

	/* stack of path finding */
	int* recursionStack;
	int stackTop;

public:
	enum Direction { 
		UP = 0, 
		DOWN = 1, 
		RIGHT = 2, 
		LEFT = 3 
	};
	PathFinder(int positionX, int positionY, int mazeWidth, int mazeHeight);
	~PathFinder() { 
		delete recursionStack; 
	}

	bool isMoving() { 
		return ismoving; 
	}	// Is it doing moving animation

	void setDest(Direction newDest);
	void Move();
	void SetBodyColor(double r, double g, double b)	{ 
		bodyColorR = r; 
		bodyColorG = g, 
		bodyColorB = b; 
	}

	void Draw();
	void UpdateStatus();
	// If animation is false, it doesn't move mouth. Just draw
	// If getGoal is true then do goal ceremony

	double CurrentX() { 
		return currentX; 
	}	// return x position of the finder
	double CurrentY() {
		return currentY; 
	}	// return y position of the finder
	void setGetgoal() { 
		getGoal = true; 
	}

	/* Stack of path finding */
	bool isstackEmpty() { 
		return stackTop == -1; 
	}

	void stackPush(int dest) { 
		recursionStack[++stackTop] = dest; 
	}

	int stackPop() { 
		return (stackTop == -1)? recursionStack[stackTop+1]: recursionStack[stackTop--]; 
	}

	int getStacktop() { 
		return recursionStack[stackTop]; 
	}
};

#endif
