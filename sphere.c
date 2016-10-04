/* Sphère:
   
   On commence par un tétraèdre, puis on répète l'opération d'affinage.
   
   Affinage:
   1) On divise chaque face triangulaire;
   2) On déplace chaque centre pour le placer sur la sphère.
   
   
   Affichage: chaque triangle mémorisera son barycentre,
   et on affichera en premier les triangles les plus éloignés de la
   caméra. Nécessaire (je crois...) si on veut des sphères transparentes.
*/

struct sphere{
	struct vecteur centre;
	float r;
	
	int nb_points;
	struct vecteur*tab_points;
	struct vecteur*tab_couleurs; /* Chaque vecteur mémorise les valeurs des composantes r, b et g pour le point associé. */
	
	/* Chaque triangle est mémorisé sous la forme d'un vecteur qui contient les indices des trois sommets du triangle. */
	struct vecteur*tab_faces;
	int nb_faces;
	
	/* Au cours des subdivisions successives, on ajoute des sommets et des faces,
	   et on les compte. */
	int nb_points_crees;
	int nb_faces_creees;
};


void init_sphere(struct sphere*s, float xp, float yp, float zp, float rp, int nb_affinages);
void normaliser_sphere(struct sphere*s);
int trouver_ppv(struct sphere*s, int k);
void borner_couleurs(struct sphere*s);
void subdiviser(struct sphere*s);


/* La sphère doit déjà avoir été allouée;
   cette fonction n'alloue pas de mémoire.
   
   La valeur n détermine le nombre de fois qu'on appliquera l'opération d'affinage à partir du tétraèdre initial.
*/
void init_sphere(struct sphere*s, float xp, float yp, float zp,
		 float rp,
		 int nb_affinages){
	
	

	int i;
	
	init_vecteur(&(s->centre), xp, yp, zp);
		     
	s->r=rp;
	s->nb_faces=8*puissance(4, nb_affinages);
	s->nb_points=6+4*(puissance(4, nb_affinages)-1);
	
	s->tab_points=(struct vecteur*)malloc(s->nb_points*sizeof(struct vecteur));
	s->tab_couleurs=(struct vecteur*)malloc(s->nb_points*sizeof(struct vecteur));
	s->tab_faces=(struct vecteur*)malloc(s->nb_faces*sizeof(struct vecteur)); 
	
	
	
	/* Centre de la sphère. */
	init_vecteur(&(s->tab_points[0]), xp, yp, zp);
	
	/* Les sommets de l'octaèdre. */
	init_vecteur(&(s->tab_points[0]), xp, yp, zp-rp);
	init_vecteur(&(s->tab_points[1]), xp, yp, zp+rp);
	init_vecteur(&(s->tab_points[2]), xp, yp-rp, zp);
	init_vecteur(&(s->tab_points[3]), xp, yp+rp, zp);
	init_vecteur(&(s->tab_points[4]), xp-rp, yp, zp);
	init_vecteur(&(s->tab_points[5]), xp+rp, yp, zp);
	
	/* Couleurs des sommets. */
	for(i=0; i<6; i++){
		init_vecteur(&(s->tab_couleurs[i]), random_int_a_b(0, 255), random_int_a_b(0, 255), random_int_a_b(0, 255));
	}
	s->nb_points_crees=6;
	borner_couleurs(s);
	
	/* Les triangles. */
	init_vecteur(&(s->tab_faces[0]), 0, 2, 5);
	init_vecteur(&(s->tab_faces[1]), 1, 2, 5);
	init_vecteur(&(s->tab_faces[2]), 0, 5, 3);
	init_vecteur(&(s->tab_faces[3]), 1, 5, 3);
	init_vecteur(&(s->tab_faces[4]), 0, 3 ,4);
	init_vecteur(&(s->tab_faces[5]), 1, 3, 4);
	init_vecteur(&(s->tab_faces[6]), 0, 4, 2);
	init_vecteur(&(s->tab_faces[7]), 1, 4, 2);
	s->nb_faces_creees=8;
	
	
	for(i=0; i<nb_affinages; i++){
		subdiviser(s);
	}
	
}
	



