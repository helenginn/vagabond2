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

#include "Superpose.h"
#include "svd/PCA.h"
#include <iostream>

using namespace PCA;

Superpose::Superpose()
{
	_transformation = glm::mat4(0.f);
}

void Superpose::getAveragePositions(glm::vec3 &pm, glm::vec3 &qm)
{
	float count = 0;
	pm = glm::vec3(0.);
	qm = glm::vec3(0.);

	for (size_t i = 0; i < _pairs.size(); i++)
	{
		pm += _pairs[i].p;
		qm += _pairs[i].q;
		count++;
	}

	pm /= count;
	qm /= count;
}

void Superpose::subtractPositions(const glm::vec3 &pm, const glm::vec3 &qm)
{
	for (size_t i = 0; i < _pairs.size(); i++)
	{
		_pairs[i].p -= pm;
		_pairs[i].q -= qm;
	}
}

void Superpose::populateSVD(SVD &svd)
{
	for (size_t i = 0; i < _pairs.size(); i++)
	{
		std::cout << glm::to_string(_pairs[i].p) << " vs " 
		<< glm::to_string(_pairs[i].q) << std::endl;

		for (size_t k = 0; k < 3; k++)
		{
			for (size_t j = 0; j < 3; j++)
			{
				float add = _pairs[i].p[k] * _pairs[i].q[j];

				svd.u.ptrs[j][k] += add;
			}
		}
	}
	
	printMatrix(&svd.u);
}

glm::mat3x3 Superpose::getRotation(SVD &svd)
{
	glm::mat3x3 rot = glm::mat3x3(0.f);

	for (size_t j = 0; j < 3; j++)
	{
		for (size_t i = 0; i < 3; i++)
		{
			for (size_t n = 0; n < 3; n++)
			{
				rot[j][i] += svd.v.ptrs[i][n] * svd.u.ptrs[j][n];
			}
		}
	}

	return rot;
}

void Superpose::addPositionPair(glm::vec3 &p, glm::vec3 &q)
{
	PosPair pair{};
	pair.p = p;
	pair.q = q;
	_pairs.push_back(pair);
}

void Superpose::addPositionPairs(std::vector<PosPair> &pairs)
{
	_pairs.reserve(_pairs.size() + pairs.size());
	_pairs.insert(_pairs.end(), pairs.begin(), pairs.end());
}

void Superpose::createTransformation(glm::vec3 &subtract, glm::mat3x3 &rot, 
                                     glm::vec3 &add)
{
	subtract *= -1;
	glm::mat4x4 submat = glm::translate(subtract);
	glm::mat4x4 rotmat = glm::mat4x4(rot);
	glm::mat4x4 addmat = glm::translate(add);
	
	std::cout << "Submat: " << std::endl;
	std::cout << glm::to_string(submat) << std::endl;
	std::cout << std::endl;
	
	std::cout << "Rotmat: " << std::endl;
	std::cout << glm::to_string(rotmat) << std::endl;
	std::cout << std::endl;
	
	std::cout << "Addmat: " << std::endl;
	std::cout << glm::to_string(addmat) << std::endl;
	std::cout << std::endl;

	_transformation = addmat * rotmat * submat;
	_rotation = glm::mat3x3(_transformation);
	_translation = glm::vec3(_transformation[3]);
}

void Superpose::superpose()
{
	glm::vec3 pm = glm::vec3(0.);
	glm::vec3 qm = glm::vec3(0.);
	getAveragePositions(pm, qm);
	subtractPositions(pm, qm);
	
	SVD svd;
	setupSVD(&svd, 3, 3);
	populateSVD(svd);
	runSVD(&svd);

	glm::mat3x3 rot = getRotation(svd);

	glm::vec3 &subtract = qm;
	glm::vec3 &add = pm;
	
	createTransformation(subtract, rot, add);
}
