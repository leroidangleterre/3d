#ifndef VECTOR
#define VECTOR

#include <iostream>
#include <string>
#include <cmath>

using namespace std;


class Vector
{
	
public:

	Vector();
	Vector(double xx, double yy, double zz);
	Vector(const Vector*v);
	void afficherEtat();
	string to_string();
	
	double get_x();
	double get_y();
	double get_z();

	void set_x(double x_param);
	void set_y(double y_param);
	void set_z(double z_param);

	void rotate_z(double angle);
	void rotate_y(double angle);

	void increase(Vector v);
	void increase(double dx, double dy, double dz);
	void to_zero();

	double dot_prod(Vector v);
	Vector vect_product(Vector other);

	Vector diff(Vector d);

	void scal_product(double fact);
	Vector get_scal_product(double fact);
	double get_norm();
	Vector get_normalized();
	void normalize();
	
private:
	
	double x;
	double y;
	double z;
};

#endif
