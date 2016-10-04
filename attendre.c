void attendre(float nb_sec){
	int date_debut=clock();
	while(clock()-date_debut<nb_sec*CLOCKS_PER_SEC){
		/* Attendre. */
	}
	return;
}