void afficher_sphere(struct sphere*s){
	int i;
	
	struct vecteur*face;
	
	/*
	  printf("Sphère:\nAllocation: %d points, %d faces\nUtilisation: %d points, %d faces\n\n",
	  s->nb_points, s->nb_faces,
	  s->nb_points_crees, s->nb_faces_creees);
	*/
	for(i=0; i<s->nb_faces_creees; i++){
		/* Afficher le i-ème triangle. */
		face=&(s->tab_faces[i]);
		glBegin(GL_TRIANGLES);{
			/* Pour chaque sommet,
			   on extrait la couleur du point en question,
			   puis on positione un vertex à l'endroit adapté. */
			/* printf("Couleur sphère: %d, %d, %d\n",
			   (int)(s->tab_couleurs[(int)face->x].x), (int)(s->tab_couleurs[(int)face->x].y), (int)(s->tab_couleurs[(int)face->x].z)); */
			
			
			glColor4ub((int)(s->tab_couleurs[(int)face->x].x), (int)(s->tab_couleurs[(int)face->x].y), (int)(s->tab_couleurs[(int)face->x].z), 0);
			glVertex3d((s->tab_points[(int)face->x]).x, (s->tab_points[(int)face->x]).y, (s->tab_points[(int)face->x]).z);
			glColor4ub((int)(s->tab_couleurs[(int)face->y].x), (int)(s->tab_couleurs[(int)face->y].y), (int)(s->tab_couleurs[(int)face->y].z), 0);
			glVertex3d((s->tab_points[(int)face->y]).x, (s->tab_points[(int)face->y]).y, (s->tab_points[(int)face->y]).z);
			glColor4ub((int)(s->tab_couleurs[(int)face->z].x), (int)(s->tab_couleurs[(int)face->z].y), (int)(s->tab_couleurs[(int)face->z].z), 0);
			glVertex3d((s->tab_points[(int)face->z]).x, (s->tab_points[(int)face->z]).y, (s->tab_points[(int)face->z]).z);
		}
		glEnd();
	}
}

/* On veut savoir si le point de coordonnées (x, y, z) est un sommet de la sphère s.
   Si c'est le cas, on renvoie son indice;
   sinon, on renvoie -1. */
int chercher_sommet(struct sphere*s, float x, float y, float z){
	int i=0;
	struct vecteur*p;
	
	while(i<s->nb_points_crees){
		p=&(s->tab_points[i]);
		if(p->x==x && p->y==y && p->z==z){
			/* On a trouvé, le point existe dans la sphère. */
			return i;
		}
		i++;
	}
	/* Si on arrive ici, c'est qu'on a tout examiné sans rien trouver. */
	return -1;
}


/* Couper en quatre la i-ème face de la sphère s.
   Cette opération ajoute trois faces au maillage de la sphère,
   donc il faut vérifier que l'on ne dépassera pas la mémoire allouée.
*/
void subdiviser_face(struct sphere*s, int i_face){
	
	/* La face initiale. */
	struct vecteur*face=&(s->tab_faces[i_face]);
	
	/* Les indices des points de la face initiale. */
	int indice_a, indice_b, indice_c;
	
	/* Les points de la face initiale. */
	struct vecteur*a;
	struct vecteur*b;
	struct vecteur*c;
	/* Les indices des milieux des segments. */
	int indice_mab, indice_mbc, indice_mac;
	
	/* Les coordonnées des nouveaux points. */
	float x, y, z;
	int indice_deja_existant;
	
	if(s->nb_faces_creees+3 <= s->nb_faces){
		
		
		indice_a=(int)face->x;
		indice_b=(int)face->y;
		indice_c=(int)face->z;
	
		a=&(s->tab_points[indice_a]);
		b=&(s->tab_points[indice_b]);
		c=&(s->tab_points[indice_c]);
	
		/* On crée les milieux des segments. */
	
		/* On calcule les coordonnées du milieu du segment, et on cherche ce nouveau point
		   dans la liste des points existants. Si on n'a pas trouvé, alors on crée une
		   nouvelle entrée dans le tableau de points; si on a trouvé, on réutilise le
		   point déjà créé. */
	
		/***************************************************************/
		/* Milieu de AB. */
		x=(a->x+b->x)/2;
		y=(a->y+b->y)/2;
		z=(a->z+b->z)/2;
		indice_deja_existant=chercher_sommet(s, x, y, z);
		if(indice_deja_existant==-1){
			/* On crée un nouveau point. */
			init_vecteur(&(s->tab_points[s->nb_points_crees]), x, y, z);
			indice_mab=s->nb_points_crees;
			s->nb_points_crees++;
		}
		else{
			/* On réutilise ce point. */
			indice_mab=indice_deja_existant;
		}
		/***************************************************************/
		/* Milieu de AC. */
		x=(a->x+c->x)/2;
		y=(a->y+c->y)/2;
		z=(a->z+c->z)/2;
		indice_deja_existant=chercher_sommet(s, x, y, z);
		if(indice_deja_existant==-1){
			/* On crée un nouveau point. */
			init_vecteur(&(s->tab_points[s->nb_points_crees]), x, y, z);
			indice_mac=s->nb_points_crees;
			s->nb_points_crees++;
		}
		else{
			/* On réutilise ce point. */
			indice_mac=indice_deja_existant;
		}
		/***************************************************************/
		/* Milieu de BC. */
		x=(b->x+c->x)/2;
		y=(b->y+c->y)/2;
		z=(b->z+c->z)/2;
		indice_deja_existant=chercher_sommet(s, x, y, z);
		if(indice_deja_existant==-1){
			/* On crée un nouveau point. */
			init_vecteur(&(s->tab_points[s->nb_points_crees]), x, y, z);
			indice_mbc=s->nb_points_crees;
			s->nb_points_crees++;
		}
		else{
			/* On réutilise ce point. */
			indice_mbc=indice_deja_existant;
		}
		/***************************************************************/


	
		/* Couleurs. */
		init_vecteur(&(s->tab_couleurs[indice_mab]),
			     255-(s->tab_couleurs[indice_a].x+s->tab_couleurs[indice_b].x)/2,
			     255-(s->tab_couleurs[indice_a].y+s->tab_couleurs[indice_b].y)/2,
			     255-(s->tab_couleurs[indice_a].z+s->tab_couleurs[indice_b].z)/2);
		init_vecteur(&(s->tab_couleurs[indice_mac]),
			     255-(s->tab_couleurs[indice_a].x+s->tab_couleurs[indice_c].x)/2,
			     255-(s->tab_couleurs[indice_a].y+s->tab_couleurs[indice_c].y)/2,
			     255-(s->tab_couleurs[indice_a].z+s->tab_couleurs[indice_c].z)/2);
		init_vecteur(&(s->tab_couleurs[indice_mbc]),
			     255-(s->tab_couleurs[indice_b].x+s->tab_couleurs[indice_c].x)/2,
			     255-(s->tab_couleurs[indice_b].y+s->tab_couleurs[indice_c].y)/2,
			     255-(s->tab_couleurs[indice_b].z+s->tab_couleurs[indice_c].z)/2);
	
	
		/* On remplace la face abc par la face composée des milieux. */
		init_vecteur(face,
			     indice_mab, indice_mbc, indice_mac);
		/* Puis on ajoute les trois faces supplémentaires. */
		init_vecteur(&(s->tab_faces[s->nb_faces_creees]),
			     indice_a, indice_mab, indice_mac);
		init_vecteur(&(s->tab_faces[s->nb_faces_creees+1]),
			     indice_b, indice_mbc, indice_mab);
		init_vecteur(&(s->tab_faces[s->nb_faces_creees+2]),
			     indice_c, indice_mac, indice_mbc);
	
		/* Maintenant le tableau de faces et le tableau de points ont chacun trois éléments de plus. */
		s->nb_faces_creees+=3;
	}
	else{
		/* On ne peut pas créer davantage de faces sans dépasser la mémoire allouée. */
		/*
		  printf("subdivision impossible\n");
		*/
	}
}





