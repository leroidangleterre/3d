/* La structure de données qui contient l'ensemble des cubes et autres objets.
   
   À terme, il faudra pouvoir enregistrer le terrain dans un fichier,
   et charger un fichier pour créer un terrain.
*/

struct terrain{
	
	/* Le terrain contient un tableau tridimensionnel de cubes. */
	struct cube***tab;
	int longueur; /* Nombre de cubes selon l'axe X, de x_min à x_max; */
	int largeur; /* Nombre de cubes selon l'axe Y, de y_min à y_max; */
	int hauteur; /* Nombre de cubes selon l'axe Z, de z_min à z_max; */
	float taille_elem; /* Taille d'un cube. */
	enum mode*mode;
	struct vecteur gravite; /* Gravité de base dans tout le terrain, elle pourra être compensée ou modifiée par certains cubes. */
	struct vecteur gravite_sauvegarde; /* On mémorise la gravité quand on veut passer en mode vol libre. */
	/* Restitution de la vitesse après un choc vertical ou après un frottement tangentiel. */
	float restit_vertic;
	float restit_horiz;

	/* When the world is defined with triangles only, these lists are used. */
	int triangles_only; // Flag true when mesh loaded, false when cubes only.
	int nb_points;
	float**tab_points;
	float**tab_uv; // To each 3d-point corresponds a location in the UV-map.
	int nb_faces;
	int**tab_faces;

	float z_min, z_max;
	
	/* Nombre de cubes dans chaque direction que l'on affichera. */
	int distance_brouillard;

	/* Pour l'affichage des flèches. */
	float gravity_scale;

	/* Computation of the gravity:
	   if the computation takes too long, it is stopped and the index of the last cube computed
	   is stored so that the next computation takes care of the rest.
	*/
	int grav_last_x;
	int grav_last_y;
	int grav_last_z;
};

void trouver_cube_selon_coordonnees(struct terrain*t, float x, float y, float z, int*i, int*j, int*k);
struct cube*trouver_cube_selon_viseur(struct terrain*t, struct perso*p, int*indice_face);
void afficher_tranche(struct terrain*t, int i, int j0, int k0);
void afficher_colonne_de_cubes(struct terrain*t, int i, int j, int k0);
void get_centre_terrain(struct terrain*t, float*x, float*y, float*z);
struct cube*get_cube(struct terrain*t, int i, int j, int k);
int point_est_dans_tunnel(struct terrain*t, float x, float y, float z);
int point_est_dans_cube(struct terrain*t, float x, float y, float z);
void trouver_face_la_plus_proche(struct cube*c, struct vecteur*pos, int*indice_face);
struct cube*trouver_voisin_selon_face(struct cube*le_cube, struct terrain*t, int indice_face);
void afficher_terrain(struct terrain*t, struct camera c, GLuint texture);
void afficher_terrain_float(struct terrain*t,
			    float xCam, float yCam, float zCam,
			    float xCible, float yCible, float zCible,
			    float xVertic, float yVertic, float zVertic);
void afficher_tranche(struct terrain*t, int i, int j0, int k0);
void afficher_colonne_de_cubes(struct terrain*t, int i, int j, int k0);
int compute_local_gravity_terrain(struct terrain*t);
void compute_local_gravity(struct terrain*t, struct cube*c);
void raz_local_gravity_terrain(struct terrain*t);
void reset_scale_gravity(struct terrain*t);
void scale_gravity(struct terrain*t, float coef);
void test_cut_cubes(struct terrain*t);
void test_load_world_file(struct terrain*t);
void display_world_as_triangles(struct terrain*t,
				float xCam, float yCam, float zCam,
				float xCible, float yCible, float zCible,
				float xVertic, float yVertic, float zVertic,
				GLuint texture);
void find_closest_point(struct terrain*t,
			float x, float y, float z,
			float*x_res, float*y_res, float*z_res);


/* Cette équation permet de spécifier quels cubes existent au début.
   Un cube existe si la fonction equation_terrain renvoie vrai (çad 1). */
float equation_terrain(struct terrain*t, float x0, float y0, float z0){
	float x=x0-t->longueur/2;
	float y=y0-t->largeur/2;
	float z=z0-t->hauteur/2;
	float r2=x*x+y*y+z*z;
	
	/* Torus: minor radius (r) and major radius (R) */
	float R = 15;
	float r = 4;
	float delta_r;

	/* Sphere or flat disc: radius. */
	float radius;
	
	int type_terrain = 8;
	
	switch(type_terrain){
	case 0: /* Terrain vide. */
		return 0;
		break;
	case 1: /* Terrain plein. */
		return 1;
		break;
	case 2: /* Un seul cube au centre. */
		return x*x<1 && y*y<1 && z*z<1;
		break;
	case 3: /* Sol horizontal. */
		radius = 30;
		return (x*x + y*y)<radius*radius && z*z<1;
		break;
	case 4: /* Deux murs. */
		return (z>=(t->taille_elem)*10 && z<(t->taille_elem)*11)
			||(x<=(t->taille_elem)*3 && x>(t->taille_elem)*2)
			||(y<=(t->taille_elem)*3 && y>(t->taille_elem)*2);
		break;
	case 5: /* Sphère. */
		radius = t->longueur/2;
		return (r2<radius*radius) && (x*x+y*y+z*z > (radius-1)*(radius-1));
		break;
	case 6: /* Pente régulière. */
		return z<(x+2*y)/5-5;
		break;
	case 7: /* Paraboloïde. */
		return (x*x+y*y)<20*z && (x*x+y*y)>20*(z-2);
		break;
	case 8: /* Terrain plein avec des tunnels, avec un mur vertical au fond. */
		return (   ((!(point_est_dans_tunnel(t, x0, y0, z0)) && x<=19 && z<20) || x>19)   );
		break;
	case 9: /* Terrain sinusoïdal. */
		return z<sin(0.1*x) + cos(0.3*y);
		break;
        case 10: /* Planète torique. */
		// printf("torique\n");
		delta_r = (R - sqrt(x*x+y*y));
		return z*z <= r*r - delta_r*delta_r
			|| (x*x<1 && y*y<1 && z*z<1);          // The central cube
		break;
	case 11: /* Deux cubes à deux sommets opposés du terrain. */
		return (x0<=1 && y0<=1 && z0<=1  ||  x0>=t->longueur-2 && y0>=t->largeur-2 && z0>= t->hauteur-2);
		break;
	case 12: /* Un cube à un sommet du terrain. */
		return (x0>=t->longueur-2 && y0>=t->largeur-2 && z0>= t->hauteur-2);
		break;
	case 13: /* Deux cubes au centre de deux faces opposées du terrain. */
		return (x0<=1 && y0<=1  ||  x0>=t->longueur-2 && y0>=t->largeur-2) && (0<=z && z<=1);
		break;
	default:
		return random_int_a_b(0, 3)==0;
		break;
	}
	 
}


