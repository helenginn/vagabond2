#include "../CifFile.h"
#include "../AtomGroup.h"
#include "../BondSequence.h"

int main()
{
	std::string path = "/assets/geometry/GLY.cif";

	CifFile geom = CifFile(path);
	geom.parse();
	
	AtomGroup *atoms = geom.atoms();
	Atom *anchor = atoms->firstAtomWithName("N");

	BondSequence *sequence = new BondSequence();
	sequence->addToGraph(anchor);

	Atom *a = sequence->atomForBlock(0);
	Atom *b = sequence->atomForBlock(1);
	
	if (a != nullptr || b != anchor)
	{
		return 1;
	}

	delete atoms;

	return 0;
}
