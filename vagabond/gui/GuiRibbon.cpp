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

#include "GuiRibbon.h"
#include "GuiAtom.h"
#include <vagabond/core/Atom.h>
#include <vagabond/core/AtomGroup.h>

#define INDICES_PER_BEZIER_DIVISION (12)
#define POINTS_PER_BEZIER (10)
#define DIVISIONS_IN_CIRCLE (5)

GuiRibbon::GuiRibbon(GuiAtom *parent) : GuiRepresentation(parent)
{
	setUsesProjection(true);
	setVertexShaderFile("assets/shaders/with_matrix.vsh");
	setFragmentShaderFile("assets/shaders/lighting.fsh");

}

GuiRibbon::~GuiRibbon()
{

}

float control_points(glm::vec3 *a, glm::vec3 b, glm::vec3 c, glm::vec3 *d)
{
	glm::vec3 ca = c - *a;
	glm::vec3 bd = b - *d;
	ca *= 0.4;
	bd *= 0.4;
	*a = b;
	*d = c;
	float ok = 0;
	 
	if (ca.x == ca.x)
	{
		*a += ca;
	}
	else
	{
		ok = -1;
	}
	
	if (bd.x == bd.x)
	{
		*d += bd;
	}
	else
	{
		ok = +1;
	}
	
	return ok;
}

glm::vec3 bezier(glm::vec3 p1, glm::vec3 p2, glm::vec3 p3, glm::vec3 p4, double t)
{
	double c1 = (1 - t) * (1 - t) * (1 - t);
	double c2 = 3 * t * (1 - t) * (1 - t);
	double c3 = 3 * t * t * (1 - t);
	double c4 = t * t * t;

	p2 *= c1;
	p3 *= c4;
	p1 *= c2;
	p4 *= c3;

	glm::vec3 add = p1 + p2;	
	add += p3 + p4;
	
	return add;
}

Snow::Vertex new_vertex(glm::vec3 p)
{
	Snow::Vertex v;
	memset(&v, 0, sizeof(Snow::Vertex));
	v.pos = p;
	v.color[3] = 1.;
	return v;
}

void GuiRibbon::addCircle(std::vector<Snow::Vertex> &vertices, 
                          glm::vec3 centre, std::vector<glm::vec3> &circle)
{
	for (size_t i = 0; i < circle.size(); i++)
	{
		glm::vec3 pos = centre + circle[i];
		Snow::Vertex v = new_vertex(pos);
		v.normal = circle[i];
		vertices.push_back(v);
	}
}

void GuiRibbon::addCylinderIndices(size_t num)
{
	int begin = - num * 2;
	int half = num;

	for (size_t i = 0; i < num - 1; i++)
	{
		addIndex(begin + 0);
		addIndex(begin + 1);
		addIndex(begin + half);
		addIndex(begin + 1);
		addIndex(begin + half);
		addIndex(begin + half + 1);
		begin++;

	}

	int one = num;
	half -= num * 2 - 1;
	addIndex(begin + 0);
	addIndex(begin + half);
	addIndex(begin + one);
	addIndex(begin + half);
	addIndex(begin + one);
	addIndex(begin + 1);
}

