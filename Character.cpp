#include "Character.hpp"

Character::Character(): empty()
{
	
}




string Character::to_string(){
	return empty.to_string();
}

void Character::print(){

	cout << to_string() << endl;
}
