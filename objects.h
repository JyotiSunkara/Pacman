#ifndef OBJECTS_H
#define OBJECTS_H

class Objects {

private:
    int posX;
    int posY;
    bool active;
    int type;

public:
    enum Type { 
		STAR = 0, // For battery
		BOMB = 1, // Lose points
		COIN = 2, // Gain points
	};
    Objects(int positionX, int positionY, int code);
	~Objects() {
		active = false;
	}
    void lists();
    double CurrentX() { 
		return posX; 
	}	// return x position of the finder
	double CurrentY() {
		return posY; 
	}

	void setUsed() {
		active = false;
	}

};

#endif

