#include "../AtomGroup.h"
#include "../Grapher.h"
#include "../CifFile.h"
#include "../BondTorsion.h"

int main()
{
	std::string path = "/assets/geometry/TYR.cif";

	CifFile geom = CifFile(path);
	geom.parse();
	
	AtomGroup &tyrosine = *geom.atoms();
	tyrosine.recalculate();

	bool bad = false;
	for (size_t i = 0; i < tyrosine.bondTorsionCount(); i++)
	{
		BondTorsion *t = tyrosine.bondTorsion(i);

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
		Grapher gr;
		gr.generateGraphs(tyrosine.possibleAnchor(0), UINT_MAX);
		int added = gr.atoms().size();
		
		std::cout << "Added " << added << " atoms." << std::endl;

		for (size_t i = 0; i < gr.graphCount(); i++)
		{
			std::cout << gr.graph(i)->desc();
		}
	}
	
	return bad;
}
