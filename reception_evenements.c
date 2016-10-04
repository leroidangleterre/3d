void reception_evenements(int**tab_appuis){
	
	SDL_Event e;

	SDL_PollEvent(&e);
	
	switch(e.type){
	case SDL_QUIT:
		printf("SDL_QUIT détecté\n");
		(*tab_appuis)[SDL_QUIT]=1;
		break;
			
	case SDL_MOUSEMOTION:
		/* Les cases tab_appuis[DX_SOURIS] et tab_appuis[DY_SOURIS] mémorisent le déplacement total de la souris
		   depuis la dernière lecture par l'autre thread. */
		(*tab_appuis)[DX_SOURIS]=(*tab_appuis)[DX_SOURIS]+e.motion.xrel;
		(*tab_appuis)[DY_SOURIS]=(*tab_appuis)[DY_SOURIS]+e.motion.yrel;
		break;
		
	case SDL_KEYDOWN:
		/* GESTION DES APPUIS SUR LES TOUCHES. */
		(*tab_appuis)[e.key.keysym.sym]=1;
		/* FIN DE LA GESTION DES APPUIS SUR LES TOUCHES. */
		break;
		
	case SDL_KEYUP:
		/* GESTION DES RELÂCHEMENTS DES TOUCHES. */
		(*tab_appuis)[e.key.keysym.sym]=0;
		/* FIN DE LA GESTION DES RELÂCHEMENTS DES TOUCHES. */
		break;

	case SDL_VIDEORESIZE:
		/* On actualise le mode vidéo, avec le nouveau format de la fenêtre. */
		SDL_SetVideoMode(e.resize.w, e.resize.h, 32, SDL_OPENGL | SDL_RESIZABLE | SDL_FULLSCREEN);
		break;
	}
}
