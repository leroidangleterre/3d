#include "Empty.hpp"


Empty::Empty() : origin(0, 0, 0), target(1, 0, 0), left(0, 1, 0), vertic(0, 0, 1)
{

}


string Empty::to_string(){
	return ("Empty: {"
		+ origin.to_string() + " ; "
		+ target.to_string() + " ; "
		+ left.to_string() + " ; "
		+ vertic.to_string() + "}");
}

void Empty::print(){
	cout << to_string() << endl;
}
