

#define TRANSPARENCY_CUBE 0
#define NB_MAX_POINTS 100
#define NB_MAX_FACES 100


struct cube;
typedef struct cube cube;

void calculer_collision_cube_perso(struct cube*c, struct perso*p);
void calculer_base(struct perso*p, struct cube*c, struct vecteur*vect_n, struct vecteur*ux, struct vecteur*uy);
void cube_to_string(struct cube*c);
void compute_attraction_cube_on_cube(struct cube*other, struct cube*target);
void cut_face(face*f, struct cube*c, vector point, vector normal);

void triangulate_cube(cube*c);
void display_uncut_cube(cube*c);
void display_cut_cube(cube*c);
int define_side(vector x, vector origin, vector normal);



struct cube{
	
	/* Centre. */
	float x, y, z;
	
	/* Côté. */
	float taille;
	
	/* Provisoire: rayon de la sphère de collision. */
	float rC;

	/* Local gravity.
	   Only used when the flag use_local_gravity was set in the main program. */
	float gx, gy, gz;
	
	/* Couleur. */
	/* NB: il faudrait que chaque face ait une couleur basée sur la couleur du cube, mais très légèrement différente.
	   Par exemple, on imagine qu'une source de lumière est placée très loin et qu'elle éclaire dans la direction
	   du vecteur {2, 1, -5}. Donc pour chaque cube, la face du haut est très bien éclairée, la face avant l'est un peu moins
	   et la face droite encore moins.
	   Ou alors, on place une source de lumière et on fait du ray tracing sur les sommets des cubes pour savoir comment
	   ils sont éclairés. Ce calcul devra être fait à chaque fois qu'on ajoute ou retire un cube ou une source de lumière.
	*/
	int r, g, b;
	
	/* Un cube peut être existant ou inexistant.
	   NB: à l'avenir, un cube inexistant ne sera pas stocké et le terrain aura une structure modifiable,
	   çad qu'on pourra ajouter ou supprimer des cubes. Pour l'instant on change juste ce paramètre. */
	int existe;
	
	int transparence; /* 0<->opaque, 255<->transparent. */
	
	/* La gravité locale dans tout le cube. */
	struct vecteur gravite;
	
	/* Si le perso heurte le cube en s'en approchant avec une vitesse normale v_in selon un axe donné,
	   alors il repart avec une vitesse v_out qui vaut -v_in*restitution;
	   càd que si restitution vaut 0, le perso s'arrête, si restitution vaut 1 le perso garde toute sa vitesse. */
	float c_rebond;
	
	
	/* Quand le perso glisse contre le cube, sa vitesse tangentielle est modifiée selon la valeur de 'restit_horiz':
	   valeur: effet
	   x<0    : valeurs non autorisées (sinon la vitesse change de sens à chaque calcul...).
	   x=0    : le perso s'arrête instantanément.
	   0<x<1  : le perso est ralenti.
	   x=1    : la vitesse tangentielle du perso n'est pas modifiée.
	   x>1    : le perso est accéléré.
	*/
	float c_frott;
	
	/* Voisins directs. */
	struct cube*voisin_z_plus; /* Le voisin qui est aligné verticalement et dont la cote est plus élevée. */
	struct cube*voisin_z_moins;
	struct cube*voisin_y_plus;
	struct cube*voisin_y_moins;
	struct cube*voisin_x_plus;
	struct cube*voisin_x_moins;
	
	/* Un cube est visible quand au moins l'un de ses six voisins n'existe pas. */
	int est_visible;
	
	struct sphere sphere;

	arrow arrow;

	/* Non-complete cubes:
	   A cube may be cut by one or more planes.
	   When the cube is cut, a specific mesh is memorized, with more vertices and triangular faces. */
	int is_cut;
	int nb_faces;
	int nb_points;
	vector*tab_points;
	face*tab_faces;
};


void init_gravite_cube(struct cube*c, float gx, float gy, float gz){
	if(c!=NULL){
		init_vecteur(&(c->gravite), gx, gy, gz);
	}
}
	
	
void changer_couleur_cube(struct cube*c){
	c->r=random_int_a_b(25, 255);
	c->b=random_int_a_b(25, 255);
	c->g=random_int_a_b(25, 255);
}

