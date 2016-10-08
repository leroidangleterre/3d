#include <iostream>

#include "Terrain.hpp"


Terrain::Terrain() : filename{"_"}
{
	
}


Terrain::Terrain(std::string filename_param) : filename{filename_param}
{
	std::cout << "Terrain: loading file '" << filename_param << "'" << std::endl;
	// Read file and create data structure.
	
}

void Terrain::print_state(){
	std::cout << "Terrain {" << filename << "}" << std::endl;

	
}


void Terrain::load_mesh(){

}
