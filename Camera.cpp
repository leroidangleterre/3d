#include "Camera.hpp"


using std::string;

Camera::Camera(Empty e) : empty{e}
{
	
}

Camera::Camera() : empty{}{
	
}



void Camera::afficherEtat(){
	std::cout << "Camera {" << empty.to_string() << "}" << std::endl;
}

string Camera::to_string(){
	return empty.to_string();
}