void init_cube(struct cube*c, float xP, float yP, float zP, float t, float c_rebond, float c_frott){
	if(c!=NULL){
		c->x=xP;
		c->y=yP;
		c->z=zP;
		c->taille=t;
		c->rC=c->taille/2;
		
		changer_couleur_cube(c);
		
		c->transparence = TRANSPARENCY_CUBE;
	
		c->c_frott=c_frott;
		c->c_rebond=c_rebond;
		
		/* Voisins (aucun lien au départ). */
		c->voisin_x_plus=NULL;
		c->voisin_x_moins=NULL;
		c->voisin_y_plus=NULL;
		c->voisin_y_moins=NULL;
		c->voisin_z_plus=NULL;
		c->voisin_z_moins=NULL;
		
		/* On crée la sphère liée au cube. */
		init_sphere(&(c->sphere),
			    xP, yP, zP,
			    t/2,
			    0);
		c->is_cut = 0;
	}
}


void afficher_cube(struct cube*c){
	
	/* Coordonnées des points situés en haut du cube, en bas,
	   à gauche, à droite, sur la face avant, sur la face arrière. */

	if(c!=NULL){

		/* Display arrow of empty cubes. */
		display_arrow(&(c->arrow));
		

	
		/* On affiche le cube seulement s'il n'est pas déclaré comme inexistant. */
		if(c->existe){
			if(c->est_visible){

				if(c->is_cut){
					display_cut_cube(c);
				}
				else{
					display_uncut_cube(c);
				}
			}
		}
	}
}

void display_uncut_cube(cube*c){
				
	/* Provisoire: on affiche la sphère de collision. */
	/* glColor4ub(c->r, c->g, c->b, c->transparence);
	   afficher_sphere(&(c->sphere));
	   return; */
	
	/* Code "normal": on affiche vraiment le cube. */
	/* NB: Pour accélérer l'étape d'affichage, il faudra que chaque cube sache quelles sont
	   les faces qui sont visibles; à chaque affichage, on ne traitera que celles-là.
	   Donc à chaque modification d'un cube du terrain (ajout ou retrait), il faudra
	   mettre à jour ces flags parmi tous les cubes voisins directs. */

	float h, b, g, d, av, ar;
	float demi_hauteur=c->taille/2;

	h=c->z+demi_hauteur;
	b=c->z-demi_hauteur;
	d=c->y+demi_hauteur;
	g=c->y-demi_hauteur;
	ar=c->x+demi_hauteur;
	av=c->x-demi_hauteur;
       
	glBegin(GL_QUADS);{
			
		/* Couleur du cube. */
		/* glColor3ub(c->r, c->g, c->b); */
		glColor4ub(c->r, c->g, c->b, c->transparence);
				
		// Face de droite.
		glVertex3d(av, d, h);
		glVertex3d(av, d, b);
		glVertex3d(ar, d, b);
		glVertex3d(ar, d, h);
		
		// Face de gauche.
		glVertex3d(av, g, h);
		glVertex3d(av, g, b);
		glVertex3d(ar, g, b);
		glVertex3d(ar, g, h);
				
		// Face du haut.
		glVertex3d(av, d, h);
		glVertex3d(av, g, h);
		glVertex3d(ar, g, h);
		glVertex3d(ar, d, h);
				
		// Face du bas.
		glVertex3d(av, d, b);
		glVertex3d(av, g, b);
		glVertex3d(ar, g, b);
		glVertex3d(ar, d, b);
				
		// Face avant.
		glVertex3d(av, d, b);
		glVertex3d(av, g, b);
		glVertex3d(av, g, h);
		glVertex3d(av, d, h);
				
		// Face arrière.
		glVertex3d(ar, d, b);
		glVertex3d(ar, g, b);
		glVertex3d(ar, g, h);
		glVertex3d(ar, d, h);
				
	}
	glEnd();
}


