#include "../BondLength.h"
#include "../AtomGroup.h"
#include "../Atom.h"

int main()
{
	AtomGroup g;
	Atom a, b;
	g += &a;
	g += &b;
	
	new BondLength(&g, &a, &b, 2);
	
	if (g.bondLengthCount() != 1 || g.bondstraintCount() != 1)
	{
		return 1;
	}

	return 0;
}
