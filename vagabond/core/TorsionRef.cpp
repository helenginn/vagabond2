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

#include "TorsionRef.h"
#include <sstream>

TorsionRef::TorsionRef()
{

}

TorsionRef::TorsionRef(BondTorsion *tmp)
{
	if (tmp == nullptr)
	{
		return;
	}

	_desc = tmp->desc();
	_main = tmp->coversMainChain();
	_torsion = tmp;
	organiseDescriptions();
}

void TorsionRef::organiseDescriptions()
{
	std::ostringstream str;
	size_t pos = 0;

	std::string copy = _desc;

	while (true)
	{
		pos = copy.rfind('-');
		
		if (pos == std::string::npos)
		{
			str << &copy[0];
			break;
		}

		copy[pos] = '\0';
		pos++;
		str << &copy[pos] << "-";
	}

	std::string rev = str.str();
	_reverse_desc = rev;
	if (_reverse_desc > _desc)
	{
		_reverse_desc = _desc;
		_desc = rev;
	}
}

void TorsionRef::housekeeping()
{
	organiseDescriptions();
}