/* Longueur de superposition sur chacun des trois axes.
   Si sur un axe donné, le cube et le perso ne se superposent pas, alors la valeur pour cet axe est mise à zéro.
   S'il y a effectivement collision, le dernier paramètre ("collision") est mis à 1.
*/
void calculer_depassements(struct cube*c, struct perso*p, float*dx, float*dy, float*dz, int*collision){
	
	float xc=c->x;
	float yc=c->y;
	float zc=c->z;
	float xp=p->empty.pos.x;
	float yp=p->empty.pos.y;
	float zp=p->empty.pos.z;
	float hp=p->hauteur;
	float lp=p->longueur;
	float t=c->taille;
	
	*collision=0;
	*dx=0;
	*dy=0;
	*dz=0;
		
	/******************************/
	/* Dépassement selon l'axe X. */
	/******************************/
	
	if(xp>xc){
		/* Perso derrière le cube. */
		if(xp-xc<t/2+lp/2){
			/* Superposition horizontale du cube et du perso. */
			(*dx)=(t/2+lp/2) - (xp-xc);
		}
	}
	else{
		/* Perso devant le cube. */
		if(xc-xp<t/2+lp/2){
			/* Superposition verticale du cube et du perso. */
			(*dx)=(t/2+lp/2) - (xc-xp);
		}
	}	
	
	
	/******************************/
	/* Dépassement selon l'axe Y. */
	/******************************/
	
	if(yp>yc){
		/* Perso à gauche du cube. */
		if(yp-yc<t/2+lp/2){
			/* Superposition horizontale du cube et du perso. */
			(*dy)=(t/2+lp/2) - (yp-yc);
		}
	}
	else{
		/* Perso en-dessous du cube. */
		if(yc-yp<t/2+lp/2){
			/* Superposition verticale du cube et du perso. */
			(*dy)=(t/2+lp/2) - (yc-yp);
		}
	}	
	
	
	/******************************/
	/* Dépassement selon l'axe Z. */
	/******************************/
	
	if(zp>zc){
		/* Perso au-dessus du cube et pas de cube au-dessus du cube. */
		if(zp-zc<t/2+hp/2){
			/* Superposition verticale du cube et du perso. */
			(*dz)=(t/2+hp/2) - (zp-zc);
		}
	}
	else{
		/* Perso en-dessous du cube et pas de voisin en-dessous du cube. */
		if(zc-zp<t/2+hp/2){
			/* Superposition verticale du cube et du perso. */
			(*dz)=(t/2+hp/2) - (zc-zp);
		}
	}
	/* printf("dx=%f, dy=%f dz=%f\n", *dx, *dy, *dz); */
	if((*dx)*(*dy)*(*dz)>0){
		*collision=1;
	}
}



void appliquer_forces_cube_sur_perso(struct cube*c, struct perso*p, float dt){
	
	/* Dans la version finale, on associera à chaque cube une force par
	   unité de volume.
	   Par exemple, si le perso a seulement 10% de son volume dans le cube,
	   alors ce cube appliquera sur le perso une force d'intensité 10 fois
	   plus faible que si le perso était à 100% dans le cube.
	   
	   En attendant la version finale, aucun cube n'exerce directement une force sur le perso,
	   c'est le terrain global qui s'en charge.
	*/
	
	
	
}








void _calculer_collision_cube_perso(struct cube*c, struct perso*p){
	if(p!=NULL && c!=NULL){
		
		float dx=p->empty.pos.x-c->x;
		float dy=p->empty.pos.y-c->y;
		float dz=p->empty.pos.z-c->z;
		float dCP=sqrt(dx*dx+dy*dy+dz*dz);
	
		/* Rayons des deux sphères d'influence. */
		float rP=p->rP;
		float rC=c->rC;
	
		struct vecteur vect_n;
		struct vecteur ux;
		struct vecteur uy;
		
		
		if(dCP<rP+rC){
			/* Alors on a collision.
			   Il faut vérifier la vitesse du perso. */
		
			/* Calcul du vecteur normal. */
			init_vecteur(&vect_n, dx, dy, dz);
			normer_vecteur(&vect_n, &vect_n);

			/* On veut savoir si le perso se rapproche du cube. */
			if(produit_scalaire(p->vitesse, vect_n)<0){
				/* Ici le perso se rapproche du cube, donc il y a collision. */
			
				/* ÉTAPE 1) PLACER LE PERSO AU CONTACT AVEC LA SPHÈRE, PAR DICHOTOMIE. */
				placer_perso_au_contact(p, c);
			
				/* ÉTAPE 2) CALCULER LA BASE (vect_n, ux, uy). */
				calculer_base(p, c, &vect_n, &ux, &uy);
			
				/* ÉTAPE 3) MODIFIER LA VITESSE DU PERSO. */
				calculer_rebond_perso(p, &vect_n, &ux, c->c_rebond, c->c_frott);
				
			}
			/* Sinon, ça veut dire que le perso s'éloigne du cube, donc on ne modifie pas sa vitesse. */
		}
	}
}



