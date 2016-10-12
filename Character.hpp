#ifndef CHARACTER
#define CHARACTER

#include <iostream>
#include <string>

#include <SDL/SDL.h>
#include <GL/gl.h>
#include <GL/glu.h>


using namespace std;

#include "Constants.hpp"

#include "Empty.hpp"
#include "Camera.hpp"

#include "Vector.hpp"


// Needed for compilation.
class Camera;


class Character
{


public:

	Character();
	string to_string();
	void print();
	void update_speed(int**tab, double period);
	void update_position(int**tab, double period);
	void render();
	void rotate_z(double angle);
	void set_camera(Camera*cam_param);
	void compute_global_speed();
	void compute_local_speed(int**tab, double period);
	void convert_speed_local_to_global();
	void convert_speed_global_to_local();
	Empty*get_empty();
	
private:

	Empty empty;
	double rotate_angle;
	Camera*my_cam;

	Vector local_speed;
	Vector global_speed;
};

#endif