void init_gravite_terrain(struct terrain*t, float gx, float gy, float gz){
	int i, j, k;
	init_vecteur(&(t->gravite), gx, gy, gz);

	if(!t->triangles_only){
		for(i=0; i<t->longueur; i++){
			for(j=0; j<t->largeur; j++){
				for(k=0; k<t->hauteur; k++){
					init_gravite_cube(&(t->tab[i][j][k]), gx, gy, gz);
				}
			}
		}
	}
}



/* La variable énumérée mode permet de savoir si on est en MODE_JEU, ou en MODE_MENU, ou dans d'autres modes.
   Les paramètres longueur, largeur, hauteur désignent le nombres de cubes
   dans chaque direction (x, y et z).
   Le paramètre taille_elem désigne la longueur des arêtes des cubes.
*/
void init_terrain(struct terrain*t, int longueur, int largeur, int hauteur, float taille_elem, float restit_vertic, float restit_horiz, enum mode*mode, int use_local_gravity){
	
	int i, j, k;
	/* Les coordonnées des centres des cubes. */
	float xCentre, yCentre, zCentre;
	
	int nb_cubes_crees=0;
	
	struct cube*cube_en_cours;
	
	t->triangles_only = 0;
	
	t->distance_brouillard=200;
	
	t->mode=mode;
	
	t->longueur=longueur;
	t->largeur=largeur;
	t->hauteur=hauteur;
	t->taille_elem=taille_elem;
	t->restit_vertic=restit_vertic;
	t->restit_horiz=restit_horiz;
	
	t->gravity_scale = 1;




	t->tab_points = NULL;
	t->tab_uv = NULL;
	t->tab_faces = NULL;
	t->nb_points = 0;
	t->nb_faces = 0;
	test_load_world_file(t);
	return;

	/***************************************************/
	/* If not return, create the world based on cubes. */
	/***************************************************/

	/* Allocation mémoire pour le tableau de cubes. */
	t->tab=(struct cube***)malloc((t->longueur)*sizeof(struct cube**));
	for(i=0; i<t->longueur; i++){
		
		xCentre=((float)(i))*taille_elem;
		
		t->tab[i]=(struct cube**)malloc((t->largeur)*sizeof(struct cube*));
		for(j=0; j<t->largeur; j++){
			
			yCentre=((float)(j))*taille_elem;
			
			t->tab[i][j]=(struct cube*)malloc((t->hauteur)*sizeof(struct cube));
			for(k=0; k<t->hauteur; k++){
				
				zCentre=((float)(k))*taille_elem;
				
				init_cube(&(t->tab[i][j][k]),
					  xCentre, yCentre, zCentre,
					  taille_elem,   /* Taille des cubes. */
					  restit_vertic, /* Restitution verticale. */
					  restit_horiz); /* Restitution horizontale. */
				/* On décide de ne créer que les cubes vérifiant l'équation donnée par la fonction equation_terrain; */
				
				triangulate_cube(&(t->tab[i][j][k])); /* All cubes are triangulated from the very beginning. */

				if(equation_terrain(t, xCentre, yCentre, zCentre)){
					/* On crée le cube. */
					(t->tab[i][j][k]).existe = 1;
					nb_cubes_crees++;
					// printf("cube existant: %f, %f, %f\n", xCentre, yCentre, zCentre);
				}
				else{
					/* le cube ne doit pas exister. */
					(t->tab[i][j][k]).existe = 0;
				}
			}
		}
	}

	/* Chaque cube veut savoir ce qu'il a comme voisins. */
	for(i=0; i<t->longueur; i++){
		for(j=0; j<t->largeur; j++){
			for(k=0; k<t->hauteur; k++){
				
				cube_en_cours=get_cube(t, i, j, k);
				
				/* Les pointeurs vers les voisins peuvent prendre la valeur NULL s'il n'y a pas de voisin,
				   ou si le voisin est vide,
				   sinon c'est l'adresse du voisin. */
				
				if(cube_en_cours!=NULL){ /* On ne traite que les cubes non vides. */
					cube_en_cours->voisin_z_plus=get_cube(t, i, j, k+1);
					cube_en_cours->voisin_z_moins=get_cube(t, i, j, k-1);
					cube_en_cours->voisin_x_moins=get_cube(t, i-1, j, k);
					cube_en_cours->voisin_x_plus=get_cube(t, i+1, j, k);
					cube_en_cours->voisin_y_plus=get_cube(t, i, j+1, k);
					cube_en_cours->voisin_y_moins=get_cube(t, i, j-1, k);
					calculer_visibilite(cube_en_cours);
				}
			}
		}
	}

	if(use_local_gravity){
		/* Set the local gravity for all cubes. */
		
	}
	t->grav_last_x = 0;
	t->grav_last_y = 0;
	t->grav_last_z = 0;
}

/* Annuler la gravité dans chaque cube du terrain. */
void raz_local_gravity_terrain(struct terrain*t){

	int i, j, k;
	struct cube*cube_en_cours;

	for(i=0; i<t->longueur; i++){
		for(j=0; j<t->largeur; j++){
			for(k=0; k<t->hauteur; k++){
				cube_en_cours=get_cube(t, i, j, k);
				cube_en_cours->gx = 0;
				cube_en_cours->gy = 0;
				cube_en_cours->gz = 0;
			}
		}
	}
}

