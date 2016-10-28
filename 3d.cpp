#include <iostream>
#include <string>
#include <sys/time.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>

#include "Constants.hpp"

#include "sdlglutils.c"
#include "Vector.hpp"
#include "Empty.hpp"
#include "Terrain.hpp"
#include "Camera.hpp"
#include "Character.hpp"
#include "sdlglutils.h"


using namespace std;

void read_input(int**pressed_keys_tab);
int delay_in_microseconds(struct timeval a, struct timeval b);


int main(){

	
	Camera my_cam;
	Character my_character;
	
	Terrain my_terrain("world_test.ply");


	int screen_height, screen_width;
	bool fullscreen = false;

	int*pressed_keys_tab;
	int i;

	int texture_terrain;

	/* Field of vision, in degrees. */
	float fov = 70;
	float near = 0.01;
	float far = 1000;

	struct timeval start_date, current_date;
	
	/* Display and computation frequency. */
	float period = 0.01; //0.05; /* Seconds */
	float fps = 1/period; /* Hertz */
	
	/* Display period. */
	int us_period = period * 1000 * 1000;
       	int elapsed_periods = 0;


	if(!(my_terrain.get_properly_loaded())){
		cout << "ERROR Terrain::Terrain(); exit." << endl;
		return -1;
	}
	
	
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
		SDL_SetVideoMode(screen_width/2, screen_height/2, 32, SDL_OPENGL | SDL_RESIZABLE);
	}
	

	pressed_keys_tab = (int*)malloc((SDLK_LAST+6)*sizeof(int));
	for(i=0; i<SDLK_LAST+6; i++){
		pressed_keys_tab[i]=0;
	}
	
	/* Activation du Z-Buffer. */
	glEnable(GL_DEPTH_TEST);
	
	/* Activation des textures 2d. */
	glEnable(GL_TEXTURE_2D);

	texture_terrain = loadTexture("uv_map_torus.jpeg", 0); //TODO: 2nd parameter is 1.
	// texture_terrain = loadTexture("uv_map_horizontal.png", 0); //TODO: 2nd parameter is 1.
	glBindTexture(GL_TEXTURE_2D, texture_terrain);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	
	if(fullscreen){
		gluPerspective(fov, ((double)screen_width)/((double)screen_height), near, far);
	}
	else{
		gluPerspective(fov, ((double)screen_width)/((double)screen_height), near, far);
	}	
	
	
	(pressed_keys_tab)[DX_MOUSE]=0;
	(pressed_keys_tab)[DY_MOUSE]=0;
	
	/* Ce flag continuer servira maintenant à boucler tant que l'utilisateur ne quitte pas. */
	(pressed_keys_tab)[LOOP]=1;
	
	gettimeofday(&start_date, NULL);
	printf("init start:   %d s %d µs\n", (int)start_date.tv_sec, (int)start_date.tv_usec);
	gettimeofday(&current_date, NULL);
	printf("init current: %d s %d µs\n", (int)current_date.tv_sec, (int)current_date.tv_usec);


	my_character.set_camera(&my_cam);
	
	
	/* Fin init globale. */
	


	/**********************/
	/* BOUCLE PRINCIPALE. */
	/**********************/
	i=0;
	while((pressed_keys_tab)[LOOP]){
		i++;
		
		/* Render the image and process the events. */
		gettimeofday(&current_date, NULL);
		if(delay_in_microseconds(current_date, start_date) > us_period * elapsed_periods){
			 
			

			/* Rendering. */
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			glMatrixMode(GL_MODELVIEW);
			glLoadIdentity();

			my_terrain.render(&my_cam, texture_terrain);
			my_character.render();
			
			glFlush();
			SDL_GL_SwapBuffers();
			/* End of rendering. */

			
			elapsed_periods++;
			
			
		
			read_input(&pressed_keys_tab);
		
			/* Quitter. */
			if(pressed_keys_tab[SDL_QUIT]==1){
				printf("SDL_QUIT\n");
				(pressed_keys_tab)[LOOP]=0;
			}
			
			// if(pressed_keys_tab[SDLK_LCTRL] == 1){
			//	printf("\t ctrl\n");
			// }
			if(pressed_keys_tab[SDLK_ESCAPE] == 1){
				printf("\t esc\n");
			}

			/* Terminating program with Alt+F4 */
			if(pressed_keys_tab[SDLK_LALT]==1 && pressed_keys_tab[SDLK_F4]==1){
				printf("ALt+F4\n");
				pressed_keys_tab[LOOP] = 0;
			}


			/* Move the character with the keyboard and mouse. */
			my_character.update_speed(&(pressed_keys_tab), period);
			my_character.update_position(&(pressed_keys_tab), period);

			/* Move the camera with the keyboard and mouse. */
			my_cam.update_position(&(pressed_keys_tab), period);
			
			
		
			//evoluer_terrain(&my_terrain, &pressed_keys_tab, &my_character);
			my_terrain.evolve(&pressed_keys_tab, &my_character);

			my_terrain.apply_gravity(&my_character, period);
			/*
			  appliquer_gravite_locamy_terrain_sur_perso(&my_terrain, &my_character, period);
			  //attendre(1);
			  }
			  else{
			  appliquer_gravite_globamy_terrain_sur_perso(&my_terrain, &my_character, period);
			  }
			*/

			my_terrain.collide(&my_character, period);
			// calculer_collision_terrain_perso(&my_terrain, &my_character, period);
		
			/* La caméra ne doit pas se retrouver coincée à l'intérieur d'un mur. */
			// my_cam.avoid_walls(&my_terrain);
			// avancer_cam_pour_mur(&my_cam, &my_terrain);
		
			/* Si on a avancé la caméra pour la faire sortir du mur, il faut la remettre là où elle était avant. */
			// reculer_cam_pour_mur(&my_cam);

			/* Saving and loading
			// Saving: Ctrl+S
			if(pressed_keys_tab[SDLK_LCTRL] && pressed_keys_tab[SDLK_s]){
			sauvegarder_terrain(&my_terrain, &my_character);
			pressed_keys_tab[SDLK_s]=0;
			}
			// Loading: Ctrl+L
			if(pressed_keys_tab[SDLK_LCTRL] && pressed_keys_tab[SDLK_l]){
			charger_terrain(&my_terrain, &my_character);
			pressed_keys_tab[SDLK_l]=0;
			}
			*/

		}
	}

	/* End of main loop. */
	
	return 0;
}




