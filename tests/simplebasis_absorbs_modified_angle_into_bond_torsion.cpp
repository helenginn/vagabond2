#include "../src/SimpleBasis.h"
#include "../src/BondTorsion.h"
#include "../src/Atom.h"

int main()
{
	Atom a, b, c, d;
	BondTorsion *torsion = new BondTorsion(nullptr, &a, &b, &c, &d, 2);
	torsion->setRefinedAngle(100);

	SimpleBasis sb;
	sb.addTorsion(torsion);
	sb.prepare();
	
	float custom = 10;
	sb.absorbVector(&custom, 1);

	return !(fabs(torsion->startingAngle() - 110) <= 1e-6);
}
