#include "../src/Atom.h"

int main()
{
	Atom *atom = new Atom();
	std::string test = "na";
	
	atom->setElementSymbol(test);
	
	std::string result = atom->elementSymbol();
	
	return !(result == "NA");
}
