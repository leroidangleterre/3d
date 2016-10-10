#ifndef EMPTY
#define EMPTY
#include <iostream>
#include <string>

#include "Vector.hpp"


using namespace std;


class Empty
{


public:

	Empty();
	void print();
	string to_string();

	
private:

	Vector origin;
	Vector target;
	Vector left;
	Vector vertic;

};

#endif