/* Calculer les vecteurs vect_n, ux, uy en fonction du cube et du perso. */
void calculer_base(struct perso*p, struct cube*c, struct vecteur*vect_n, struct vecteur*ux, struct vecteur*uy){
	
	float dx=p->empty.pos.x-c->x;
	float dy=p->empty.pos.y-c->y;
	float dz=p->empty.pos.z-c->z;
	
	/* Calcul du vecteur normal. */
	init_vecteur(vect_n, dx, dy, dz);
	normer_vecteur(vect_n, vect_n);
	
	/* Uy = n^v */
	produit_vectoriel(*vect_n, p->vitesse, uy);
	
	/* Ux  = uy^n */
	produit_vectoriel(*uy, *vect_n, ux);
	
	/* On norme ux et uy, s'ils sont non nuls. */
	normer_vecteur(ux, ux);
	normer_vecteur(uy, uy);
}




/* On calcule la collision entre le cube du terrain (fixe) et le cube d'influence du perso (mobile).
   NB: en fonction des voisins du cube, la réaction du cube pourra être modifiée.
*/

void calculer_collision_cube_perso(struct cube*c, struct perso*p){
	if(c!=NULL && p!=NULL && c->existe){
		
		float dx, dy, dz;
		int collision;
		/* Après le choc, on replace le perso au contact avec le cube. */
		float correction_x=0;
		float correction_y=0;
		float correction_z=0;
		
		
		/* On calcule les dépassements sur les 3 axes entre le perso et le cube. */
		/* void calculer_depassements(struct cube*c, struct perso*p, float*dx, float*dy, float*dz, int*collision); */
		
		calculer_depassements(c, p, &dx, &dy, &dz, &collision);
		if(collision){
			/* printf("Welding à faire...\n"); */
			
			if(dz>0 && dz<=dx && dz<=dy){
				/* Réaction sur l'axe Z. */
				
				/* Changer la vitesse si le perso s'approche du cube. */
				if((p->empty.pos.z>c->z && p->vitesse.z<0) || (p->empty.pos.z<c->z && p->vitesse.z>0)){
					p->vitesse.z = -p->vitesse.z * c->c_rebond;
					p->vitesse.x = p->vitesse.x*c->c_frott;
					p->vitesse.y = p->vitesse.y*c->c_frott;
				}
				if(p->empty.pos.z<c->z){
					correction_z=-dz;
				}
				else{
					correction_z=dz;
				}
			}
			else if(dy>0 && dy<=dx && dy<=dz){
				/* Réaction sur l'axe Y. */
				printf("réac Y: vx=%f, vy=%f\n", p->vitesse.x, p->vitesse.y);
				
				/* Changer la vitesse si le perso s'approche du cube. */
				if((p->empty.pos.y>c->y && p->vitesse.y<0) || (p->empty.pos.y<c->y && p->vitesse.y>0)){
					p->vitesse.y = -p->vitesse.y * c->c_rebond;
					/* p->vitesse.x = p->vitesse.x*c->c_frott;
					   p->vitesse.z = p->vitesse.z*c->c_frott; */
				}
				if(p->empty.pos.y<c->y){
					correction_y=-dy;
				}
				else{
					correction_y=dy;
				}
			}
			else if(dx>0 && dx<=dy && dx<=dz){
				/* Réaction sur l'axe X. */
				printf("réac X: vx=%f, vy=%f\n", p->vitesse.x, p->vitesse.y);
				
				/* Changer la vitesse si le perso s'approche du cube. */
				if((p->empty.pos.x>c->x && p->vitesse.x<0) || (p->empty.pos.x<c->x && p->vitesse.x>0)){
					p->vitesse.x = -p->vitesse.x * c->c_rebond;
					/* p->vitesse.y = p->vitesse.y*c->c_frott;
					   p->vitesse.z = p->vitesse.z*c->c_frott; */
				}
				if(p->empty.pos.x<c->x){
					correction_x=-dx;
				}
				else{
					correction_x=dx;
				}
			}
			
			/* Replacer le perso au contact.
			   NB: la caméra subit automatiquement le même mouvement. */
			deplacer_perso_global(p, correction_x, correction_y, correction_z);
		}
	}
}



