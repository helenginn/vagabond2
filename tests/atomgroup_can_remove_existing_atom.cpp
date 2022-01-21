#include "../src/Atom.h"
#include "../src/AtomGroup.h"

int main()
{
	Atom *atom = new Atom();
	Atom *other = new Atom();

	AtomGroup group;
	group += atom;
	group += other;
	
	group -= other;

	return !(group.size() == 1);
}





