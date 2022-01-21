#include "../src/Cif2Geometry.h"
#include "../src/GeometryTable.h"

int main()
{
	Cif2Geometry *geom = new Cif2Geometry("");
	GeometryTable *table = geom->geometryTable();
	delete geom;
	
	bool exists = table->lengthExists("", "", "");
	
	return !(exists == false);
}
