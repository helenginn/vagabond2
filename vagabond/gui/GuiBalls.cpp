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

#include "GuiBalls.h"
#include "GuiBond.h"

#include <vagabond/gui/elements/Icosahedron.h>
#include <vagabond/gui/elements/SnowGL.h>

#include <vagabond/core/matrix_functions.h>
#include <vagabond/core/Atom.h>
#include <vagabond/core/AtomGroup.h>

#include <iostream>
#include <thread>

GuiBalls::GuiBalls(GuiAtom *parent) : GuiRepresentation(parent)
{
	setUsesProjection(true);
	setVertexShaderFile("assets/shaders/with_matrix.vsh");
	setFragmentShaderFile("assets/shaders/lighting.fsh");
	
	_template = new Icosahedron();
	_template->triangulate();
	_template->setColour(0.5, 0.5, 0.5);
	_template->resize(0.3);
	_template->setAlpha(1.);
	
	_bonds = new GuiBond();
	addObject(_bonds);
//	setDisabled(true);
}

GuiBalls::~GuiBalls()
{
	delete _bonds;
	_bonds = nullptr;
	delete _template;
	_template = nullptr;
	
}

void GuiBalls::colourByElement(std::string ele)
{
	glm::vec4 colour = glm::vec4(0.5, 0.5, 0.5, 1.);
	if (ele == "O")
	{
		colour = glm::vec4(1.0, 0.2, 0.2, 1.);
	}
	if (ele == "S")
	{
		colour = glm::vec4(1.0, 1.0, 0.2, 1.);
	}
	if (ele == "P")
	{
		colour = glm::vec4(1.0, 0.2, 1.0, 1.);
	}
	if (ele == "H")
	{
		colour = glm::vec4(0.8, 0.8, 0.8, 1.);
	}
	if (ele == "N")
	{
		colour = glm::vec4(0.2, 0.2, 1.0, 1.);
	}

	_template->setColour(colour.x, colour.y, colour.z);
}

void GuiBalls::setPosition(glm::vec3 position)
{
	if (!is_glm_vec_sane(position))
	{
		throw std::runtime_error("position contains nan or vec values");
	}

	_template->setPosition(position);
}

size_t GuiBalls::indicesPerAtom()
{
	return _template->indexCount();
}

size_t GuiBalls::verticesPerAtom()
{
	return _template->vertexCount();
}

void GuiBalls::updateMultiPositions(Atom *a, Atom::WithPos &wp)
{
	_bonds->updateAtoms(a, wp);
}

void GuiBalls::updateSinglePosition(Atom *a, glm::vec3 &p)
{
	int idx = _atomIndex[a];
	glm::vec3 last = _atomPos[a];

	glm::vec3 diff = p - last;

	if (!is_glm_vec_sane(diff))
	{
		throw std::runtime_error("position contains nan or vec values");
	}

	int end = idx + verticesPerAtom(); 
	lockMutex();
	for (size_t j = idx; j < end; j++)
	{
		_vertices[j].pos += diff;
	}
	unlockMutex();

	_atomPos[a] = p;
	_bonds->updateAtom(a, p);
}

void GuiBalls::watchAtom(Atom *a)
{
	long index = _vertices.size();

	colourByElement(a->elementSymbol());
	setPosition(a->initialPosition());

	appendObject(_template);

	_atomPos[a] = a->initialPosition();
	_atomIndex[a] = index;
}

void GuiBalls::watchBonds(AtomGroup *ag)
{
	_bonds->watchBonds(ag);
}

void GuiBalls::prepareAtomSpace(AtomGroup *ag)
{
	_vertices.reserve(verticesPerAtom() * ag->size());
	_indices.reserve(indicesPerAtom() * ag->size());
}
