#include"includes.c"


void lire_input(int**tab_appuis);
void deplacer_perso(struct perso*p, int*tab_appuis, float dt);
void rendu(struct terrain*le_terrain, struct camera*la_cam, struct perso*le_perso, GLuint texture);
void evoluer_terrain();
void collisions_perso_terrain();
int delay_in_microseconds(struct timeval a, struct timeval b);



int main(int argc, char*argv[]){
	
	/* If this flag is off (0), the gravity is uniform and its value is (gx, gy, gz);
	   If this flag is on (1), each cube applies a gravitational force on each other,
	   so that when the character is located in an empty cube, it is pulled in a direction
	   that depends on the rest of the world;
	   in that case, the local gravity of each cube must be computed at launch
	   (and when the world is modified). */
	int use_local_gravity = 0;


	/* Fullscreen or not fullscreen
	   (it is possible to switch between fullscreen and window mode). */
	int fullscreen = 0;
	
	
	/* Taille de la fenêtre.
	   On règlera ces valeurs une fois que l'on sera passé en plein écran. */
	int largeur_fenetre = 1500;
	int hauteur_fenetre = 800;

	/* Size of the screen of the device (will not be modified). */
	int screen_width;
	int screen_height;

	/* Dimensions du terrain. */
	int n = 60;
	int longueur = n;
	int largeur  = n;
	int hauteur  = n;
	/* Le point central du terrain. */
	float x_centre, y_centre, z_centre;
	
	/* Paramètres des cubes. */
	float taille_cubes  = 1.0;
	float restit_vertic = 0.0;
	float restit_horiz  = 1.0; //0.4;
	
	/* Caméra. */
	struct camera la_cam;
	float distance_a_perso_std = 5;
	
	/* Perso. */
	struct perso le_perso;
	/* Coordonnées initiales du perso. */
	//float x_perso_init = longueur*taille_cubes/2, y_perso_init = largeur*taille_cubes/2, z_perso_init = hauteur*taille_cubes/2;
	float x_perso_init = 0, y_perso_init = 0, z_perso_init = 3;
	float vitesse_de_saut = 6;
	float vitesse_de_mvt  = 15;
	
	/* Fréquence d'affichage et de calcul. */
	float periode = 0.01; //0.05; /* Seconds */
	float fps = 1/periode; /* Hertz */
	/* Period of the display. */
	int us_period = periode * 1000 * 1000;
       	int elapsed_periods = 0;


	
	/* La date en cours. */
	struct timeval start_date, current_date;
	
	/* Gravité standard du terrain. */
	float gx = 0;
	float gy = 0;
	float gz = 0; //-9;
	
	float fov = 70; /* Champ de vision horizontal, en degrés. */
	/* Limites proche et lointaine du cône de vision. */
	float near = 1;
	float far = 1000;
	
	
	struct terrain le_terrain;
	GLuint texture;
	
	int*tab_appuis;
	int i;
	
	enum mode mode = MODE_JEU;
	
	float angle_rotation = 0.5;
	float distance_deplacement = 0.35;
	
	
	int gravity_still_todo = use_local_gravity;
	
	/* Début init globale. */
	
	
	/* Initialiser la position du personnage. */
	init_perso(&le_perso,
		   x_perso_init, y_perso_init, z_perso_init,
		   angle_rotation, distance_deplacement,
		   vitesse_de_saut, vitesse_de_mvt);
	init_terrain(&le_terrain, longueur, largeur, hauteur, taille_cubes, restit_vertic, restit_horiz, &mode, use_local_gravity);
	init_gravite_terrain(&le_terrain, gx, gy, gz);
	
	get_centre_terrain(&le_terrain, &x_centre, &y_centre, &z_centre);
	
	/* Lier le perso et la caméra, pour que chacun connaisse l'autre;
	   positionner la caméra derrière le perso. */
	la_cam.distance_a_perso=distance_a_perso_std;
	attacher_cam_a_perso(&le_perso, &la_cam, la_cam.distance_a_perso);
	

	/* Init SDL. */

	SDL_Init(SDL_INIT_VIDEO);

	const SDL_VideoInfo* info = SDL_GetVideoInfo();
	screen_width = info->current_w;
	screen_height = info->current_h;
	printf("screen: w = %d, h = %d\n", screen_width, screen_height);


	SDL_WM_SetCaption("SDL GL Application", NULL);
	
	
	if(fullscreen){
		/* Fullscreen. */
		
		SDL_WM_GrabInput(SDL_GRAB_ON);
		SDL_ShowCursor(0);
		
		SDL_SetVideoMode(screen_width, screen_height, 32, SDL_OPENGL | SDL_RESIZABLE | SDL_FULLSCREEN);
		
	}
	else{
		/* Non fullscreen. */
		SDL_SetVideoMode(largeur_fenetre, hauteur_fenetre, 32, SDL_OPENGL | SDL_RESIZABLE);
	}
	
	
	/* Les constantes suivantes ont des valeurs qui vont de SDLK_LAST à SDLK_LAST+5;
	   elles sont définies dans le fichier "includes.c".
	   DX_SOURIS
	   DY_SOURIS
	   CONTINUER
	   NIVEAU_MOLETTE
	   CLIC_GAUCHE_ACTIF
	   CLIC_DROIT_ACTIF
	*/
	tab_appuis=(int*)malloc((SDLK_LAST+6)*sizeof(int));
	for(i=0; i<SDLK_LAST+6; i++){
		tab_appuis[i]=0;
	}
	
	/* Activation du Z-Buffer. */
	glEnable(GL_DEPTH_TEST);
	
	/* Activation des textures 2d. */
	glEnable(GL_TEXTURE_2D);

	texture = loadTexture("uv_map_torus.jpeg", 0); //TODO: 2nd parameter is 1.
	glBindTexture(GL_TEXTURE_2D, texture);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	
	if(fullscreen){
		gluPerspective(fov, ((double)screen_width)/((double)screen_height), near, far);
	}
	else{
		gluPerspective(fov, ((double)largeur_fenetre)/((double)hauteur_fenetre), near, far);
	}	
	
	
	(tab_appuis)[DX_SOURIS]=0;
	(tab_appuis)[DY_SOURIS]=0;
	
	/* Ce flag continuer servira maintenant à boucler tant que l'utilisateur ne quitte pas. */
	(tab_appuis)[CONTINUER]=1;
	
	gettimeofday(&start_date, NULL);
	printf("init start:   %d s %d µs\n", (int)start_date.tv_sec, (int)start_date.tv_usec);
	gettimeofday(&current_date, NULL);
	printf("init current: %d s %d µs\n", (int)current_date.tv_sec, (int)current_date.tv_usec);

	
	/* Fin init globale. */
	
	
	/**********************/
	/* BOUCLE PRINCIPALE. */
	/**********************/
	i=0;
	while((tab_appuis)[CONTINUER]){
		i++;
		
		/* date=clock(); */

		/******************************************************/
		/* Affichage.                                         */
		/* On n'effectue l'affichage que si on a suffisamment */
		/* attendu depuis l'affichage précédent.              */
		/******************************************************/
		gettimeofday(&current_date, NULL);
		if(delay_in_microseconds(current_date, start_date) > us_period * elapsed_periods){
			 
			// printf("start: %d s %d µs\n", (int)start_date.tv_sec, (int)start_date.tv_usec);
			// printf("current: %d s %d µs\n", (int)current_date.tv_sec, (int)current_date.tv_usec);
			// printf("elapsed_periods = %d, elapsed seconds = %d:%d\n", elapsed_periods, (int)(current_date.tv_sec - start_date.tv_sec), (int)(current_date.tv_usec - start_date.tv_usec));
			// printf("periode = %f\n", periode);
			// printf("us_period = %d\n", us_period);
			// printf("fps = %f\n", fps);
			

			rendu(&le_terrain, &la_cam, &le_perso, texture);
			elapsed_periods++;
			
			
		
			lire_input(&tab_appuis);
		
			/* Quitter. */
			if(tab_appuis[SDL_QUIT]==1){
				printf("SDL_QUIT\n");
				(tab_appuis)[CONTINUER]=0;
			}
			
			if(tab_appuis[SDLK_LCTRL] == 1){
				printf("\t ctrl\n");
			}
			if(tab_appuis[SDLK_ESCAPE] == 1){
				printf("\t esc\n");
			}
			/*
			  if(fullscreen){
			  Fullscreen.
			  
			  SDL_WM_GrabInput(SDL_GRAB_ON);
			  SDL_ShowCursor(0);
			  
			  SDL_SetVideoMode(screen_width, screen_height, 32, SDL_OPENGL | SDL_RESIZABLE | SDL_FULLSCREEN);
			  
			  }
			  else{
			  Non fullscreen.
			  SDL_SetVideoMode(largeur_fenetre, hauteur_fenetre, 32, SDL_OPENGL | SDL_RESIZABLE);
			  }
			*/
			/* Escape: switch between fullscreen and window mode. */
			/*	
			if(tab_appuis[SDLK_ESCAPE]==1){
				if(fullscreen){
					
					SDL_Init(SDL_INIT_VIDEO);

					printf("switching to non-fullscreen.\n");
					SDL_WM_GrabInput(SDL_GRAB_OFF);
					SDL_ShowCursor(1);
					SDL_SetVideoMode(largeur_fenetre, hauteur_fenetre, 32, SDL_OPENGL | SDL_RESIZABLE);
					fullscreen = 0;
				}
				else{
					// Switch back to fullscreen.
					SDL_Init(SDL_INIT_VIDEO);

					printf("switching to fullscreen (%d x %d).\n", screen_width, screen_height);
					SDL_WM_GrabInput(SDL_GRAB_ON);
					SDL_ShowCursor(0);
					SDL_SetVideoMode(screen_width, screen_height, 32, SDL_OPENGL | SDL_RESIZABLE | SDL_FULLSCREEN);
					fullscreen = 1;
				}
				tab_appuis[SDLK_ESCAPE] = 0;
				
			}
			*/


			/* Terminating program with Alt+F4 */
			if(tab_appuis[SDLK_LALT]==1 && tab_appuis[SDLK_F4]==1){
				printf("ALt+F4\n");
				tab_appuis[CONTINUER] = 0;
			}

		
			deplacer_perso(&le_perso, tab_appuis, periode);
		
			/* On modifie la distance standard entre la cam et le perso. */
			avancer_camera_vers_perso_selon_molette(&la_cam, &(tab_appuis[NIVEAU_MOLETTE]));
		
		
			evoluer_terrain(&le_terrain, &tab_appuis, &le_perso);
			if(use_local_gravity){
				appliquer_gravite_locale_terrain_sur_perso(&le_terrain, &le_perso, periode);
				//attendre(1);
			}
			else{
				appliquer_gravite_globale_terrain_sur_perso(&le_terrain, &le_perso, periode);
			}

			calculer_collision_terrain_perso(&le_terrain, &le_perso, periode);
		
			/* La caméra ne doit pas se retrouver coincée à l'intérieur d'un mur. */
			avancer_cam_pour_mur(&la_cam, &le_terrain);
		
			/* Si on a avancé la caméra pour la faire sortir du mur, il faut la remettre là où elle était avant. */
			reculer_cam_pour_mur(&la_cam);
		
			/* Si on appuie en même temps sur CTRL et S, alors on sauvegarde le terrain. */
			if(tab_appuis[SDLK_LCTRL] && tab_appuis[SDLK_s]){
				sauvegarder_terrain(&le_terrain, &le_perso);
				tab_appuis[SDLK_s]=0;
			}
			/* On charge la dernière sauvegarde en appuyant simultanément sur CTRL et L. */
			if(tab_appuis[SDLK_LCTRL] && tab_appuis[SDLK_l]){
				charger_terrain(&le_terrain, &le_perso);
				tab_appuis[SDLK_l]=0;
			}

			if(tab_appuis[SDLK_g] || gravity_still_todo){
				/* Compute gravity. */
			
				/* Switch to non-fullscreen mode before launching that operation.
				   TODO: give an estimation of the time needed, make a loading bar, or enable the user to quit.
				   Create a popup with a loading bar and an event handling.
				*/

				printf("computing gravity\n");
				/* Spend one second computing gravity and exit;
				   at each time, if the computation is not over, display a loading bar (or loading information);
				   Once every second, the flow gets back to the events handling. */
				gravity_still_todo = compute_local_gravity_terrain(&le_terrain);
				
				if(gravity_still_todo){
					printf("still something to do...\n");
				}
				else{
					printf("gravity_done\n");
				}
				
				tab_appuis[SDLK_g] = 0;
			}
			/* Changer la taille apparente des vecteurs gravité. */
			if(tab_appuis[SDLK_KP_PLUS]){
				scale_gravity(&le_terrain, 1.1);
				tab_appuis[SDLK_KP_PLUS] = 0;
			}
			if(tab_appuis[SDLK_KP_MINUS]){
				scale_gravity(&le_terrain, 1/1.1);
				tab_appuis[SDLK_KP_MINUS] = 0;
			}
		}
	}
	
	/**************************/
	/* FIN BOUCLE PRINCIPALE. */
	/**************************/
	
	return 0;
}



