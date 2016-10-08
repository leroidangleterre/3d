#ifndef TERRAIN
#define TERRAIN

class Terrain{
	
public:

	Terrain();
	Terrain(std::string filename_param);
	void print_state();
	void load_mesh();
    
private:
	
	std::string filename;
};


#endif