/* Mettre à jour la gravité dans tout le terrain. */
/* Update the gravity in all the world.
   The execution of this function must not exceed a maximum allocated time (max_time); if it is not done in that delay,
   it returns anyway but signals that some work is left to do (returned value is 1);
   at the next call, it will start where it left.
*/
int compute_local_gravity_terrain(struct terrain*t){

	float percent;
	int i, j, k;
	int i0, j0, k0; /* First cube that will be treated by the current call of the function. */
	struct cube*cube_en_cours;
	
	int done = 0;
	clock_t start_date = clock();
	float max_time = 1; /* Time allocated to a single call to this function. */
	int delayed_expired = 0;
	int nb_treated_cubes = 0;
	
	raz_local_gravity_terrain(t);
	reset_scale_gravity(t);

	/* Start right after the last considered cube. */
	i0 = t->grav_last_x;
	j0 = t->grav_last_y;
	k0 = t->grav_last_z;

	printf("\n\tstart grav at %d, %d, %d\n", i0, j0, k0);

	i = i0;
	j = j0;
	k = k0;

	while(i<t->longueur){
		if(i==i0){
			/* This is the loop restarting. */
			j = j0;
		}
		else{
			/* This is the usual situation. */
			j = 0;
		}
		while(j<t->largeur){
			if(i==i0 && j==j0){
				/* This is the loop restarting. */
				k = k0;
			}
			else{
				/* This is the usual situation. */
				k = 0;
			}
			while(k<t->hauteur){
				cube_en_cours=get_cube(t, i, j, k);
				// printf("[%d %d %d] - ", i, j, k);
				// printf("Computing local gravity:\n");
				compute_local_gravity(t, cube_en_cours);
				nb_treated_cubes++;
					
				// percent = ((float)i) / ((float)(t->longueur)) + 0.01*((float)j) / ((float)(t->largeur)) + 0.0001*((float)k) / ((float)(t->hauteur));
				
				if(clock() - start_date > max_time * CLOCKS_PER_SEC){
					/* Memorize the last cube checked... */
					t->grav_last_x = i;
					t->grav_last_y = j;
					t->grav_last_z = k;
					// printf("grav: not finished: %d/%d,  %d/%d, %d/%d\n", t->grav_last_x, t->longueur, t->grav_last_y, t->largeur, t->grav_last_z, t->hauteur);
					/* ... and stop the computation. */
					printf("\t\t%d cubes out of %d\n", nb_treated_cubes, t->longueur * t->largeur * t->hauteur);
					return 1; // Not finished.
				}
				k++;
			}
			j++;
		}
		i++;
	}
	

	t->grav_last_x = i;
	t->grav_last_y = j;
	t->grav_last_z = k;

	/* Do this only at the very end, when all cubes were updated. */
	for(i=0; i<t->longueur; i++){
		for(j=0; j<t->largeur; j++){
			for(k=0; k<t->hauteur; k++){
				cube_en_cours=get_cube(t, i, j, k);
				init_arrow(&(cube_en_cours->arrow),
					   cube_en_cours->x,
					   cube_en_cours->y,
					   cube_en_cours->z,
					   cube_en_cours->gx,
					   cube_en_cours->gy,
					   cube_en_cours->gz);
			}
		}
	}
	printf("grav: Finished: %d/%d,  %d/%d, %d/%d\n",
	       t->grav_last_x, t->longueur, t->grav_last_y, t->largeur, t->grav_last_z, t->hauteur);
	/* Next time, we start from scratch. */
	t->grav_last_x = 0;
	t->grav_last_y = 0;
	t->grav_last_z = 0;
	return 0; // Finished.
}



/* Compute the effect of each cube (other_cube) of the terrain on the gravity of the selected cube (c).
   Only the non-empty 'other_cube's have an influence on 'c'. */
void compute_local_gravity(struct terrain*t, struct cube*c){

	int i, j, k;
	struct cube*other_cube;
	
	if(c!=NULL){
		for(i=0; i<t->longueur; i++){
			for(j=0; j<t->largeur; j++){
				for(k=0; k<t->hauteur; k++){
					other_cube = get_cube(t, i, j, k);
					if(other_cube != NULL && other_cube->existe){
						/* Effect of c on the other_cube. */
						compute_attraction_cube_on_cube(other_cube, c);
					}
				}
			}
		}
	}
}




/* Si les valeurs i, j et k correspondent à un cube du terrain, même vide, alors on renvoie un pointeur vers ce cube;
   sinon, on renvoie NULL. */
struct cube*get_cube(struct terrain*t, int i, int j, int k){
	
	if(i>=0 && i<t->longueur
	   && j>=0 && j<t->largeur
	   && k>=0 && k<t->hauteur){
		
		return &(t->tab[i][j][k]);
		
	}
	else{
		return NULL;
	}
}



void afficher_terrain(struct terrain*t, struct camera c, GLuint texture){
	if(t->tab_faces == NULL){

		/* The world is defined by the cubes. */
		afficher_terrain_float(t,
				       c.empty.pos.x, c.empty.pos.y, c.empty.pos.z,
				       c.empty.cible.x, c.empty.cible.y, c.empty.cible.z,
				       c.empty.vertic.x, c.empty.vertic.y, c.empty.vertic.z);
	}
	else{
		/* The world is defined by the triangles. */
		display_world_as_triangles(t,
					   c.empty.pos.x, c.empty.pos.y, c.empty.pos.z,
					   c.empty.cible.x, c.empty.cible.y, c.empty.cible.z,
					   c.empty.vertic.x, c.empty.vertic.y, c.empty.vertic.z,
					   texture);
	}
}

void afficher_terrain_float(struct terrain*t,
			    float xCam, float yCam, float zCam,
			    float xCible, float yCible, float zCible,
			    float xVertic, float yVertic, float zVertic){
	
	/* Indice pour les boucles. */
	int i, j, k;
	
	/* Indices du carreau contenant la caméra. */
	int i0, j0, k0;

	struct cube*c;
	
	/* Pour afficher correctement les cubes dotés de transparence, il faut commencer par l'extérieur du terrain
	   et se rapprocher du point d'observation.
	   Pour cela, il faut déterminer dans quel carreau la caméra se trouve. */
	trouver_cube_selon_coordonnees(t, xCam, yCam, zCam, &i0, &j0, &k0);
	
	/* Si la caméra est à l'extérieur du terrain, il faut modifier les valeurs
	   de i0, j0 ou k0 pour ne pas demander à afficher des données inexistantes. */
	i0=min(max(i0, 0), t->longueur);
	j0=min(max(j0, 0), t->largeur);
	k0=min(max(k0, 0), t->hauteur);
	
	gluLookAt(xCam ,yCam, zCam,
		  xCam+xCible, yCam+yCible, zCam+zCible,
		  xVertic, yVertic, zVertic);

	/* On affiche d'abord toutes les flèches. */
	for(i=0; i<t->longueur; i++){
		for(j=0; j<t->largeur; j++){
			for(k=0; k<t->hauteur; k++){
				c = get_cube(t, i, j, k);
				if(c != NULL){
					display_arrow(&(c->arrow));
				}
			}
		}
	}

	
	/* On affiche le terrain en démarrant à l'extérieur pour finir à l'endroit où est positionnée la caméra. */

	for(i=max(0, i0-t->distance_brouillard); i<i0; i++){
		afficher_tranche(t, i, j0, k0);
	}
	for(i=min(t->longueur-1, i0+t->distance_brouillard); i>=i0; i--){
		afficher_tranche(t, i, j0, k0);
	}
}




