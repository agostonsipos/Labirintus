#include "GameObjects.hpp"
#include <cmath>
#include <vector>

MovingObject MovingObject::operator+=(MovingObject s)
{
	x += s.x; y += s.y; dir += s.dir;
	return *this;
}

void MovingObject::removeNumericalErrors()
{
	x = roundf(x);
	y = roundf(y);
	dir = roundf(dir);
}

FixedObject FixedObject::operator+(FixedObject b)
{
	return FixedObject(x + b.x, y + b.y);
}

bool operator<(Border a, Border b){
	std::vector<int> v1 = { a.x, a.y, a.dir }, v2 = { b.x, b.y, b.dir };
	return std::lexicographical_compare(v1.begin(), v1.end(), v2.begin(), v2.end());
}

bool operator<(FixedObject a, FixedObject b){
	if (a.x < b.x){
		return true;
	}
	else if (a.x > b.x){
		return false;
	}
	else{
		return a.y < b.y;
	}
}
