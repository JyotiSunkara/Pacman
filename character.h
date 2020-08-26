#ifndef CHARACTER_H
#define CHARACTER_H

# define ROLL_FACTOR 10

#define Face 1
#define Mouth  2
#define Eye  3


class Pacman {

    private:
        // Position and direction for path finding 
        double currentX;
        double currentY;
        double oldX;
        double oldY;
        int Destination;
        int initDest;
        bool getGoal;

        // Animation 
        void lists();	// call list of the finder
        bool inMotion;
        double degree_7;    // a degree of 7, used calculating movement of one frame of animation
        int walkStatus;     // Waking animation
        int eyeStatus;		// Face animation
        int rollingStatus;	// Rotate animation
        int goalceremonyStatus;	// for goal ceremony
        double bodyColorR;
        double bodyColorG;
        double bodyColorB;

        // Stack of path finding 
        int* recursionStack;
        int stackTop;

    public:
        enum Direction {
            UP = 0,
            DOWN = 1,
            RIGHT = 2,
            LEFT = 3
        };

        Pacman(int xPosition, int yPosition, int mazeWidth, int mazeHeight);
	    ~Pacman() { 
            delete recursionStack; 
        }

        bool isMoving() { return inMotion; }	// is it doing moving animation?
	    void setDest(Direction newDest);

	    void Move();
        void SetBodyColor(double r, double g, double b)	{ 
            bodyColorR = r; 
            bodyColorG = g, 
            bodyColorB = b; 
        }

        void Draw();
        void UpdateStatus();

        double CurrentX() { 
            return currentX; // Return X position of the finder
        }

        double CurrentY() { 
            return currentY; // Return y position of the finder
        }

        void setgetgoal() { 
            getGoal = true; 
        }

        // Stack of path finding 
        bool isstackEmpty() { 
            return (stackTop == -1); 
        }

        void stackPush(int dest) { 
            stackTop = stackTop + 1;
            recursionStack[stackTop] = dest; 
        }

        int stackPop() { 
            return (stackTop == -1)? recursionStack[stackTop + 1]: recursionStack[stackTop--]; 
        }

        int getStacktop() { 
            return recursionStack[stackTop]; 
        }


    
};

#endif