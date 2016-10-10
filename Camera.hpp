#ifndef CAMERA
#define CAMERA

#include <iostream>
#include <string>

#include "Empty.hpp"


class Camera
{
	
public:

	Camera();
	Camera(Empty e);
	void afficherEtat();
	string to_string();
	
private:
	
	Empty empty;
};

#endif
