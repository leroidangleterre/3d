void afficher_tab(int**tab, int N){
	int i;
	
	for(i=0; i<N; i++){
		if((*tab)[i]==0){
			printf("-");
		}
		else{
			printf("1");
		}
	}
	printf("\n");
}
