#include "Camera.hpp"


using std::string;

Camera::Camera(Empty e) : empty{e}
{
	
}

Camera::Camera() : empty{}
{
	
}


void Camera::set_character(Character*c){
	character = c;
	distance_to_character = 5;
}


/* Set the origin of the camera, not its orientation. */
void Camera::set_position(Empty param){
	empty.set_origin(param);
}

/* Set both origin and orientation of the camera
   by copying the parameter. */
void Camera::set_empty(Empty&e){
	empty = Empty(e);
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
	// cout << "Camera::rotate_local_y( " << angle << " );" << endl;
	empty.rotate_local_y(angle);
}

void Camera::raz_rotation(){
	empty.raz_rotation();
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


void Camera::update_position(int**tab, double period){

	/* Change the distance to the character when the mousewheel is scrolled.
	   Scroll up: get closer to the character;
	   Scroll down: get further away. */

	Vector unused; // TODO: check warning
	Vector diff;
	
	while((*tab)[WHEEL_LEVEL] > 0){
		(*tab)[WHEEL_LEVEL] --;
		push_forward(distance_to_character);
		distance_to_character *= 1.1;
		push_forward(-distance_to_character);
	}
	while((*tab)[WHEEL_LEVEL] < 0){
		(*tab)[WHEEL_LEVEL] ++;
		push_forward(distance_to_character);
		distance_to_character /= 1.1;
		push_forward(-distance_to_character);
	}

	translate(diff);
}



void Camera::multiply_distance_from_target(double coef){
	if(coef > 0){
		distance_to_character *= coef;
	}
}