/* Couper en quatre chaque triangle de la sphère,
   puis replacer tous les points à la bonne distance du centre. */
void subdiviser(struct sphere*s){
	
	/* On divise les faces en commençant par la dernière créée et en remontant vers le début de la liste.
	   À chaque face que l'on coupe, on ajoute trois faces à la fin, et on remplace la face originale
	   par la face portée par les milieux des côtés. */
	int i_face_final=s->nb_faces_creees;
	int i;
	/*
	  printf("subdiviser\n");
	*/	
	for(i=0; i<i_face_final; i++){
		/* On traite la face i. */
		subdiviser_face(s, i);
	}
	
	normaliser_sphere(s);
}



/* Borner les composantes de couleur des points de la sphère 
   entre 0 et 255; il ne faut pas dépasser les bornes de cet intervalle. */
void borner_couleurs(struct sphere*s){
	int i;
	/*
	  printf("nb_points_crees=%d\n", s->nb_points_crees);
	*/
	for(i=0; i<s->nb_points_crees; i++){
		init_vecteur(&(s->tab_couleurs[i]),
			     max(0, min(255, s->tab_couleurs[i].x)),
			     max(0, min(255, s->tab_couleurs[i].y)),
			     max(0, min(255, s->tab_couleurs[i].z)));
	}
}




void normaliser_sphere(struct sphere*s){
	/* Normaliser les points pour les placer sur la sphère.
	   NB: On ne traite pas le premier point qui est le centre de la sphère.
	   Les points d'indice 1 à 4 sont les sommets du premier tétra. */
	
	/* Attention: la sphère n'est pas forcément centrée sur l'origine du repère.
	   Il faut donc commencer par translater tous les points pour placer la sphère
	   à l'origine. NB: On ne modifiera pas le vecteur centre, ce qui permet de
	   conserver l'info initiale et de replacer les points après normalisation. */
	int i;
	
	for(i=0; i<s->nb_points; i++){
		/* 1) On ramène le point proche de l'origine du repère. */
		diff_vecteurs(s->tab_points[i], s->centre, &(s->tab_points[i]));
		
		/* 2) On le normalise. */
		normer_vecteur(&(s->tab_points[i]), &(s->tab_points[i]));
		
		/* 3) On le place sur la sphère. */
		mult_vect_scal(&(s->tab_points[i]), s->r, &(s->tab_points[i]));
		
		/* 4) On replace le point autour du centre réel de la sphère. */
		somme_vecteurs(s->tab_points[i], s->centre, &(s->tab_points[i]));
	}
}
