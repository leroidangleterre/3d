#ifndef EMPTY
#define EMPTY
#include <iostream>
#include <string>

#include "Vector.hpp"
#include "Arrow.hpp"


using namespace std;




class Empty
{


public:

	Empty();
	//Empty(const Empty&e);
	void print();
	string to_string();
	Vector*get_origin();
	Vector*get_target();
	Vector*get_left();
	Vector*get_vertic();
	void set_origin(Vector param);
	void set_origin(Empty param);
	void rotate_z(double angle);
	void rotate_local_y(double angle);
	void translate(Vector v);
	void push_forward(double distance);
	void raz_rotation();
	void render();

	
private:

	Vector origin;
	Vector target;
	Vector left;
	Vector vertic;

	/* The rotation around the Z axix is used by rotate_y_local. */
	double angle_z;
};

#endif
