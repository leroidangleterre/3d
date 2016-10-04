/* This code is used to display an arrow in a 3-dimensional space. */


typedef struct arrow{
	vector pos;
	vector heading;
	
	int r, g, b;
	
	float scale;

}arrow;

void display_triangle(vector*a, vector*b, vector*c);

void init_arrow(arrow*a,
		float x, float y, float z,
		float dx, float dy, float dz){

	// printf("init_arrow\n");

	init_vecteur(&(a->pos), x, y, z);
	init_vecteur(&(a->heading), dx, dy, dz);
	

	a->r = 255;
	a->g = 255;
	a->b = 255;

	a->scale = 1;
}


/* Set the display scale of the arrow. */
void set_scale_arrow(arrow*a, float new_scale){

	mult_vect_scal(&(a->heading), new_scale/a->scale, &(a->heading));
	a->scale = new_scale;
}


void display_arrow(arrow*arrow){

	/* Points o and h are located at the origin and the head of the arrow.
	   The four points a, b, c, d are the base of the pyramid
	   and are located on a square in the plane that contains o and that has (oh) as a normal line.
	*/

	vector unit;
	vector*heading = &(arrow->heading);
	vector target;
	vector ortho1;
	vector ortho2;

	vector o, h, a, b, c, d;

	/* Width of the base of the arrow. */
	float coef = 0.02;
	
	// printf("display arrow\n");

	init_vecteur(&unit, 1, 0, 0);


	produit_vectoriel(*heading, unit, &ortho1);
	produit_vectoriel(ortho1, *heading, &ortho2);
	normer_vecteur(&ortho1, &ortho1);
	normer_vecteur(&ortho2, &ortho2);
	mult_vect_scal(&ortho1, coef, &ortho1);
	mult_vect_scal(&ortho2, coef, &ortho2);


	somme_vecteurs(arrow->pos, ortho1, &a);
	somme_vecteurs(arrow->pos, ortho2, &b);
	diff_vecteurs(arrow->pos, ortho1, &c);
	diff_vecteurs(arrow->pos, ortho2, &d);
	somme_vecteurs(arrow->pos, arrow->heading, &target);

	glColor4ub(255, 0, 0, 127);
	display_triangle(&a, &c, &target);
	glColor4ub(255, 255, 0, 127);
	display_triangle(&b, &d, &target);
}


void display_triangle(vector*a, vector*b, vector*c){

	glBegin(GL_TRIANGLES);{
		glVertex3d(a->x, a->y, a->z);
		glVertex3d(b->x, b->y, b->z);
		glVertex3d(c->x, c->y, c->z);
	}
	glEnd();
}
