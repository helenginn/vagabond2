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

#ifndef __vagabond__BondTorsion__
#define __vagabond__BondTorsion__

class AtomGroup;
class Atom;

#include "Bondstraint.h"
#include <stdexcept>

class BondTorsion : public Bondstraint
{
public:
	/** ownership is taken over by the AtomGroup after creation */
	BondTorsion(AtomGroup *owner, Atom *a, Atom *b, Atom *c, 
	            Atom *d, double angle);

	virtual ~BondTorsion() {};

	double angle() const
	{
		return _angle;
	}
	
	Atom *atom(int i)
	{
		if (i == 0) return _a;
		if (i == 1) return _b;
		if (i == 2) return _c;
		if (i == 3) return _d;
		throw std::runtime_error("asked for silly atom number from bond angle");
	}
	
	bool atomIsTerminal(Atom *a)
	{
		return (_d == a || _a == a) && (_b != a && _c != a);
	}
	
	bool atomIsCentral(Atom *a)
	{
		return _b == a || _c == a;
	}
	
	bool operator==(const BondTorsion &other) const;
	bool operator!=(const BondTorsion &other) const
	{
		return !(*this == other);
	}

	bool isConstrained();
	
	void setConstrained(bool constrained)
	{
		_constrained = constrained;
	}
	
	virtual std::string desc();
private:
	AtomGroup *_owner;
	Atom *_a;
	Atom *_b;
	Atom *_c;
	Atom *_d;
	double _angle;
	bool _constrained;
};

#endif

