/* Triangular face of a mesh (used to create a cube). */

typedef struct face{
	vector*a;
	vector*b;
	vector*c;
}face;


void init_face(face*f, vector*a, vector*b, vector*c){
	f->a = a;
	f->b = b;
	f->c = c;
}