/* Afficher les cubes du terrain t qui constituent la tranche d'indice i.
   On rappelle que la caméra se situe dans le cube (i0, j0, k0). */
void afficher_tranche(struct terrain*t, int i, int j0, int k0){
	int j;
	for(j=0; j<j0; j++){
		afficher_colonne_de_cubes(t, i, j, k0);
	}
	for(j=t->largeur-1; j>=j0; j--){
		afficher_colonne_de_cubes(t, i, j, k0);
	}
}



/* Afficher les cubes du terrain t qui appartiennent à une colonne verticale,
   et dont les deux premiers indices sont i et j;
   on va du bas jusqu'à l'indice k0-1, puis du haut jusqu'à l'indice k0; */
void afficher_colonne_de_cubes(struct terrain*t, int i, int j, int k0){
	int k;
	for(k=0; k<k0; k++){
		afficher_cube(&(t->tab[i][j][k]));
	}
	for(k=t->hauteur-1; k>=k0; k--){
		afficher_cube(&(t->tab[i][j][k]));
	}
}





/* On cherche le cube contenant le point (x, y, z);
   on place ses indices dans les entiers désignés par les pointeurs i, j et k.
   ATTENTION!!! Cette fonction devra être modifiée si on change la façon dont on place les cubes
   lors de l'initialisation. */
void trouver_cube_selon_coordonnees(struct terrain*t, float x, float y, float z, int*i, int*j, int*k){
	float taille=t->taille_elem;
	*i=(int)(x/taille + 0.5);
	*j=(int)(y/taille + 0.5);
	*k=(int)(z/taille + 0.5);
}




/* Trouver le point situé au centre du terrain. */
void get_centre_terrain(struct terrain*t, float*x, float*y, float*z){
	
	float x_min=((float)0)*t->taille_elem;
	float x_max=((float)(t->longueur))*t->taille_elem;
	float y_min=((float)0)*t->taille_elem;
	float y_max=((float)(t->largeur))*t->taille_elem;
	float z_min=((float)0)*t->taille_elem;
	float z_max=((float)(t->hauteur))*t->taille_elem;
	*x=(x_min+x_max)/2;
	*y=(y_min+y_max)/2;
	*z=(z_min+z_max)/2;
}



/* Appliquer au perso la gravité globale du terrain. */
void appliquer_gravite_globale_terrain_sur_perso(struct terrain*t, struct perso*p, float dt){
	struct vecteur increment; /* La vitesse qui sera ajoutée à la vitesse actuelle. */
	if(t!=NULL && p!=NULL){
		init_vecteur(&increment,
			     t->gravite.x*dt,
			     t->gravite.y*dt,
			     t->gravite.z*dt);
		somme_vecteurs(p->vitesse,
			       increment,
			       &(p->vitesse));
	}
}


/* Apply the local gravity to the character. */
void appliquer_gravite_locale_terrain_sur_perso(struct terrain*t, struct perso*p, float dt){
	struct vecteur increment; /* La vitesse qui sera ajoutée à la vitesse actuelle. */
	int i, j, k;
	struct cube*c;
	if(t!=NULL && p!=NULL){
		/* trouver_cube_selon_coordonnees(struct terrain*t, float x, float y, float z, int*i, int*j, int*k); */
		trouver_cube_selon_coordonnees(t,
					       p->empty.pos.x, p->empty.pos.y, p->empty.pos.z,
					       &i, &j, &k);
	
		c = get_cube(t, i, j, k);
		if(c!=NULL){
			printf("g = {%f, %f, %f}\n", c->gx, c->gy, c->gz);
			init_vecteur(&increment,
				     c->gx*dt,
				     c->gy*dt,
				     c->gz*dt);
			somme_vecteurs(p->vitesse,
				       increment,
				       &(p->vitesse));
		}
	}
}

/* Chaque cube qui touche le perso doit appliquer au perso une force
   qui dépend du champ local au cube et du volume de l'intersection entre le perso et le cube. */
void appliquer_forces_terrain_sur_perso(struct terrain*t, struct perso*p, float dt){
	
	
	
	/* On scanne tous les cubes (il faut limiter ce scan aux cubes proches du perso),
	   et pour chacun deux on applique une force au perso. */
	
	/* Les coordonnées du perso. */
	float x=p->empty.pos.x;
	float y=p->empty.pos.y;
	float z=p->empty.pos.z;
	
	float taille=t->taille_elem; /* Le côté d'un cube. */
	
	int i_cube, j_cube, k_cube; /* Les indices du cube contenant le centre du perso. */
	int i, j, k; /* Indices pour les boucles. */
	
	trouver_cube_selon_coordonnees(t,
				       x, y, z,
				       &i_cube, &j_cube, &k_cube);

	
	/* On utilise les min et les max pour être sûr de ne jamais
	   appliquer des forces liées à un cube inexistant. */
	for(i=max(0, i_cube-2*taille); i<=min(t->longueur, i_cube+2*taille); i++){
		for(j=max(0, j_cube-2*taille); j<=min(t->largeur, j_cube+2*taille); j++){
			for(k=max(0, k_cube-2*taille); k<=min(t->largeur, k_cube+2*taille); k++){
				
				appliquer_forces_cube_sur_perso(get_cube(t, i, j, k), p, dt);
			}
		}
	}
	
	/* Appliquer au perso la gravité globale du terrain. */
	appliquer_gravite_globale_terrain_sur_perso(t, p, dt);
}










