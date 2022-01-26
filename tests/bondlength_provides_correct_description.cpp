#include "../src/BondLength.h"
#include "../src/AtomGroup.h"
#include "../src/Atom.h"

int main()
{
	AtomGroup g;
	Atom a, b, c;
	a.setAtomName("A");
	b.setAtomName("B");
	g += &a;
	g += &b;
	
	BondLength *l = new BondLength(&g, &a, &b, 2);
	
	std::string description = l->desc();
	std::string compare = "A-B";
	
	return !(compare == description);
}

