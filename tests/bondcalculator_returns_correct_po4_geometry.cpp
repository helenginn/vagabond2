#include "../src/Cif2Geometry.h"
#include "../src/AtomGroup.h"
#include "../src/BondAngle.h"
#include "../src/BondLength.h"
#include "../src/BondCalculator.h"
#include "bondcalculator_checks_geometry_for_ligand.h"

int main()
{
	std::string path = "/assets/geometry/PO4.cif";

	return checkGeometry(path, 0.1);
}