void GuiRibbon::convertToCylinder(std::vector<Snow::Vertex> *vertices)
{
	bool is_vertex_array = false;
	if (vertices == nullptr)
	{
		vertices = &_vertices;
		_indices.clear();
		is_vertex_array = true;
	}

	std::vector<Snow::Vertex> orig = *vertices;
	vertices->clear();
	
	const int divisions = DIVISIONS_IN_CIRCLE;

	glm::vec3 prev_axis = glm::vec3(0, 0, 0);
	glm::vec3 xAxis = glm::vec3(1, 0, 0);

	std::vector<glm::vec3> pucker;

	for (size_t i = 0; i < divisions; i++)
	{
		pucker.push_back(glm::vec3(0.f));
	}

	for (int i = 0; i < (int)orig.size() - 1; i++)
	{
		glm::vec3 v1 = orig[i].pos;
		glm::vec3 v2 = orig[i + 1].pos;
		
		/* get the axis of motion */
		glm::vec3 axis = glm::normalize(v2 - v1);
		glm::vec3 average_with_prev = glm::normalize(axis + prev_axis);

		/* generate the appropriate circle params */
		std::vector<glm::vec3> circle;
		float angle = (2 * M_PI) / (double)divisions;

		glm::mat4x4 rot = glm::rotate(glm::mat4(1.f), angle, average_with_prev);
		glm::vec3 cross = glm::normalize(glm::cross(axis, xAxis));
		cross *= 0.3;

		prev_axis = axis;

		for (size_t i = 0; i < divisions; i++)
		{
			circle.push_back(cross);
			cross = glm::vec3(rot * glm::vec4(cross, 1.));
		}

		if (is_vertex_array && i == 0)
		{
			addCircle(*vertices, v1, pucker);
		}
		else if (i == 0)
		{
			addCircle(*vertices, v1, circle);
		}

		if (orig[i].extra[0] < -0.5)
		{
			addCircle(*vertices, v2, pucker);
		}
		else if (orig[i].extra[0] > 0.5)
		{
			addCircle(*vertices, v1, pucker);
		}
		else
		{
			addCircle(*vertices, v2, circle);
		}

		if (is_vertex_array)
		{
			addCylinderIndices(circle.size());
		}
	}
	
	if (is_vertex_array)
	{
		_renderType = GL_TRIANGLES;
		setColour(0.4, 0.4, 0.4);
	}
}

std::vector<Snow::Vertex> GuiRibbon::makeBezier(int index)
{
	int is[] = {index - 1, index + 0, index + 1, index + 2};
	
	if (is[0] < 0) is[0] = 0;
	if (is[2] >= _cAlphas.size()) is[2] = _cAlphas.size() - 1;
	if (is[3] >= _cAlphas.size()) is[3] = _cAlphas.size() - 1;

	glm::vec3 p1 = _idxPos[is[0]];
	glm::vec3 p2 = _idxPos[is[1]];
	glm::vec3 p3 = _idxPos[is[2]];
	glm::vec3 p4 = _idxPos[is[3]];
	
	return makeBezier(p1, p2, p3, p4, true);
}

std::vector<Snow::Vertex> GuiRibbon::makeBezier(glm::vec3 p1, glm::vec3 p2,
                                                glm::vec3 p3, glm::vec3 p4,
                                                bool overwrite)
{
	std::vector<Snow::Vertex> vs;

	float ok = control_points(&p1, p2, p3, &p4);

	float limit = 0.95;
	for (double t = 0; t <= limit; t += 1.0 / (float)POINTS_PER_BEZIER)
	{
		glm::vec3 p = bezier(p1, p2, p3, p4, t);
		Snow::Vertex v = new_vertex(p);

		if (ok < -0.5 && t < 0.05)
		{
			v.extra[0] = ok;
		}
		else if (ok > 0.5 && t > 0.75)
		{
			v.extra[0] = ok;
		}
		
		vs.push_back(v);
	}

	return vs;
}

void GuiRibbon::convertToBezier()
{
	std::vector<Snow::Vertex> vs = _vertices;
	_vertices.clear();

	for (int i = 1; i < (int)vs.size() - 2; i++)
	{
		glm::vec3 p1 = vs[i - 1].pos;
		glm::vec3 p2 = vs[i + 0].pos;
		glm::vec3 p3 = vs[i + 1].pos;
		glm::vec3 p4 = vs[i + 2].pos;

		std::vector<Snow::Vertex> next_set = makeBezier(p1, p2, p3, p4);
		_vertices.reserve(_vertices.size() + next_set.size());
		_vertices.insert(_vertices.end(), next_set.begin(), next_set.end());
	}
}

bool GuiRibbon::previousPositionValid()
{
	bool lastOK = (_vertices.size() > 0 && 
	               _vertices.back().pos[0] == _vertices.back().pos[0]);

	return lastOK;
}

