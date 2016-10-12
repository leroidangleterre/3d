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
	void apply_gravity(Character*ch, float period);
	void collide(Character*cg, float period);
	void render(Camera*my_cam, GLuint texture);
	
private:

	void render(float xCam, float yCam, float zCam,
		    float xCible, float yCible, float zCible,
		    float xVertic, float yVertic, float zVertic,
		    GLuint texture);
	
	string filename;
	int nb_vertices;
	float**tab_vertices;
	float**tab_normals;
	float**tab_texture;
	int nb_faces;
	int**tab_faces;
	double z_min, z_max;
};


#endif