void read_input(int**pressed_keys_tab){
	
	SDL_Event e;
	/*
	  printf("lire\n");
	*/
	while(SDL_PollEvent(&e)!=0){
		
		switch(e.type){
		case SDL_QUIT:
			printf("SDL_QUIT détecté\n");
			(*pressed_keys_tab)[SDL_QUIT]=1;
			(*pressed_keys_tab)[LOOP]=0;
			break;
		case SDL_MOUSEMOTION:
			/* Les cases pressed_keys_tab[DX_MOUSE] et pressed_keys_tab[DY_MOUSE] mémorisent le déplacement total de la souris
			   depuis la dernière lecture par l'autre thread. */
			(*pressed_keys_tab)[DX_MOUSE]=(*pressed_keys_tab)[DX_MOUSE]+e.motion.xrel;
			(*pressed_keys_tab)[DY_MOUSE]=(*pressed_keys_tab)[DY_MOUSE]+e.motion.yrel;
		
			/* Bidouille... qui fonctionne... */
			e.motion.xrel=0;
			e.motion.yrel=0;
			break;
			
		case SDL_MOUSEBUTTONDOWN:
			/*//////////////////////////////////////////////////////////////////////////////////////////////////////////////// */
			
			switch(e.button.button){
			case SDL_BUTTON_LEFT:
				(*pressed_keys_tab)[LEFT_CLICK_ONGOING]=1;
				break;
			case SDL_BUTTON_MIDDLE:
				break;
			case SDL_BUTTON_RIGHT:
				(*pressed_keys_tab)[RIGHT_CLICK_ONGOING]=1;
				break;
			case SDL_BUTTON_WHEELUP:
				(*pressed_keys_tab)[WHEEL_LEVEL]++;
				printf("Wheel level: %d\n", (*pressed_keys_tab)[WHEEL_LEVEL]);
				break;
			case SDL_BUTTON_WHEELDOWN:
				(*pressed_keys_tab)[WHEEL_LEVEL]--;
				printf("Wheel level: %d\n", (*pressed_keys_tab)[WHEEL_LEVEL]);
				break;
			}
		case SDL_KEYDOWN:
			/* GESTION DES APPUIS SUR LES TOUCHES. */
			(*pressed_keys_tab)[e.key.keysym.sym]=1;
			/* FIN DE LA GESTION DES APPUIS SUR LES TOUCHES. */
			break;

		case SDL_KEYUP:
			printf("keyup\n");
			/* GESTION DES RELÂCHEMENTS DES TOUCHES. */
			(*pressed_keys_tab)[e.key.keysym.sym]=0;
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

/*****************************************************************/
/* Compute the difference (a-b) and give result in microseconds. */
/*****************************************************************/
int delay_in_microseconds(struct timeval a, struct timeval b){
	int res = 1000000*(a.tv_sec - b.tv_sec)  +  (a.tv_usec - b.tv_usec);

	// printf("res = %d\n", res);

	return res;
}