/* Le but de cette fonction est d'examiner les points de collision entre le perso et les cubes qui composent le terrain,
   puis de modifier la position, la vitesse ou l'accélération du perso en conséquence.
*/
void calculer_collision_terrain_perso(struct terrain*t, struct perso*p, float dt){	
	
	
	
	/* Trouver le cube contenant le perso. */
	int i_cube, j_cube, k_cube;
	/* Trouver les indices des cubes à examiner. */
	/* On prendra les 26 cubes voisins. */
	
	
	/* Centre du perso. */
	float x=p->empty.pos.x;
	float y=p->empty.pos.y;
	float z=p->empty.pos.z;
	/* Cette valeur sert à sélectionner les cubes les plus proches du perso,
	   pour limiter le nombre de calculs de collision. */
	/* float longueur=p->longueur;*/

	int i;

	/* The position of the reacting point. */
	float xp, yp, zp;

	if(t->tab_faces != NULL){

		/* Compute collisions between the character and the triangular faces that define the world. */
		// TODO

		// Step One: find the point that is closest to the center of the character.
		find_closest_point(t, x, y, z,
				   &xp, &yp, &zp);

		p->x_close = xp;
		p->y_close = yp;
		p->z_close = zp;
		init_arrow(&(p->arrow),
			   p->empty.pos.x, p->empty.pos.y, p->empty.pos.z,
			   p->x_close - p->empty.pos.x,
			   p->y_close - p->empty.pos.y,
			   p->z_close - p->empty.pos.z);
			   

		// Step Two: compute the reaction, act on the character's position and speed.
		// Get the character further away from that point.


	}
	else{
	
		/* Le cube avec lequel on considère une collision. */
		/* struct cube*c; */
	
		/* On va considérer tous les cubes qui peuvent intersecter la sphère du perso;
		   les autres seront forcément trop loin donc on ne les testera pas. */
	
	
	
		/* Ici on doit limiter le nombre de cubes à examiner. */
		/* RAPPEL: void trouver_cube_selon_coordonnees(struct terrain*t, float x, float y, float z, int*i, int*j, int*k); */
		trouver_cube_selon_coordonnees(t,
					       x, y, z,
					       &i_cube, &j_cube, &k_cube);
	
		/* Le centre de gravité du perso est dans le cube {i_cube, j_cube, k_cube}.
		   On examine, dans cet ordre:
		   - le cube contenant le centre du perso;
		   - les six cubes voisins directs, situés à une unité de distance Manhattan; ce sont les "centres des faces du Rubik's".
		   - les douze cubes "arêtes" (distance 2);
		   - les huit cubes "coins" du Rubik's (distance 3).
		*/
	
		/* Centre. */
		calculer_collision_cube_perso(get_cube(t, i_cube, j_cube, k_cube), p);
	
		/* Centres des faces. */
		calculer_collision_cube_perso(get_cube(t, i_cube+1, j_cube, k_cube), p);
		calculer_collision_cube_perso(get_cube(t, i_cube-1, j_cube, k_cube), p);
		calculer_collision_cube_perso(get_cube(t, i_cube, j_cube+1, k_cube), p);
		calculer_collision_cube_perso(get_cube(t, i_cube, j_cube-1, k_cube), p);
		calculer_collision_cube_perso(get_cube(t, i_cube, j_cube, k_cube+1), p);
		calculer_collision_cube_perso(get_cube(t, i_cube, j_cube, k_cube-1), p);
	
		/* Cubes "arêtes. */
		calculer_collision_cube_perso(get_cube(t, i_cube+1, j_cube+1, k_cube), p);
		calculer_collision_cube_perso(get_cube(t, i_cube+1, j_cube-1, k_cube), p);
		calculer_collision_cube_perso(get_cube(t, i_cube-1, j_cube-1, k_cube), p);
		calculer_collision_cube_perso(get_cube(t, i_cube-1, j_cube+1, k_cube), p);
	
		calculer_collision_cube_perso(get_cube(t, i_cube, j_cube+1, k_cube+1), p);
		calculer_collision_cube_perso(get_cube(t, i_cube, j_cube+1, k_cube-1), p);
		calculer_collision_cube_perso(get_cube(t, i_cube, j_cube-1, k_cube-1), p);
		calculer_collision_cube_perso(get_cube(t, i_cube, j_cube-1, k_cube+1), p);
	
		calculer_collision_cube_perso(get_cube(t, i_cube+1, j_cube, k_cube+1), p);
		calculer_collision_cube_perso(get_cube(t, i_cube+1, j_cube, k_cube-1), p);
		calculer_collision_cube_perso(get_cube(t, i_cube-1, j_cube, k_cube-1), p);
		calculer_collision_cube_perso(get_cube(t, i_cube-1, j_cube, k_cube+1), p);
	
		/* Cubes "coins. */
		calculer_collision_cube_perso(get_cube(t, i_cube+1, j_cube+1, k_cube+1), p);
		calculer_collision_cube_perso(get_cube(t, i_cube+1, j_cube+1, k_cube-1), p);
		calculer_collision_cube_perso(get_cube(t, i_cube+1, j_cube-1, k_cube+1), p);
		calculer_collision_cube_perso(get_cube(t, i_cube+1, j_cube-1, k_cube-1), p);
		calculer_collision_cube_perso(get_cube(t, i_cube-1, j_cube+1, k_cube+1), p);
		calculer_collision_cube_perso(get_cube(t, i_cube-1, j_cube+1, k_cube-1), p);
		calculer_collision_cube_perso(get_cube(t, i_cube-1, j_cube-1, k_cube+1), p);
		calculer_collision_cube_perso(get_cube(t, i_cube-1, j_cube-1, k_cube-1), p);
	
	
		/* Débug: On fait rebondir le perso sur la frontière du terrain. */
		/*
		  if((p->empty.pos.x>t->longueur && p->vitesse.x>0)
		  ||
		  (p->empty.pos.x<0 && p->vitesse.x>0)){
		  p->vitesse.x=-p->vitesse.x;
		  }
		  if((p->empty.pos.y>t->largeur && p->vitesse.y>0)
		  ||
		  (p->empty.pos.y<0 && p->vitesse.y>0)){
		  p->vitesse.y=-p->vitesse.y;
		  }
		  if((p->empty.pos.z>t->hauteur && p->vitesse.z>0)
		  ||
		  (p->empty.pos.z<0 && p->vitesse.z>0)){
		  p->vitesse.z=-p->vitesse.z;
		  }
		*/
	}
}




