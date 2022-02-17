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

#include "RefList.h"
#include "matrix_functions.h"
#include <iostream>

RefList::RefList(const std::vector<Reflection> &refls)
{
	_refls = refls;
}

void RefList::setSpaceGroup(int num)
{
	_spg = CSym::ccp4spg_load_by_ccp4_num(num);
	extractSymops();
}

void RefList::extractSymops()
{
	delete [] _rots;
	delete [] _trans;
	_rots = nullptr;
	_trans = nullptr;
	
	if (_spg == NULL)
	{
		_nsymops = 0;
		return;
	}

	_nsymops = _spg->nsymop;
	_rots = new glm::mat3[_nsymops];
	_trans = new glm::vec3[_nsymops];
	
	for (size_t i = 0; i < _nsymops; i++)
	{
		CSym::ccp4_symop &s = _spg->symop[i];
		_rots[i] = glm::mat3x3(s.rot[0][0], s.rot[0][1], s.rot[0][2],
		                       s.rot[1][0], s.rot[1][1], s.rot[1][2],
		                       s.rot[2][0], s.rot[2][1], s.rot[2][2]);

		_trans[i] = glm::vec3(s.trn[0], s.trn[1], s.trn[2]);
	}
}

HKL RefList::maxHKL()
{
	HKL hkl{};
	
	for (size_t i = 0; i < _refls.size(); i++)
	{
		for (size_t j = 0; j < 3; j++)
		{
			if (abs(_refls[i].hkl[j]) > hkl[j])
			{
				hkl[j] = abs(_refls[i].hkl[j]);
			}
		}

	}

	return hkl;
}

glm::vec3 RefList::applyRotSym(const glm::vec3 v, const int i)
{
	glm::vec3 w = glm::transpose(_frac2Real) * v;
	w = _rots[i] * w;
	w = glm::transpose(_recip2Frac) * w;
	return w;
}

const double RefList::resolutionOf(glm::vec3 v) const
{
	double l = glm::length(v);
	return 1 / l;
}

const glm::vec3 RefList::reflAsIndex(const int i) const
{
	glm::vec3 v = glm::vec3(_refls[i].hkl.h, _refls[i].hkl.k,
	                        _refls[i].hkl.l);
	
	return v;
}

const glm::vec3 RefList::reflAsFraction(const int i) const
{
	glm::vec3 v = _recip2Frac * reflAsIndex(i);
	return v;
}

const double RefList::resolutionOf(const int i) const
{
	glm::vec3 v = reflAsFraction(i);
	return resolutionOf(v);
}

void RefList::setUnitCell(std::array<double, 6> &cell)
{
	_cell = cell;
	_frac2Real = mat3x3_from_unit_cell(cell[0], cell[1], cell[2],
	                                   cell[3], cell[4], cell[5]);
	_recip2Frac = glm::inverse(_frac2Real);
}
