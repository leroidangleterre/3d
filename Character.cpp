#include "Character.hpp"

Character::Character(): empty(), rotate_angle(0.5), my_cam(), local_speed(), global_speed()
{
}




string Character::to_string(){
	return empty.to_string();
}

void Character::print(){

	cout << to_string() << endl;
}


void Character::set_camera(Camera*cam_param){
	my_cam = cam_param;
	my_cam->set_character(this);
	/* Place the camera at the appropriate location. */
	//my_cam->set_position(empty);

	/* Place the camera at the same location and orientation as the character. */
	my_cam->set_empty(empty);
	
	my_cam->push_forward(-3);
}


/* Updates the local expression of the speed (in the character's referential). */
void Character::update_speed(int**tab, double period){

	if((*tab)[SDLK_z] == 1){
		(*tab)[SDLK_z] = 0;
		local_speed.increase(+1, 0, 0);
	}
	if((*tab)[SDLK_s] == 1){
		(*tab)[SDLK_s] = 0;
		local_speed.increase(-1, 0, 0);
	}
	if((*tab)[SDLK_SPACE] == 1){
		(*tab)[SDLK_SPACE] = 0;
		local_speed.increase(0, 0, +1);
	}
	if((*tab)[SDLK_LSHIFT] == 1){
		(*tab)[SDLK_LSHIFT] = 0;
		local_speed.increase(0, 0, -1);
	}
	if((*tab)[SDLK_BACKSPACE] == 1){
		(*tab)[SDLK_BACKSPACE] = 0;
		local_speed.to_zero();
	}
}

void Character::rotate_z(double angle){
	empty.rotate_z(angle);
}




/* The position and rotation are modified if the translation/rotation speeds are non-zero,
   or if the mouse has moved. */
void Character::update_position(int**tab, double period){
	
	// cout << "Character::update_position" << endl;

	Vector movement;
	
	if(!(*tab)[SDLK_LCTRL] && !(*tab)[SDLK_RCTRL]){
		rotate_z(-rotate_angle*((double)((*tab)[DX_MOUSE]))/100);
		
		/* Rotate the camera around the center of the character:
		   1) Move the camera to the character;
		   2) Turn the camera around both axes Y and Z;
		   3) Move the camera backwards along its new axis. */

		my_cam->set_position(empty);
		my_cam->rotate_local_y(-rotate_angle * ((double)((*tab)[DY_MOUSE]))/100);
		my_cam->rotate_z(-rotate_angle * ((double)((*tab)[DX_MOUSE]))/100);
		my_cam->pull_from_character();
	}
	(*tab)[DX_MOUSE]=0; /* RAZ. */
	(*tab)[DY_MOUSE]=0; /* RAZ. */


	/* Rotations for debug. */
	// Keypad 1: Front
	if((*tab)[SDLK_KP1]){
		(*tab)[SDLK_KP1] = 0;
		raz_rotation();
	}
	// Keypad 3: Side
	if((*tab)[SDLK_KP3]){
		(*tab)[SDLK_KP3] = 0;
		raz_rotation();
		empty.rotate_z(PI/2);
		my_cam -> rotate_z(PI/2);
	}
	// Keypad 7: Top
	if((*tab)[SDLK_KP7]){
		(*tab)[SDLK_KP7] = 0;
		raz_rotation();
		empty.rotate_local_y(PI/2);
		cout << "my_cam->rotate_local_y(" << PI/2 << ");" << endl;
		my_cam -> rotate_local_y(PI/2);
	}

	// Change distance from cam to character
	if((*tab)[SDLK_KP_PLUS]){
		(*tab)[SDLK_KP_PLUS] = 0;
		my_cam -> multiply_distance_from_target(1/1.1);
	}
	if((*tab)[SDLK_KP_MINUS]){
		(*tab)[SDLK_KP_MINUS] = 0;
		my_cam -> multiply_distance_from_target(1.1);
	}


	convert_speed_local_to_global();
	movement = global_speed.get_scal_product(period);
	empty.translate(movement);
	my_cam->translate(movement);
}


void Character::render(){

	double x = empty.get_origin()->get_x();
	double y = empty.get_origin()->get_y();
	double z = empty.get_origin()->get_z();

	double i;

	glBegin(GL_QUADS);
	for(i=0; i<=5; i++){
		/* Base: Red,
		   Top: Green. */
		glColor3ub(255*(5-i)/5,
			   255*(i/5),
			   0);
		glVertex3d(x+1, y+1, z+i/5);
		glVertex3d(x+1, y-1, z+i/5);
		glVertex3d(x-1, y-1, z+i/5);
		glVertex3d(x-1, y+1, z+i/5);
	}
	glEnd();

	empty.render();
}


/* Exprimer la vitesse du perso dans le repère global en fonction de son expression dans le repère local. */
void Character::convert_speed_local_to_global(){
	
	/* Components of the local velocity. */
	double vxl = local_speed.get_x();
	double vyl = local_speed.get_y();
	double vzl = local_speed.get_z();
	
	double x_target = empty.get_target()->get_x();
	double y_target = empty.get_target()->get_y();
	double z_target = empty.get_target()->get_z();
	
	double x_left = empty.get_left()->get_x();
	double y_left = empty.get_left()->get_y();
	double z_left = empty.get_left()->get_z();
	
	double x_vertic = empty.get_vertic()->get_x();
	double y_vertic = empty.get_vertic()->get_y();
	double z_vertic = empty.get_vertic()->get_z();
	
	/* La vitesse du perso exprimée dans le repère global. */
	double vxg = vxl*x_target + vyl*x_left + vzl*x_vertic;
	double vyg = vxl*y_target + vyl*y_left + vzl*y_vertic;
	double vzg = vxl*z_target + vyl*z_left + vzl*z_vertic;
	
	global_speed = Vector(vxg, vyg, vzg);
	// cout << "Local speed: " << local_speed.to_string() << ", global_speed: " << global_speed.to_string() << endl;
}
/* Exprimer la vitesse du perso dans le repère local en fonction de son expression dans le repère global. */
void Character::convert_speed_global_to_local(){

	/* Components of the global velocity. */
	double vxg = global_speed.get_x();
	double vyg = global_speed.get_y();
	double vzg = global_speed.get_z();
	
	double x_target = empty.get_target()->get_x();
	double y_target = empty.get_target()->get_y();
	double z_target = empty.get_target()->get_z();
	
	double x_left = empty.get_left()->get_x();
	double y_left = empty.get_left()->get_y();
	double z_left = empty.get_left()->get_z();
	
	double x_vertic = empty.get_vertic()->get_x();
	double y_vertic = empty.get_vertic()->get_y();
	double z_vertic = empty.get_vertic()->get_z();
	
	/* La global_speed du perso exprimée dans le repère local. */
	double vxl = vxg*x_target + vyg*y_target + vzg*z_target;
	double vyl = vxg*x_left   + vyg*y_left   + vzg*z_left  ;
	double vzl = vxg*x_vertic + vyg*y_vertic + vzg*z_vertic;
	
	local_speed = Vector(vxl, vyl, vzl);
	cout << "Global speed: " << global_speed.to_string() << ", local_speed: " << local_speed.to_string() << endl;
}



Empty* Character::get_empty(){

	return &empty;
}


void Character::raz_rotation(){

	empty.raz_rotation();
	my_cam->raz_rotation();
}