void evoluer_terrain(struct terrain*t, int**tab_appuis, struct perso*le_perso){
	
	struct cube*le_cube; /* Le cube sur lequel on a cliqué. */
	struct cube*voisin;  /* Le voisin de le_cube qui partage la face sur laquelle on a cliqué. */
	int indice_face;
	
	if((*tab_appuis)[SDLK_v]){
		/* Changer la gravité entre sa valeur standard et 0. */
		(*tab_appuis)[SDLK_v]=0;
		if(est_vecteur_nul(&(t->gravite))){
			/* Rétablir la gravité sauvegardée. */
			clone_vecteur(t->gravite_sauvegarde, &(t->gravite));
			printf("gravité ON.\n");
		}
		else{
			/* Sauvegarder la valeur de la gravité puis l'annuler. */
			clone_vecteur(t->gravite, &(t->gravite_sauvegarde));
			init_vecteur(&(t->gravite), 0, 0, 0);
			printf("gravité OFF.\n");
		}
	}
	
	/* Si l'un des clics de la souris est actif,
	   alors il faut trouver quelle face de quel cube est visée par le perso;
	   si cette face est suffisamment proche, alors on supprime un cube ou on en crée un,
	   selon qu'on a effectué un clic gauche ou droit. */
	if((*tab_appuis)[CLIC_GAUCHE_ACTIF]){
		(*tab_appuis)[CLIC_GAUCHE_ACTIF]=0;
		le_cube=trouver_cube_selon_viseur(t, le_perso, &indice_face);
		if(le_cube!=NULL){
			/* On détruit le cube. */
			if(le_cube!=NULL){
				le_cube->existe=0;
			}
			/* Les voisins du cube changent de visibilité. */
			calculer_visibilite(le_cube->voisin_x_plus);
			calculer_visibilite(le_cube->voisin_x_moins);
			calculer_visibilite(le_cube->voisin_y_plus);
			calculer_visibilite(le_cube->voisin_y_moins);
			calculer_visibilite(le_cube->voisin_z_plus);
			calculer_visibilite(le_cube->voisin_z_moins);
		}
	}
	if((*tab_appuis)[CLIC_DROIT_ACTIF]){
		(*tab_appuis)[CLIC_DROIT_ACTIF]=0;
		le_cube=trouver_cube_selon_viseur(t, le_perso, &indice_face);
		if(le_cube!=NULL){
			/* On place un cube à côté du cube visé. */
			/* Test: on change la couleur du cube visé. */
			changer_couleur_cube(le_cube);
			printf("face=%d\n", indice_face);
			
			voisin=trouver_voisin_selon_face(le_cube, t, indice_face);
			if(voisin!=NULL){
				printf("voisin non NULL\n");
				voisin->existe=1;
			}
			else{
				printf("voisin NULL\n");
			}
		}
	}
	if((*tab_appuis)[SDLK_c]){
		/* Test: cut the cubes. */
		printf("terrain: test_cut_cubes\n");
		test_cut_cubes(t);
	}
	if((*tab_appuis)[SDLK_l]){
		/* Test: load a world file. */
		printf("load world file.\n");
		test_load_world_file(t);
	}

}



/*
  La valeur de *indice_face donne une indication sur la face qui a été touchée:
  0<->face x sup;
  1<->face x inf;
  2<->face y sup;
  3<->face y inf;
  4<->face z sup;
  5<->face z inf.
*/
struct cube*trouver_voisin_selon_face(struct cube*le_cube, struct terrain*t, int indice_face){
	struct cube*res;
	switch(indice_face){
	case 0: res=le_cube->voisin_x_plus; break;
	case 1: res=le_cube->voisin_x_moins; break;
	case 2: res=le_cube->voisin_y_plus; break;
	case 3: res=le_cube->voisin_y_moins; break;
	case 4: res=le_cube->voisin_z_plus; break;
	case 5: res=le_cube->voisin_z_moins; break;
	default: res=NULL; break;
	}
	return res;
}



/* On définit des tunnels par un ensemble de points positionnés sur une ou plusieurs lignes,
   et un rayon.
   Cette fonction renvoie vrai si on est à l'intérieur d'un tunnel du terrain,
   càd s'il existe un point dans la liste_points_tunnels dont la distance avec (x0, y0, y0) est
   inférieure au rayon.
*/
int point_est_dans_tunnel(struct terrain*t, float x, float y, float z){
	
	struct vecteur tab[100];
	struct vecteur point;
	struct vecteur diff;
	int i;
	
	float rayon=3;
	
	/* On change de coordonnées: on se place dans le repère où le terrain va de -longueur/2 à +longueur/2, etc. */
	x=x-t->longueur/2;
	y=y-t->largeur/2;
	z=z-t->hauteur/2;
	
	/* Créer le tableau de points. */
	for(i=0; i<100; i++){
		init_vecteur(&(tab[i]),
			     (-t->longueur/2 + i*(t->longueur)/100), /* X */
			     0,
			     0.07*(-t->longueur/2 + i*(t->longueur)/100)*(-t->longueur/2 + i*(t->longueur)/100)); /* X*X */
	}
	
	
	/* On cherche un point du tableau suffisamment proche du point paramètre. */
	init_vecteur(&point, x, y, z);
	for(i=0; i<100; i++){
		diff_vecteurs(point, tab[i], &diff);
		if(get_norme_vecteur(&diff)<rayon){
			return 1; /* On est proche de la trajectoire du tunnel. */
		}
	}
	return 0; /* On n'est proche d'aucun point de la trajectoire du tunnel. */
}


/* Cette fonction renvoie vrai s'il existe un cube dans le terrain
   qui contient le point de coordonnées pos. */
int terrain_contient_point(struct terrain*t, struct vecteur*pos){
	
	float x=pos->x;
	float y=pos->y;
	float z=pos->z;
	
	float taille=t->taille_elem;
	int i=(int)(x/taille + 0.5);
	int j=(int)(y/taille + 0.5);
	int k=(int)(z/taille + 0.5);

	struct cube*le_cube;
	
	int res;
	
	if(t->triangles_only){
		res = 0;
	}
	else{
		le_cube = get_cube(t, i, j, k);

		if(le_cube!=NULL){
			if(le_cube->existe){
				/* On est donc à l'intérieur d'un cube. */
				res=1; /* Vrai. */
			}
			else{
				res=0; /* Faux. */
			}
		}
		else{
			res=0;
		}
	}
	return res;
}




/* Trouver le premier cube situé dans l'axe du viseur du perso.
   Pour l'instant on ne s'ocupe pas de la distance entre ce cube et le perso.
   
   La valeur de *indice_face donne une indication sur la face qui a été touchée:
   0<->face x sup;
   1<->face x inf;
   2<->face y sup;
   3<->face y inf;
   4<->face z sup;
   5<->face z inf.
*/
struct cube*trouver_cube_selon_viseur(struct terrain*t, struct perso*p, int*indice_face){
	
	/* Le point m se déplace depuis le point d'observation du perso
	   (à mi-chemin entre le milieu et le haut du perso);
	   et dans la direction de la cible de la caméra,
	   jusqu'à rencontrer une face d'un cube qui existe.
	   On mémorisera notamment quelle face a été attaquée. */
	
