#include "Camera.hpp"


using std::string;

Camera::Camera(Empty e) : empty{e}
{
	
}

Camera::Camera() : empty{}{
	
}


void Camera::set_character(Character*c){
	character = c;
}

void Camera::set_position(Empty param){
	empty.set_origin(param);
}

void Camera::afficherEtat(){
	std::cout << "Camera {" << empty.to_string() << "}" << std::endl;
}

string Camera::to_string(){
	return empty.to_string();
}

void Camera::push_forward_with_wheel(int*wheel_level){

}

Empty*Camera::get_empty(){

	return &empty;
}


void Camera::rotate_z(double angle){
	empty.rotate_z(angle);
}

void Camera::rotate_local_y(double angle){
	cout << "Camera::rotate_local_y( " << angle << " );" << endl;
	empty.rotate_local_y(angle);
}

void Camera::translate(Vector v){
	empty.translate(v);
}


void Camera::push_forward(double distance){
	empty.push_forward(distance);
}

/* These methods are used before and after the rotation around the local Y axis. */
void Camera::push_to_character(){
	empty.set_origin(*(character->get_empty()->get_origin()));
}
void Camera::pull_from_character(){
	push_forward(-distance_to_character);
}
