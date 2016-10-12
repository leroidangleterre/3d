#include "Terrain.hpp"



Terrain::Terrain() : filename{"_"}{
	
}


// Read file and create data structure.
Terrain::Terrain(string filename_param) : filename{filename_param}{

	load_mesh();
}


void Terrain::load_mesh(){
	

	ifstream file(filename, ios::in);
	string line;
	string word;
	int pos;
	bool reading_header = true;
	int i, j;

	z_min = 0;
	z_max = 0;

	cout << "Terrain: loading file '" << filename << "'" << endl;

	if(!file){
		cout << "Terrain::Terrain : error when opening file '" << filename << "'." << endl;
	}
	else{
		// Header
		/* TODO: adapt the header to the types of data:
		   position, normal and/or texture of each vertex. */
		while(getline(file, line) && reading_header){

			if(line.substr(0, 10) == "end_header"){
				reading_header = false;
			}

			/* Find the amount of vertices and faces. */
			if(line.substr(0, 14) == "element vertex"){
				nb_vertices = stoi(line.substr(14, -1));
				// cout << "Number of vertices: " << nb_vertices << endl;
			}
			if(line.substr(0, 12) == "element face"){
				nb_faces = stoi(line.substr(12, -1));
				// cout << "Number of faces: " << nb_faces << endl;
			}
		}

		/* Memory allocation. */
		tab_vertices = (float**)malloc(nb_vertices*sizeof(float*));
		tab_texture = (float**)malloc(nb_vertices*sizeof(float*));
		tab_normals = (float**)malloc(nb_vertices*sizeof(float*));
		for(i=0; i<nb_vertices; i++){
			tab_vertices[i] = (float*)malloc(3*sizeof(float));
			tab_texture[i] = (float*)malloc(3*sizeof(float));
			tab_normals[i] = (float*)malloc(3*sizeof(float));
		}
		tab_faces = (int**)malloc(nb_faces*sizeof(int*));
		for(i=0; i<nb_faces; i++){
			tab_faces[i] = (int*)malloc(3*sizeof(int));
		}


		/* Vertices. */
		
		i=0;
		while(i<nb_vertices-1){
			getline(file, line);	

			/* Vertices coordinates. */
			for(j=0; j<3; j++){
				pos = line.find(" ");
				word = line.substr(0, pos);
				tab_vertices[i][j] = stof(word);
				line = line.substr((pos+1), -1);
			}
			/* Vertices normals. */
			for(j=0; j<3; j++){
				pos = line.find(" ");
				word = line.substr(0, pos);
				tab_normals[i][j] = stof(word);
				line = line.substr((pos+1), -1);
			}
			/* UV-coordinates. */
			pos = line.find(" ");
			word = line.substr(0, pos);
			tab_texture[i][0] = stof(word);
			line = line.substr((pos+1), -1);
			tab_texture[i][1] = stof(line);

			/*
			if(i > nb_vertices - 15){
				cout << "Checking: ("
				     << tab_vertices[i][0] << " ; "
				     << tab_vertices[i][1] << " ; "
				     << tab_vertices[i][2] << ") ; ("
				     << tab_normals[i][0] << " ; "
				     << tab_normals[i][1] << " ; "
				     << tab_normals[i][2] << ") ; ("
				     << tab_texture[i][0] << " ; "
				     << tab_texture[i][1] << ")"
				     << endl;
			}
			*/
			
			i++;
		}

		/* Set the values of z_min and z_max. */
		z_min = tab_vertices[0][2];
		z_max = tab_vertices[0][2];
		for(i=1; i<nb_vertices; i++){
			if(tab_vertices[i][2] > z_max){
				z_max = tab_vertices[i][2];
			}
			if(tab_vertices[i][2] < z_min){
				z_min = tab_vertices[i][2];
			}
		}

		/* Faces. */
		
		i=0;
		while(getline(file, line) && i<nb_faces){

			// cout << "line = '" << line << "'" << endl;

			/* Number of points for the current face. */
			pos = line.find(" ");
			line = line.substr((pos+1), -1);
			
			/* Indexes for the current face. */
			for(j=0; j<3; j++){
				pos = line.find(" ");
				word = line.substr(0, pos);
				tab_faces[i][j] = stoi(word);
				line = line.substr((pos+1), -1);
			}
			
			/*
			cout << "Checking: ("
			     << tab_faces[i][0] << " ; "
			     << tab_faces[i][1] << " ; "
			     << tab_faces[i][2] << ") ;"
			     << endl;
			*/
			
			i++;
		}
		
		cout << "Terrain file loaded." << endl;
		// End of file.
	}
}

void Terrain::print_state(){
	cout << "Terrain {" << filename << "}" << endl;

	
}

string Terrain::to_string(){
	return "Terrain: {" + filename + " }";
}



/* Use the content of the table pointed to by the first parameter. */
void Terrain::evolve(int**ptr_pressed_keys_tab, Character*ch){


}



/* The gravity around the terrain affects the character.
   It may be uniform or computed with the current position of the character.
   That function modifies the speed of the character.
 */
void Terrain::apply_gravity(Character*ch, float period){


}


void Terrain::collide(Character*cg, float period){

}



void Terrain::render(Camera*my_cam, GLuint texture){
	
	glEnable(GL_TEXTURE_2D);

	Empty*empty = my_cam -> get_empty();
	render(empty->get_origin()->get_x(),
	       empty->get_origin()->get_y(),
	       empty->get_origin()->get_z(),
	       empty->get_target()->get_x(),
	       empty->get_target()->get_y(),
	       empty->get_target()->get_z(),
	       empty->get_vertic()->get_x(),
	       empty->get_vertic()->get_y(),
	       empty->get_vertic()->get_z(),
	       texture);
	glDisable(GL_TEXTURE_2D);
}



void Terrain::render(float xCam, float yCam, float zCam,
		     float xCible, float yCible, float zCible,
		     float xVertic, float yVertic, float zVertic,
		     GLuint texture){

	int index;
	int i, j;
	float z;
	float z_int = 0.5*(z_min+z_max);
	/* Color: RGB and UV-mapping. */
	int r, g, b;
	float uv_u, uv_v;


	gluLookAt(xCam ,yCam, zCam,
		  xCam+xCible, yCam+yCible, zCam+zCible,
		  xVertic, yVertic, zVertic);

	
	/* Display each triangle with the appropriate texture. */
	glBegin(GL_TRIANGLES);
	for(i=0; i<nb_faces; i++){
		for(j=0; j<=2; j++){
			index = tab_faces[i][j];
			if(index >= nb_vertices){
				cout << "Error render" << endl;
			}

			uv_u = tab_texture[index][0];
			uv_v = tab_texture[index][1];

			glTexCoord2d(uv_u, uv_v);
			glVertex3d(tab_vertices[index][0],
				   tab_vertices[index][1],
				   tab_vertices[index][2]);
		}
	}
	glEnd();
}
