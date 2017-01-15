#pragma once

struct MovingObject{
	float x;
	float y;
	float dir;
	MovingObject(float a, float b, float c) : x(a), y(b), dir(c){}
	MovingObject(int a = 0, int b = 0, int c = 0) : x(a), y(b), dir(c){}
	MovingObject operator+=(MovingObject s);
	void removeNumericalErrors();
};
struct FixedObject{
	int x;
	int y;
	FixedObject(int a, int b) : x(a), y(b){}
	FixedObject(MovingObject s) : x(s.x), y(s.y){}
	FixedObject operator+(FixedObject b);
};
struct Border{
	int x;
	int y;
	bool dir;
	Border(int a, int b, bool c) :x(a), y(b), dir(c){}
};

// Only for putting them into std::set
bool operator<(Border a, Border b);
bool operator<(FixedObject a, FixedObject b);

// Used names in the program
typedef MovingObject PlayerObject;
typedef MovingObject Shot;

typedef FixedObject Coin;
typedef FixedObject Diamond;

typedef Border Bush;
