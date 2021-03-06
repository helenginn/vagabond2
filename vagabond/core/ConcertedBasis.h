// vagabond
// Copyright (C) 2019 Helen Ginn
// 
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.
// 
// Please email: vagabond @ hginn.co.uk for more details.

#ifndef __vagabond__ConcertedBasis__
#define __vagabond__ConcertedBasis__

#include "TorsionBasis.h"
#include "../utils/svd/PCA.h"
#include <vagabond/core/Residue.h>

class Molecule;
class Residue;

class ConcertedBasis : public TorsionBasis
{
public:
	ConcertedBasis();
	~ConcertedBasis();

	virtual float torsionForVector(int idx, const float *vec, int n);
	virtual void prepare();

	/** only the torsions available in the mask will be used for calculating
	 *  principle axes of motion */
	virtual void supplyMask(std::vector<bool> mask);
	
	size_t activeBonds();
	
	void setCustom(bool custom)
	{
		_custom = custom;
	}
	
	void fillFromMoleculeList(Molecule *molecule, int axis,
	                          const std::vector<ResidueTorsion> &list,
	                          const std::vector<float> &values);

	const std::vector<BondTorsion *> &missingBonds() const
	{
		return _missing;
	}
	
	Residue *unusedTorsion()
	{
		return _unusedId;
	}
private:
	void prepareSVD();
	void setupAngleList();
	std::vector<bool> _refineMask;

	std::vector<BondTorsion *> _missing;
	Residue *_unusedId{};

	bool _custom = false;
	
	/* one idx per every torsion angle; >=0 if refined, -1 if not refined 
	 * where value < _nActive*/
	std::vector<int> _idxs;
	size_t _nActive;

	std::vector<BondTorsion *> _filtered;
	PCA::SVD _svd{};
};

#endif
