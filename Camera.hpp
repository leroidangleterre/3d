#ifndef CAMERA
#define CAMERA

#include <iostream>
#include <string>

#include "Empty.hpp"
#include "Character.hpp"


// Needed for compilation.
class Character;


class Camera
{
	
public:

	Camera();
	Camera(Empty e);
	void set_character(Character*c);
	void set_position(Empty pos);
	void set_empty(Empty&e);
	void afficherEtat();
	string to_string();
	void push_forward_with_wheel(int*wheel_level);
	void push_forward(double distance);
	//void avoid_walls(Terrain*my_terrain);
	Empty*get_empty();
	void rotate_z(double angle);
	void rotate_local_y(double angle);
	void raz_rotation();
	void translate(Vector v);

	/* Bring to the character and memorize the distance;
	   replace at the former distance. */
	void push_to_character();
	void pull_from_character();
	void update_position(int**tab, double period);
	void multiply_distance_from_target(double coef); // Only works if coef>0

	
private:
	
	Empty empty;
	Character*character;
	double distance_to_character;
};

#endif
