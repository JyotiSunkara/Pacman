#ifndef ENEMY_H
#define ENEMY_H

#define EnemyFace  11
#define EnemyMouth  12
#define EnemyEye  13

class Ghost {
private:
    double currentX;
    double currentY;
    double oldX;
    double oldY;
    int Dest;
    int initDest;
    bool getGoal;
    bool active;

    /* animation */
	void lists();	// call list of the finder
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
	Ghost(int positionX, int positionY, int mazeWidth, int mazeHeight);
	~Ghost() { 
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