	struct vecteur m; /* Origine. */
	struct vecteur dm; /* Axe de visée. */
	float d_mini=0.01;
	int i, j, k;
	struct cube*res;
	
	init_vecteur(&m,
		     p->empty.pos.x,
		     p->empty.pos.y,
		     p->empty.pos.z+p->hauteur/4);
	init_vecteur(&dm,
		     p->empty.pos.x - p->la_cam->empty.pos.x,
		     p->empty.pos.y - p->la_cam->empty.pos.y,
		     p->empty.pos.z + p->hauteur/4 - p->la_cam->empty.pos.z);
	normer_vecteur(&dm, &dm);
	mult_vect_scal(&dm, d_mini, &dm);
	i=0;
	while(i<10000 && !(terrain_contient_point(t, &m))){
		i++;
		/* m=m+dm; */
		somme_vecteurs(m, dm, &m);
	}
	/* Ici, soit on est très loin du point de départ et on considère qu'on n'a rien touché,
	   soit on est dans un cube et il faut donc renvoyer un pointeur vers ce cube
	   ainsi qu'une indication sur la face que l'on a touchée. */
	if(terrain_contient_point(t, &m)){
		//void trouver_cube_selon_coordonnees(struct terrain*t, float x, float y, float z, int*i, int*j, int*k);
		//struct cube*get_cube(struct terrain*t, int i, int j, int k);
		trouver_cube_selon_coordonnees(t, m.x, m.y, m.z, &i, &j, &k);
		/* On est arrivé dans le cube (i, j, k). */
		res=get_cube(t, i, j, k);
		/* On connaît donc le cube sur lequel on a cliqué. */
		
		trouver_face_la_plus_proche(res, &m, indice_face);
	}
	else{
		res=NULL;
	}
	return res;
}


/* La valeur de *indice_face donne une indication sur la face qui a été touchée:
   0<->face x sup;
   1<->face x inf;
   2<->face y sup;
   3<->face y inf;
   4<->face z sup;
   5<->face z inf.
*/
void trouver_face_la_plus_proche(struct cube*c, struct vecteur*pos, int*indice_face){
	
	/* Distances entre chaque face et le point pos. */
	float x_plus, x_moins, y_plus, y_moins, z_plus, z_moins;
	/* La plus petite de ces distances. */
	float min;
	
	cube_to_string(c);
	vecteur_to_string(pos);
	
	x_plus=(c->x+c->taille) - pos->x;
	x_moins=pos->x - (c->x-c->taille);
	y_plus=(c->y+c->taille) - pos->y;
	y_moins=pos->y - (c->y-c->taille);
	z_plus=(c->z+c->taille) - pos->z;
	z_moins=pos->z - (c->z-c->taille);
	
	min=min6(x_plus, x_moins, y_plus, y_moins, z_plus, z_moins);
	if(min==x_plus){
		*indice_face=0;
	}
	else if(min==x_moins){
		*indice_face=1;
	}
	else if(min==y_plus){
		*indice_face=2;
	}
	else if(min==y_moins){
		*indice_face=3;
	}
	else if(min==z_plus){
		*indice_face=4;
	}
	else if(min==z_moins){
		*indice_face=5;
	}
	else{
		printf("terrain: trouver_face_la_plus_proche: erreur...\n");
		*indice_face=-1;
	}
	return;
}




void sauvegarder_terrain(struct terrain*le_terrain, struct perso*le_perso){
	
	/* 1) créer un nom de fichier incluant la date et l'heure actuelle;
	   2) ouvrir un fichier en écriture avec ce nom;
	   3) écrire dans le fichier les caractéristiques du perso;
	   4) écrire dans le fichier les caractéristiques du terrain puis tous les cubes.
	*/
	char nom[50]="terrain.txt";
	FILE*fichier=fopen(nom, "r");
	
	
	
}

void charger_terrain(struct terrain*le_terrain, struct perso*le_perso){
	
	/* 1) ouvrir en lecture le fichier ayant le nom le plus récent;
	   3) lire les caractéristiques du perso, initialiser le perso;
	   4) lire les caractéristiques du terrain, initialiser le terrain; idem avec les cubes.
	*/
	printf("charger: TODO\n");
}




void reset_scale_gravity(struct terrain*t){
	scale_gravity(t, 1/t->gravity_scale);
}



/* Change the apparent size of the arrows displaying the gravity field. */
void scale_gravity(struct terrain*t, float coef){
	
	struct cube*c;
	int i, j, k;

	t->gravity_scale = t->gravity_scale * coef;

	/* Process each arrow. */
	for(i=0; i<t->longueur; i++){
		for(j=0; j<t->largeur; j++){
			for(k=0; k<t->hauteur; k++){
				c = get_cube(t, i, j, k);

				set_scale_arrow(&(c->arrow), t->gravity_scale);
			}
		}
	}
}


/* Test: compute the triangulation of half the terrain. */
void test_cut_cubes(struct terrain*t){
	
	int i, j, k;
	cube*c;

	vector point;
	vector normal;

	init_vecteur(&point, 0, 0, 0);
	init_vecteur(&normal, 1, 1, 1);

	for(i=0; i<t->longueur; i++){
		for(j=0; j<t->largeur/2; j++){
			for(k=0; k<t->hauteur; k++){
				c = get_cube(t, i, j, k);
				/* Cut each cube with the plane. */
				cut_cube(c, point, normal);
			}
		}
	}
}


void display_world_as_triangles(struct terrain*t,
				float xCam, float yCam, float zCam,
				float xCible, float yCible, float zCible,
				float xVertic, float yVertic, float zVertic,
				GLuint texture){

	int index;
	int i, j;
	float z;
	float z_min = t->z_min;
	float z_max = t->z_max;
	float z_int = 0.5*(z_min+z_max);
	/* Color: RGB and UV-mapping. */
	int r, g, b;
	float uv_u, uv_v;


	gluLookAt(xCam ,yCam, zCam,
		  xCam+xCible, yCam+yCible, zCam+zCible,
		  xVertic, yVertic, zVertic);

	
	/* Display each triangle with the appropriate texture. */
	glBegin(GL_TRIANGLES);
	for(i=0; i<t->nb_faces; i++){
		for(j=0; j<=2; j++){
			index = t->tab_faces[i][j];

			uv_u = t->tab_uv[index][0];
			uv_v = t->tab_uv[index][1];

			glTexCoord2d(uv_u, uv_v);
			glVertex3d(t->tab_points[index][0],
				   t->tab_points[index][1],
				   t->tab_points[index][2]);
		}
	}
	glEnd();
}