/* Au début du jeu et dès qu'on modifie l'un des voisins du cube,
   on re-calcule le booléen est_visible.
   Le cube sera invisible si tous ses six voisins existent simultanément.
*/
void calculer_visibilite(struct cube*c){
	if(c!=NULL){
		if(c->voisin_x_plus!=NULL && c->voisin_x_plus->existe
		   && c->voisin_x_moins!=NULL && c->voisin_x_moins->existe
		   && c->voisin_y_plus!=NULL && c->voisin_y_plus->existe
		   && c->voisin_y_moins!=NULL && c->voisin_y_moins->existe
		   && c->voisin_z_plus!=NULL && c->voisin_z_plus->existe
		   && c->voisin_z_moins!=NULL && c->voisin_z_moins->existe
		   ){
			/* Tous les voisins sont opaques, donc on ne voit pas le cube paramètre. */
			c->est_visible=0;
		}
		else{
			c->est_visible=1;
		}
	}
}



void cube_to_string(struct cube*c){
	
	printf("cube{\n\tcentre: {%f, %f, %f}, taille=%f\n}\n", c->x, c->y, c->z, c->taille);
}



/* Add the influence of source to the target.
   All cubes have the same weight.
*/
void compute_attraction_cube_on_cube(struct cube*source, struct cube*target){
	float g = 1; /* Gravitational constant. */
	float dx = source->x - target->x;
	float dy = source->y - target->y;
	float dz = source->z - target->z;
	float distance = sqrt(dx*dx + dy*dy + dz*dz);
	float val;

	if(distance != 0){
		val = g/(distance*distance*distance);

		target->gx = target->gx + val*dx;
		target->gy = target->gy + val*dy;
		target->gz = target->gz + val*dz;
		//printf("target: %f, %f, %f\n", target->gx, target->gy, target->gz);
	}
	/* A cube has no gravitational influence on itself. */
}



/* Detect a point inside the cube;
   returns true if the point is inside the cube;
   returns false otherwise. */
int is_point_in_cube(vector point, struct cube*c){
	
	if(c->is_cut){
		// Special treatment with the face list.

		return 0;
	}
	else{
		// Check the center of the cube plus/minus the half-size.
		return 0;
	}
}