void GuiRibbon::insertAtom(Atom *a)
{
	if (a != nullptr)
	{
		glm::vec3 pos = a->initialPosition();
		addVertex(pos).color[3] = 1;
		int idx = _cAlphas.size() - 1;
		_cAlphas.push_back(a);
		_atomPos[a] = pos;
		_idxPos[idx] = pos;
		_atomIndex[a] = idx;
	}
	else
	{
		glm::vec3 nan = glm::vec3(NAN, NAN, NAN);
		addVertex(nan).color[3] = 1.;
		int idx = _cAlphas.size() - 1;
		_cAlphas.push_back(nullptr);
		_idxPos[idx] = nan;
	}
}

void GuiRibbon::watchAtom(Atom *a)
{
	if (_vertices.size() == 0)
	{
		insertAtom(nullptr);
	}
	
	// HARDCODE
	if (a->atomName() != "CA")
	{
		return;
	}
	
	Atom *prev = _cAlphas.back();
	float l = FLT_MAX;

	if (prev)
	{
		glm::vec3 ppos = prev->initialPosition();
		glm::vec3 npos = a->initialPosition();
		
		l = glm::length(ppos - npos);
	}
	
	if (!prev)
	{
		insertAtom(a);
	}
	else if (a->residueId().num == prev->residueId().num + 1)
	{
		insertAtom(a);
	}
	else if (l < 3.9)
	{
		insertAtom(a);
	}
	else 
	{
		insertAtom(nullptr);
	}
	
}

void GuiRibbon::convert()
{
	insertAtom(nullptr);
	convertToBezier();
	convertToCylinder();
}

void GuiRibbon::transplantCylinder(std::vector<Snow::Vertex> &cylinder, int start)
{
	int j = 0;

	if (_vertices.size() <= start + cylinder.size())
	{
		_vertices.resize(start + cylinder.size());
	}

	std::copy(cylinder.begin(), cylinder.end(), _vertices.begin() + start);

//	for (size_t i = start; i < start + cylinder.size(); i++)
	{
		/*
		std::cout << glm::to_string(_vertices[i].pos) << " ";
		std::cout << glm::to_string(cylinder[j].pos) << "\t";
		std::cout << glm::length(_vertices[i].pos - cylinder[j].pos) << std::endl;
		*/

//		_vertices[i].pos = cylinder[j].pos;
//		_vertices[i].normal = cylinder[j].normal;
//		j++;
	}
}

size_t GuiRibbon::indicesPerAtom()
{
	return POINTS_PER_BEZIER * DIVISIONS_IN_CIRCLE * INDICES_PER_BEZIER_DIVISION;
}

size_t GuiRibbon::verticesPerAtom()
{
	return POINTS_PER_BEZIER * DIVISIONS_IN_CIRCLE;
}

void GuiRibbon::prepareAtomSpace(AtomGroup *ag)
{
	int count = 0;
	for (size_t i = 0; i < ag->size(); i++)
	{
		// HARDCODE
		if ((*ag)[i]->atomName() == "CA")
		{
			count++;
		}
	}

	_vertices.reserve(count * verticesPerAtom());
	_indices.reserve(count * indicesPerAtom());
}

void GuiRibbon::updateSinglePosition(Atom *a, glm::vec3 &p)
{
	if (_atomIndex.count(a) == 0)
	{
		return;
	}

	int fix = _atomIndex[a];
	_idxPos[fix] = p;
	_atomPos[a] = p;
	
	for (int i = fix - 2; i < fix + 1; i++)
	{
		if (i < 0 || i >= (int)_cAlphas.size() - 1)
		{
			continue;
		}

		std::vector<Snow::Vertex> bez = makeBezier(i);

		convertToCylinder(&bez);

		int start = i * verticesPerAtom();
		lockMutex();
		transplantCylinder(bez, start);
		unlockMutex();
	}
	
}

void GuiRibbon::updateMultiPositions(Atom *a, Atom::WithPos &wp)
{
	updateSinglePosition(a, wp.ave);
}
