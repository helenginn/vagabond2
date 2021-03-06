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

#ifndef __vagabond__Cluster__cpp__
#define __vagabond__Cluster__cpp__

#include "Cluster.h"
#include <vagabond/utils/maths.h>
#include <vagabond/utils/Canonical.h>

template <class DG>
Cluster<DG>::Cluster(DG &dg) : _dg(dg)
{
	_dg = dg;

}

template <class DG>
const size_t Cluster<DG>::pointCount() const
{
	return _result.rows;
}

struct AxisCC
{
	size_t axis;
	float cc;
	
	const bool operator>(const AxisCC &other) const
	{
		return (cc > other.cc);
	}
};

template <class DG>
int Cluster<DG>::bestAxisFit(std::vector<float> &vals)
{
	std::vector<AxisCC> _pairs;
	for (size_t j = 0; j < _result.rows; j++)
	{
		float x = 0; float y = 0; float xx = 0; 
		float yy = 0; float xy = 0; float s = 0;

		for (size_t i = 0; i < _result.cols; i++)
		{
			float x_ = _result[i][j];
			float y_ = vals[i];

			if (x_ != x_ || y_ != y_)
			{
				continue;
			}
			
			if (!isfinite(x_) || !isfinite(y_))
			{
				continue;
			}

			x += x_;
			xx += x_ * x_;
			y += y_;
			yy += y_ * y_;
			xy += x_ * y_;
			s += 1;
		}

		double top = s * xy - x * y;
		double bottom_left = s * xx - x * x;
		double bottom_right = s * yy - y * y;

		float r = top / sqrt(bottom_left * bottom_right);
		float cc = fabs(r);
		
		if (!isfinite(cc))
		{
			cc = 0;
		}
		
		_pairs.push_back(AxisCC{j, cc});
	}
	
	std::sort(_pairs.begin(), _pairs.end(), std::greater<AxisCC>());
	
	/*
	
	int max = std::min(3, _result.cols);
	PCA::Matrix covariance;
	setupMatrix(&covariance, max, 1);
	
	for (size_t i = 0; i < _result.cols; i++)
	{
		for (size_t j = 0; j < max; j++)
		{
			if (vals[i] != vals[i])
			{
				continue;
			}

			covariance[j][0] += _result[i][j] * vals[i];
		}
	}
	
	printMatrix(&covariance);
	
	CorrelData cd = empty_CD();
	for (size_t i = 0; i < _result.rows; i++)
	{
		float sum = 0;
		for (size_t j = 0; j < max; j++)
		{
			std::cout << _result[i][j] << " ";
			sum += covariance[j][0] * _result[i][j];
		}
		std::cout << std::endl;
		
		add_to_CD(&cd, sum, vals[i]);
	}
	
//	freeMatrix(&covariance);
	double cc = evaluate_CD(cd);
	*/
	
	for (size_t i = 0; i < 3; i++)
	{
		std::cout << _pairs[i].axis << " " << _pairs[i].cc << std::endl;
		_axes[i] = _pairs[i].axis;
	}

	/*
	std::cout << "Best correlation in first " << max << " vectors: "
	<< cc << std::endl;
	*/

	return 0;
}

template <class DG>
glm::vec3 Cluster<DG>::point(int idx)
{
	glm::vec3 v = glm::vec3(0.f);

	for (size_t i = 0; i < 3 && i < _result.cols; i++)
	{
		int axis = _axes[i];
		v[i] = _result[idx][axis];
	}
	
	return v;
}

template <class DG>
void Cluster<DG>::normaliseResults(float scale)
{
	float sum = 0;
	for (size_t i = 0; i < _result.rows * _result.cols; i++)
	{
		sum += _result.vals[i] * _result.vals[i];
	}

	float mag = sqrt(sum);
	
	for (size_t i = 0; i < _result.rows * _result.cols; i++)
	{
		_result.vals[i] /= mag;
		_result.vals[i] *= scale;
	}
}

template <class DG>
std::vector<float> Cluster<DG>::torsionVector(int axis)
{
	std::vector<float> weights;
	
	for (size_t i = 0; i < _result.rows; i++)
	{
		double weight = _result[i][axis] / _scaleFactor;
		weights.push_back(weight);
	}

	std::vector<float> result = _dg.weightedDifferences(weights);
	
	double average = 0;
	for (size_t i = 0; i < result.size(); i++)
	{
		if (result[i] != result[i])
		{
			result[i] = 0;
		}

		average += fabs(result[i]);
	}
	
	average /= (double)result.size();

	for (size_t i = 0; i < result.size(); i++)
	{
		result[i] /= average;
		result[i] *= 4;
	}

	return result;
}

#endif