/* Create a mesh where each face of the cube is represented by two triangles. */
void triangulate_cube(cube*c){

	/* nb:
	   int nb_faces;
	   int nb_points;
	   vector*tab_points;
	   face*face_list;

	   x, y, z, taille;
	*/
	float t2 = c->taille / 2;

	if(c->tab_points != NULL){
		printf("cube is already meshed.\n");
		return;
	}
	if(c->tab_faces != NULL){
		printf("cube already has faces.\n");
	}

	/* Usual situation. */
	c->is_cut = 1;
	c->nb_points = 8;
	c->nb_faces = 12;
	c->tab_points = (vector*)malloc(NB_MAX_POINTS*sizeof(vector));
	c->tab_faces = (face*)malloc(NB_MAX_FACES*sizeof(face));

	/* Initialization of the points and the faces. */
	init_vecteur(&(c->tab_points[0]), c->x - t2, c->y - t2, c->z - t2);
	init_vecteur(&(c->tab_points[1]), c->x - t2, c->y - t2, c->z + t2);
	init_vecteur(&(c->tab_points[2]), c->x + t2, c->y - t2, c->z - t2);
	init_vecteur(&(c->tab_points[3]), c->x + t2, c->y - t2, c->z + t2);
	init_vecteur(&(c->tab_points[4]), c->x - t2, c->y + t2, c->z - t2);
	init_vecteur(&(c->tab_points[5]), c->x - t2, c->y + t2, c->z + t2);
	init_vecteur(&(c->tab_points[6]), c->x + t2, c->y + t2, c->z - t2);
	init_vecteur(&(c->tab_points[7]), c->x + t2, c->y + t2, c->z + t2);

	init_face(&(c->tab_faces[0]), &(c->tab_points[0]), &(c->tab_points[1]), &(c->tab_points[2]));
	init_face(&(c->tab_faces[1]), &(c->tab_points[1]), &(c->tab_points[2]), &(c->tab_points[3]));
	
	init_face(&(c->tab_faces[2]), &(c->tab_points[1]), &(c->tab_points[5]), &(c->tab_points[7]));
	init_face(&(c->tab_faces[3]), &(c->tab_points[1]), &(c->tab_points[3]), &(c->tab_points[7]));
													      
	init_face(&(c->tab_faces[4]), &(c->tab_points[2]), &(c->tab_points[3]), &(c->tab_points[7]));
	init_face(&(c->tab_faces[5]), &(c->tab_points[2]), &(c->tab_points[6]), &(c->tab_points[7]));
    
	init_face(&(c->tab_faces[6]), &(c->tab_points[0]), &(c->tab_points[1]), &(c->tab_points[5]));
	init_face(&(c->tab_faces[7]), &(c->tab_points[0]), &(c->tab_points[4]), &(c->tab_points[5]));

	init_face(&(c->tab_faces[8]), &(c->tab_points[4]), &(c->tab_points[5]), &(c->tab_points[6]));
	init_face(&(c->tab_faces[9]), &(c->tab_points[5]), &(c->tab_points[6]), &(c->tab_points[7]));

	init_face(&(c->tab_faces[10]), &(c->tab_points[0]), &(c->tab_points[4]), &(c->tab_points[6]));
	init_face(&(c->tab_faces[11]), &(c->tab_points[0]), &(c->tab_points[2]), &(c->tab_points[6]));

	return;
}

/* Get a randomized value close to the parameter but not over 255 and not under 0. */
int randomize_test(int x){
	return max(0, min(255, random_int_a_b(x-15, x+15)));
}


/* Just a test to get a slightly different color. */
void set_random_color_test(int r, int g, int b, int transp){
	r = randomize_test(r);
	g = randomize_test(g);
	b = randomize_test(b);
	glColor4ub(r, g, b, transp);
}


/* When the cube was split in twelve or more triangular faces, we must display these triangles.
   ! This function is to be used only for such cubes!!! */
void display_cut_cube(cube*c){
	
	int i;
	vector*p;	

	
	glBegin(GL_TRIANGLES);
	glColor4ub(c->r, c->g, c->b, c->transparence);
	
	for(i=0; i<c->nb_faces; i++){
		
		/* Just a test to get a slightly different color. */
		set_random_color_test(c->r, c->g, c->b, c->transparence);
		
		p = c->tab_faces[i].a;
		glVertex3d(p->x, p->y, p->z);

		p = c->tab_faces[i].b;
		glVertex3d(p->x, p->y, p->z);

		p = c->tab_faces[i].c;
		glVertex3d(p->x, p->y, p->z);
	}

	glEnd();
	
	return;
}






/* Cut a cube with a specific plane.
   If the specified point is included in the cube, then we slice the cube with the plane
   containing that point and defined by the normal vector.
   We keep the part of the cube that is located in the direction of the vector.
*/

void cut_cube(struct cube*c, vector point, vector normal){

	int i_face;

	/* Here we must actually cut the cube. */
	for(i_face=0; i_face<c->nb_faces; i_face++){
		
		/* Cut the face, add the new points to the cube or re-use the existing points. */
		cut_face(&(c->tab_faces[i_face]), c, point, normal);
	}
}


