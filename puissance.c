
/* Calcul de a puissance b
   pour des valeurs positives entières de a et b. */
int puissance(int a, int b){
	if(b==0){
		return 1;
	}
	else if(b<0){
		return 0;
	}
	else{
		return a*puissance(a, b-1);
	}
}
