#include "Empty.hpp"


Empty::Empty() : origin(0, 0, 0), target(1, 0, 0), left(0, 1, 0), vertic(0, 0, 1), angle_z(0)
{
	
}

// Empty::Empty(const Empty&e): 
	

string Empty::to_string(){
	return ("Empty: {"
		+ origin.to_string() + " ; "
		+ target.to_string() + " ; "
		+ left.to_string() + " ; "
		+ vertic.to_string() + "}");
}

void Empty::print(){
	cout << to_string() << endl;
}

Vector*Empty::get_origin(){
	return &origin;
}

Vector*Empty::get_target(){
	return &target;
}

Vector*Empty::get_left(){
	return &left;
}

Vector*Empty::get_vertic(){
	return &vertic;
}

void Empty::set_origin(Vector param){
	cout << "Before set_origin: " << param.to_string() << ", " << origin.to_string() << endl;
	origin = param;
	cout << "After set_origin: " << param.to_string() << ", " << origin.to_string() << endl;
}

void Empty::set_origin(Empty param){
	origin = param.origin;
}


void Empty::rotate_z(double angle){

	// origin.rotate_z(angle);
	target.rotate_z(angle);
	left.rotate_z(angle);
	vertic.rotate_z(angle);

	angle_z += angle;
}

void Empty::rotate_local_y(double angle){

	double saved_angle = angle_z;
	
	if(angle != 0){
		cout << "Empty::rotate_local_y(" << angle << ");" << endl;
	}

	/* Preparation: momentarily reset the angle_z. */
	rotate_z(-saved_angle);

	target.rotate_y(angle);
	left.rotate_y(angle);
	vertic.rotate_y(angle);

	rotate_z(+saved_angle);
}


void Empty::translate(Vector v){
	origin.increase(v);
}


void Empty::push_forward(double distance){
	origin.increase(target.get_normalized().get_scal_product(distance));
}


void Empty::raz_rotation(){
	target = Vector(1, 0, 0);
	left = Vector(0, 1, 0);
	vertic = Vector(0, 0, 1);
	angle_z = 0;
}


void Empty::render(){
	Arrow targ_arrow(origin, target);
	Arrow left_arrow(origin, left);
	Arrow vertic_arrow(origin, vertic);
	targ_arrow.render();
	left_arrow.render();
	vertic_arrow.render();
}
