#ifndef CHARACTER
#define CHARACTER

#include <iostream>
#include <string>

using namespace std;

#include "Empty.hpp"


class Character
{


public:

	Character();
	string to_string();
	void print();
	
private:

	Empty empty;

};

#endif
