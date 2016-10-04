/* Modifier aléatoirement la composante initiale, pour obtenir une valeur comprise entre 0 et 255, et peu éloignée
   de la valeur initiale. */
int calc_comp(int val){
	return max(0, min(255, val+random_int_a_b(-10, 10)));
}

void afficher_diamant(float x, float y , float z, float taille, float transparence, struct vecteur*couleur){
	
	int r, g, b;
	if(couleur==NULL){
		r=random_int_a_b(0, 255);
		g=random_int_a_b(0, 255);
		b=random_int_a_b(0, 255);
	}
	else{
		r=couleur->x;
		g=couleur->y;
		b=couleur->z;
	}
	
	glBegin(GL_TRIANGLES);
	
	glColor4ub(calc_comp(r), calc_comp(g), calc_comp(b), transparence);
	glVertex3d(x+taille, y, z);
	glVertex3d(x, y+taille, z);
	glVertex3d(x, y, z+taille);
	
	glColor4ub(calc_comp(r), calc_comp(g), calc_comp(b), transparence);
	glVertex3d(x-taille, y, z);
	glVertex3d(x, y+taille, z);
	glVertex3d(x, y, z+taille);
	
	glColor4ub(calc_comp(r), calc_comp(g), calc_comp(b), transparence);
	glVertex3d(x-taille, y, z);
	glVertex3d(x, y-taille, z);
	glVertex3d(x, y, z+taille);
	
	glColor4ub(calc_comp(r), calc_comp(g), calc_comp(b), transparence);
	glVertex3d(x+taille, y, z);
	glVertex3d(x, y-taille, z);
	glVertex3d(x, y, z+taille);
	
	glColor4ub(calc_comp(r), calc_comp(g), calc_comp(b), transparence);
	glVertex3d(x+taille, y, z);
	glVertex3d(x, y+taille, z);
	glVertex3d(x, y, z-taille);
	
	glColor4ub(calc_comp(r), calc_comp(g), calc_comp(b), transparence);
	glVertex3d(x-taille, y, z);
	glVertex3d(x, y+taille, z);
	glVertex3d(x, y, z-taille);
	
	glColor4ub(calc_comp(r), calc_comp(g), calc_comp(b), transparence);
	glVertex3d(x-taille, y, z);
	glVertex3d(x, y-taille, z);
	glVertex3d(x, y, z-taille);
	
	glColor4ub(calc_comp(r), calc_comp(g), calc_comp(b), transparence);
	glVertex3d(x+taille, y, z);
	glVertex3d(x, y-taille, z);
	glVertex3d(x, y, z-taille);
	
	glEnd();
}
