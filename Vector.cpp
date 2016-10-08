#include "Vector.hpp"

#include <iostream>

using namespace std;

Vector::Vector() : x{0}, y{0}, z{0}
{
	
}


Vector::Vector(float xx, float yy, float zz):
	x{xx}, y{yy}, z{zz}
	 {
	 }

void Vector::afficherEtat(){
	std::cout << "Vector {" << x << ", " << y << ", " << z << "}" << std::endl;
}