void lire_input(int**tab_appuis){
	
	SDL_Event e;
	/*
	  printf("lire\n");
	*/
	while(SDL_PollEvent(&e)!=0){
		
		switch(e.type){
		case SDL_QUIT:
			printf("SDL_QUIT détecté\n");
			(*tab_appuis)[SDL_QUIT]=1;
			(*tab_appuis)[CONTINUER]=0;
			break;
		case SDL_MOUSEMOTION:
			/* Les cases tab_appuis[DX_SOURIS] et tab_appuis[DY_SOURIS] mémorisent le déplacement total de la souris
			   depuis la dernière lecture par l'autre thread. */
			(*tab_appuis)[DX_SOURIS]=(*tab_appuis)[DX_SOURIS]+e.motion.xrel;
			(*tab_appuis)[DY_SOURIS]=(*tab_appuis)[DY_SOURIS]+e.motion.yrel;
		
			/* Bidouille... qui fonctionne... */
			e.motion.xrel=0;
			e.motion.yrel=0;
			break;
			
		case SDL_MOUSEBUTTONDOWN:
			/*//////////////////////////////////////////////////////////////////////////////////////////////////////////////// */
			
			switch(e.button.button){
			case SDL_BUTTON_LEFT:
				(*tab_appuis)[CLIC_GAUCHE_ACTIF]=1;
				break;
			case SDL_BUTTON_MIDDLE:
				break;
			case SDL_BUTTON_RIGHT:
				(*tab_appuis)[CLIC_DROIT_ACTIF]=1;
				break;
			case SDL_BUTTON_WHEELUP:
				(*tab_appuis)[NIVEAU_MOLETTE]++;
				printf("niveau molette: %d\n", (*tab_appuis)[NIVEAU_MOLETTE]);
				break;
			case SDL_BUTTON_WHEELDOWN:
				(*tab_appuis)[NIVEAU_MOLETTE]--;
				printf("niveau molette: %d\n", (*tab_appuis)[NIVEAU_MOLETTE]);
				break;
			}
			/*/////////////////////////////////////////////////////////////////////////////////////////////////////////////////*/
			/*
			  case SDL_MOUSEBUTTONUP:
			  switch(e.button.button){
			  case SDL_BUTTON_LEFT:
			  (*tab_appuis)[CLIC_GAUCHE_ACTIF]=0;
			  break;
			  default:
			  break;
			  }
			*/
			
		case SDL_KEYDOWN:
			/* GESTION DES APPUIS SUR LES TOUCHES. */
			(*tab_appuis)[e.key.keysym.sym]=1;
			/* FIN DE LA GESTION DES APPUIS SUR LES TOUCHES. */
			break;

		case SDL_KEYUP:
			printf("keyup\n");
			/* GESTION DES RELÂCHEMENTS DES TOUCHES. */
			(*tab_appuis)[e.key.keysym.sym]=0;
			/* FIN DE LA GESTION DES RELÂCHEMENTS DES TOUCHES. */
			break;
	
		case SDL_VIDEORESIZE:
			printf("resize\n");
			/* On actualise le mode vidéo, avec le nouveau format de la fenêtre. */
			//if(TODO: check for fullscreen){
			SDL_SetVideoMode(e.resize.w, e.resize.h, 32, SDL_OPENGL | SDL_RESIZABLE | SDL_FULLSCREEN);
			/*}
			else{
				SDL_SetVideoMode(e.resize.w, e.resize.h, 32, SDL_OPENGL | SDL_RESIZABLE);
				}*/	
			break;
		
		default: break;
		}
	}
	
}			     

/******************************************************/
/* Affichage.                                         */
/* On n'effectue l'affichage que si on a suffisamment */
/* attendu depuis l'affichage précédent.              */
/******************************************************/
void rendu(struct terrain*le_terrain, struct camera*la_cam, struct perso*le_perso, GLuint texture){
	
	// printf("\trendering...\n");

	/* Début affichage. */
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	// printf("\nafficher_terrain\n");
	
	glEnable(GL_TEXTURE_2D);
	afficher_terrain(le_terrain, *la_cam, texture);
	glDisable(GL_TEXTURE_2D);
	
	// afficher_perso(le_perso, *la_cam);
	
	/* Rafraîchir l'écran. */
	glFlush();
	SDL_GL_SwapBuffers();
	/* Fin affichage. */

	// printf("\tend render.\n");
}

/*****************************************************************/
/* Compute the difference (a-b) and give result in microseconds. */
/*****************************************************************/
int delay_in_microseconds(struct timeval a, struct timeval b){
	int res = 1000000*(a.tv_sec - b.tv_sec)  +  (a.tv_usec - b.tv_usec);

	// printf("res = %d\n", res);

	return res;
}