void test_load_world_file(struct terrain*t){
	
	printf("Loading terrain from file...\n");
	
	FILE*file = fopen("world_test.ply", "r");
	char s[200];
	char*word;
	int i, j;
	float x, y, z;
	float nx, ny, nz;
	float uv_u, uv_v;
	
	if(file == NULL){
		printf("error loading file\n");
		return;
	}
	
	t->triangles_only = 1;

	t->z_min = +1000000;
	t->z_max = -1000000;

	t->tab_points = NULL;
	t->tab_uv = NULL;
	t->tab_faces = NULL;
	t->nb_points = 0;
	t->nb_faces = 0;

	// type of file (ply)
	fgets(s, 200, file);
	// format (ascii 1.0)
	fgets(s, 200, file);
	// comment
	fgets(s, 200, file);
	// nb_points
	fgets(s, 200, file);
	word = strtok(s, " ");
	word = strtok(NULL, " ");
	word = strtok(NULL, " ");
	t->nb_points = atoi(word);
	t->tab_points = (float**)malloc(t->nb_points * sizeof(float*));
	for(i=0; i<t->nb_points; i++){
		t->tab_points[i] = (float*)malloc(3*sizeof(float));
	}

	t->tab_uv = (float**)malloc(t->nb_points * sizeof(float*));
	for(i=0; i<t->nb_points; i++){
		t->tab_uv[i] = (float*)malloc(2*sizeof(float));
	}


	/* Skip the lines describing the properties (x, y, z, nx, ny, nz, ...). */
	do{
		fgets(s, 200, file);
	}
	while(s[0] != 'e'); /* Stop at the "element face xxxxxx" line. */
	
	word = strtok(s, " ");
	word = strtok(NULL, " ");
	word = strtok(NULL, " ");
	t->nb_faces = atoi(word);
	t->tab_faces = (int**)malloc(t->nb_faces * sizeof(int*));
	for(i=0; i<t->nb_faces; i++){
		t->tab_faces[i] = (int*)malloc(3*sizeof(int));
	}

	fgets(s, 200, file);
	fgets(s, 200, file);

	/* Start reading the points. */
	for(i=0; i<t->nb_points; i++){
		fgets(s, 200, file);
		s[strlen(s)-1] = '\0';
		/* Position (x, y, z). */
		word = strtok(s, " ");
		x = atof(word);
		word = strtok(NULL, " ");
		y = atof(word);
		word = strtok(NULL, " ");
		z = atof(word);
		t->tab_points[i][0] = x;
		t->tab_points[i][1] = y;
		t->tab_points[i][2] = z;
		if(z > t->z_max){
			t->z_max = z;
		}
		if(z < t->z_min){
			t->z_min = z;
		}

		/* Local normal (nx, ny, nz). */
		word = strtok(NULL, " ");
		nx = atof(word);
		word = strtok(NULL, " ");
		ny = atof(word);
		word = strtok(NULL, " ");
		nz = atof(word);

		/* UV-coordinates. */
		word = strtok(NULL, " ");
		uv_u = atof(word);
		word = strtok(NULL, " ");
		uv_v = atof(word);
		t->tab_uv[i][0] = uv_u;
		t->tab_uv[i][1] = uv_v;
		
		word = strtok(NULL, " ");
	}
	
	
	/* Start reading the faces. */
	for(i=0; i<t->nb_faces; i++){
		fgets(s, 200, file);
		s[strlen(s)-1] = '\0';
		word = strtok(s, " ");
		// nb of points for the current face, value 3 is assumed.
		word = strtok(NULL, " ");
		x = atof(word);
		word = strtok(NULL, " ");
		y = atof(word);
		word = strtok(NULL, " ");
		z = atof(word);
		// printf("face %d, (%f, %f, %f);\n", i, x, y, z);
		t->tab_faces[i][0] = x;
		t->tab_faces[i][1] = y;
		t->tab_faces[i][2] = z;
	}
	fclose(file);
	printf("Terrain loaded: %d points, %d triangles.\n", t->nb_points, t->nb_faces);
	
}


void choose_color(float z, float z_min, float z_max,
		  int*r, int*g, int*b){

	int version = 1;

	switch(version){
	case 0:
		/* Simple version: */
		*r = 255*(z - z_min)/(z_max - z_min);
		*g = 0;
		*b = 255*(z - z_max)/(z_min - z_max);
		break;
	case 1:
		if(z < -5){
			*r = 0;
			*g = 0;
			*b = 10;
		}
		else if(z < -3){
			*r = 0;
			*g = 0;
			*b = 60;
		}
		else if(z < -2){
			*r = 0;
			*g = 0;
			*b = 100;
		}
		else if(z < -1){
			*r = 0;
			*g = 0;
			*b = 200;
		}
		else if(z <= 0){
			*r = 0;
			*g = 0;
			*b = 255;
		}
		else if(z < 1){
			*r = 0;
			*g = 255;
			*b = 10;
		}
		else if(z < 2){
			*r = 150;
			*g = 150;
			*b = 150;
		}
		else if(z < 3){
			*r = 100;
			*g = 100;
			*b = 100;
		}
		else{
			*r = 255;
			*g = 255;
			*b = 255;
		}
		break;
	default:
		*r = 128;
		*g = 0;
		*b = 0;
		break;
	}
	
}

			     

/* Get the distance betxeen the two points. */
float find_distance(float x0, float y0, float z0,
		    float x1, float y1, float z1){

	float dx = x0-x1;
	float dy = y0-y1;
	float dz = z0-z1;
	return dx*dx + dy*dy + dz*dz;
}



/* Find among the points of tab_points the one that is the closest to the coordinates (x, y, z);
   store the coordinates in the _res parameters. */
void find_closest_point(struct terrain*t,
			float x, float y, float z,
			float*x_res, float*y_res, float*z_res){

	int i_min = 0;
	int i;
	float distance_min = find_distance(x, y, z,
					   t->tab_points[0][0], t->tab_points[0][1], t->tab_points[0][2]);
	float distance;

	for(i=1; i<t->nb_points; i++){
		distance = find_distance(x, y, z,
					 t->tab_points[i][0], t->tab_points[i][1], t->tab_points[i][2]);
		if(distance < distance_min){
			distance_min = distance;
			i_min = i;
		}
	}
	// printf("d = %f\n", distance_min);
	
	*x_res = t->tab_points[i_min][0];
	*y_res = t->tab_points[i_min][1];
	*z_res = t->tab_points[i_min][2];
}


