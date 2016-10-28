#ifndef TERRAIN
#define TERRAIN

#include <iostream>
#include <fstream>
#include <string>
#include <GL/gl.h>
#include <GL/glu.h>

#include "sdlglutils.h"
#include "Character.hpp"
#include "Camera.hpp"
#include "Vector.hpp"

using namespace std;

class Terrain{
	
public:

	Terrain();
	Terrain(string filename_param);
	void print_state();
	void load_mesh();
	string to_string();
	void evolve(int**pressed_keys_tab, Character*ch);
	void apply_gravity(Character*ch, double period);
	void collide(Character*cg, double period);
	void render(Camera*my_cam, GLuint texture);
	bool get_properly_loaded();
	
private:

	void render(double xCam, double yCam, double zCam,
		    double xCible, double yCible, double zCible,
		    double xVertic, double yVertic, double zVertic,
		    GLuint texture);
	
	string filename;
	int nb_vertices;
	double**tab_vertices;
	double**tab_normals;
	double**tab_texture;
	int nb_faces;
	int**tab_faces;
	double z_min, z_max;
	bool is_properly_loaded;
};


#endif