/* We want to know the situation of the point x relatively to the plane defined by the pair (origin, normal):
   x may be:
   *  in the half-space on the side of the normal, or
   *  in the plane, or
   *  in the other half-space;
   The function returns respectively the values +1, 0 or -1.

   A point is detected inside the plane with a precision of epsilon.
*/
int define_side(vector x, vector origin, vector normal){
	
	/* We compute the vector that goes from the origin of the plane to the considered x point,
	   and then the scalar product of that vector to the normal of the plane;
	   if that scalar product is positive, then the point is on the right side;
	   if zero, the point is in the plane;
	   if negative, on the left side.
	*/
	
	float epsilon = 0.001;
	
	vector diff;
	float scal;
	diff_vecteurs(x, origin, &diff);
	
	scal = produit_scalaire(diff, normal);

	if(scal < epsilon && scal > -epsilon){
		/* In the plane. */
		return 0;
	}
	if(scal > 0){
		/* Same side as the normal. */
		return +1;
	}
	if(scal < 0){
		/* Other side as the normal. */
		return -1;
	}

}



/* The plane is defined by the point and the vector. */
void cut_face_old_version(face*f, struct cube*c, vector origin_plane, vector normal){

	/* First we must determine on which side of the plane each summits are. */
	int side_a, side_b, side_c;

	/* The potential new points created when cutting the face.
	   NB: these local variables are only pointers to the data, which are written in the table within the cube.
	 */
	vector*point_d;
	vector*point_e;

	/* Error cases: if the plane is not properly defined, the cube is not modified. */
	if(est_vecteur_nul(&normal)){
		return;
	}

	/* The points may be either on one side of the plan, or on the other side, or exactly included in the plan.
	 */	   
	side_a = define_side(*(f->a), origin_plane, normal);
	side_b = define_side(*(f->b), origin_plane, normal);
	side_c = define_side(*(f->c), origin_plane, normal);
	
	/* Different situations are possible.
	   Everytime a segment is cut, a new point is added to the mesh. We call these points D and E, when they exist.
	   D is created when cutting the first segment, and E when cutting the second one.
	 */
	
	if(side_a < 0){
		if(side_b < 0){
			if(side_c > 0){
				/* [AC] [BC] */
				/* Add the new points to the mesh. */
				point_d = &(c->tab_points[c->nb_points]);
				point_e = &(c->tab_points[c->nb_points+1]);
				c->nb_points = c->nb_points+2;
				/* Compute the coordinates of D and E, which are the intersections of [AC] and [BC] with the cutting plane. */
				intersection(f->a, f->c, &origin_plane, &normal, point_d);
				intersection(f->b, f->c, &origin_plane, &normal, point_e);
				/* Add faces (ABD) and (BED). */
				init_face(&(c->tab_faces[c->nb_faces]), f->a, f->b, point_d);
				init_face(&(c->tab_faces[c->nb_faces+1]), f->b, point_e, point_d);
				c->nb_faces = c->nb_faces+2;
				/* Replace the face (ABC) with (DEC). */
				f->a = point_d;
				f->b = point_e;
			}
		}
		else if(side_b == 0){
			if(side_c > 0){
				/* [AC] */
			}
		}
		else{
			if(side_c < 0){
				/* [AB] [BC] */
			}
			else if(side_c == 0){
				/* [AB] */
			}
			else{
				/* [AB] [AC] */
			}
		}
	}
	else if(side_a==0){
		if(side_b != 0){
			if(side_b != side_c && side_c != 0){
				/* [BC] */
			}
		}
	}
	else{
		if(side_b < 0){
			if(side_c < 0){
				/* [AB] [AC] */
			}
			else if(side_c == 0){
				/* [AB] */
			}
			else{
				/* [AB] [BC] */
			}
		}
		else if(side_b == 0){
			if(side_c < 0){
				/* [AC] */
			}
		}
		else{
			if(side_c < 0){
				/* [AC] [BC] */
			}
		}
	}




	// printf("cube: TODO\n");
	return;

}





/* The plane is defined by the point and the vector. */
void cut_face(face*f, struct cube*c, vector origin_plane, vector normal){
	
	
	
	
}
