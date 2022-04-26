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

#include "ModelManager.h"
#include <iostream>

ModelManager::ModelManager() : Manager()
{

}

void ModelManager::insertIfUnique(const Model &m)
{
	for (Model &other : _objects)
	{
		if (other.name() == m.name())
		{
			throw std::runtime_error("Model with same name exists");
		}
	}
	
	if (m.name().length() == 0)
	{
		throw std::runtime_error("Model has no name");
	}
	
	if (m.filename().length() == 0)
	{
		throw std::runtime_error("Model has no initial file");
	}
	
	_objects.push_back(m);
	housekeeping();

	if (_responder)
	{
		_responder->objectsChanged();
	}
}

void ModelManager::update(const Model &m)
{
	Model *old = model(m.name());
	*old = m;
}

void ModelManager::housekeeping()
{
	for (Model &m : _objects)
	{
		_name2Model[m.name()] = &m;
	}

}
