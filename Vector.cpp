#include "Vector.hpp"


Vector::Vector() : x{0.0}, y{0.0}, z{0.0}
{
	
}


Vector::Vector(double xx, double yy, double zz):
	x{xx}, y{yy}, z{zz}
	 {
	 }

void Vector::afficherEtat(){
	cout << "Vector {" << x << ", " << y << ", " << z << "}" << endl;
}

string Vector::to_string(){
	return "Vector{"
		+ std::to_string(x) + ", "
		+ std::to_string(y) + ", "
		+ std::to_string(z) + "}";
}

double Vector::get_x(){
	return x;
}
double Vector::get_y(){
	return y;
}
double Vector::get_z(){
	return z;
}
void Vector::set_x(double x_param){
	x = x_param;
}
void Vector::set_y(double y_param){
	y = y_param;
}
void Vector::set_z(double z_param){
	z = z_param;
}



void Vector::rotate_z(double angle){

	double new_x = x*cos(angle) - y*sin(angle);
	double new_y = x*sin(angle) + y*cos(angle);
	
	x = new_x;
	y = new_y;
}

void Vector::rotate_y(double angle){

	double new_x = x*cos(angle) - z*sin(angle);
	double new_z = x*sin(angle) + z*cos(angle);
	
	x = new_x;
	z = new_z;
}


void Vector::increase(double dx, double dy, double dz){

	x += dx;
	y += dy;
	z += dz;
}

void Vector::increase(Vector v){
	increase(v.x, v.y, v.z);
}


/* Sets all components of the vector to zero. */
void Vector::to_zero(){
	x = 0;
	y = 0;
	z = 0;
}


double Vector::dot_prod(Vector v){

	return x*v.x + y*v.y + z*v.z;
}


Vector Vector::scal_product(double fact){
	Vector prod(fact*x, fact*y, fact*z);
	return prod;
}

double Vector::get_norm(){
	return sqrt(x*x + y*y + z*z);
}

Vector Vector::normalize(){
	double norm = get_norm();

	Vector res;
	
	if(norm == 0){
		x = 0;
		y = 0;
		z = 0;
	}
	else{
		x = x / norm;
		y = y / norm;
		z = z / norm;
	}

	res.set_x(x);
	res.set_y(y);
	res.set_z(z);

	return res;
}
