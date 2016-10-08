#include <iostream>
#include "Vector.hpp"
#include "Terrain.hpp"
using namespace std;

int main(){
	/*
	  Camera my_cam();
	  Character my_character();
	*/
	Vector v;
	Vector w;
	Vector z(1, 2, 3);

	Terrain terrain;
	Terrain terrain2("world_test.ply");
	
	v.afficherEtat();
	w.afficherEtat();
	z.afficherEtat();


	terrain.print_state();
	terrain2.print_state();
	

	std::cout << "coucou" << std::endl;
	
	return 0;
}
