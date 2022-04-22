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

#ifndef __vagabond__Residue__
#define __vagabond__Residue__

#include <string>
#include "ResidueId.h"

class Residue
{
public:
	Residue(ResidueId num, std::string code, std::string chain);

	int as_num()
	{
		return _id.as_num();
	}

	/** return sequence number and/or insertion character
	 * @return formatted string of number followed by insertion e.g. "65A" */
	std::string id()
	{
		return _id.as_string();
	}

	/** @return three letter code */
	std::string code()
	{
		return _code;
	}

	/** @return chain identifier */
	std::string chain()
	{
		return _chain;
	}
private:
	ResidueId _id;
	std::string _code;
	std::string _chain;
};

#endif
