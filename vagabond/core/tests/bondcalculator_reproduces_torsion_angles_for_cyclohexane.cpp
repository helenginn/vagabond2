#include "../AtomGroup.h"
#include "../BondSequence.h"
#include "../CifFile.h"
#include "../BondTorsion.h"

int main()
{
	std::string path = "/assets/geometry/CHX.cif";

	CifFile geom = CifFile(path);
	geom.parse();
	
	AtomGroup &chx = *geom.atoms();
	chx.recalculate();

	bool bad = false;
	for (size_t i = 0; i < chx.bondTorsionCount(); i++)
	{
		BondTorsion *t = chx.bondTorsion(i);

		double init = t->measurement(BondTorsion::SourceInitial);
		double derived = t->measurement(BondTorsion::SourceDerived);
		
		while (fabs(derived - 360 - init) < fabs(derived - init))
		{
			derived -= 360;
		}

		while (fabs(derived + 360 - init) < fabs(derived - init))
		{
			derived += 360;
		}
		
		if (fabs(init - derived) > 5)
		{
			bad = true;
			std::cout << "Problem for torsion " << t->desc() << ", "
			"init angle " << init << " does not match " <<
			derived << std::endl;
		}
		else
		{
			std::cout << "MATCH for torsion " << t->desc() << ", "
			"init angle " << init << " matches " <<
			derived << std::endl;

		}
	}
	
	if (bad)
	{
		BondSequence *sequence = new BondSequence();
		sequence->addToGraph(chx.possibleAnchor(0), UINT_MAX);
		int added = sequence->addedAtomsCount();
		
		std::cout << "Added " << added << " atoms." << std::endl;

		for (size_t i = 0; i < sequence->atomGraphCount(); i++)
		{
			std::cout << sequence->atomGraphDesc(i);
		}
		
		delete sequence;

	}
	
	return bad;
}
