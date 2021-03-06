// vagabond
// Copyright (C) 2022 Helen Ginn
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

#ifndef __vagabond__DegreeDataGroup__
#define __vagabond__DegreeDataGroup__

#include "DataGroup.h"

/** \class DegreeDataGroup
 * specialised class of DataGroup that takes degree values and ensures all
 * values match previous entries as closely as possible by adding or subtracting
 * multiple of 360 degrees */

template <class Header>
class DegreeDataGroup : public DataGroup<float, Header>
{
public:
	using Array = typename DataGroup<float, Header>::Array;

	DegreeDataGroup(int num) : DataGroup<float, Header>(num) {}

	virtual void addArray(std::string name, Array next);
	virtual float difference(int m, int n, int j);
protected:
	using DataGroup<float, Header>::_length;
	using DataGroup<float, Header>::_vectors;
	using DataGroup<float, Header>::_diffs;
	using DataGroup<float, Header>::_stdevs;

private:
	void matchDegrees(Array &next);

};

#include "DegreeDataGroup.cpp"

#endif
