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

#include "ElementSegment.h"
#include "AtomGroup.h"

ElementSegment::ElementSegment(AtomGroup *grp)
: FFTCubicGrid<VoxelElement>()
{
	int nx = 0;
	int ny = 0;
	int nz = 0;
	
	// work out dimensions from atom group

	this->Grid::setDimensions(nx, ny, nz);
}

void ElementSegment::populatePlan(FFT<VoxelElement>::PlanDims &dims)
{
	int ns[3] = {dims.nx, dims.ny, dims.nz};
	const int rank = 3;
	const int base_stride = sizeof(VoxelElement) / sizeof(float);

	fftw_iodim rankdims[3];
	rankdims[2] = {ns[2], base_stride, base_stride};
	rankdims[1] = {ns[1], ns[2] * rankdims[2].is, ns[2] * rankdims[2].os};
	rankdims[0] = {ns[0], ns[1] * rankdims[1].is, ns[1] * rankdims[1].os};
	
	unsigned flags = FFTW_MEASURE;

	dims.forward =  fftwf_plan_guru_split_dft(rank, rankdims, 0, nullptr,
	                                          &_data[0].value[0], 
	                                          &_data[0].value[1],
	                                          &_data[0].value[0], 
	                                          &_data[0].value[1], flags);

	dims.backward = fftwf_plan_guru_split_dft(rank, rankdims, 0, nullptr,
	                                          &_data[0].value[1], 
	                                          &_data[0].value[0],
	                                          &_data[0].value[1], 
	                                          &_data[0].value[0], flags);

}

void ElementSegment::addDensity(glm::vec3 real, float density)
{
	real -= origin();
	real2Voxel(real);

	collapse(real);

	float &sx = real.x;
	float &sy = real.y;
	float &sz = real.z;

	float ss[3] = {sx, sy, sz};
	float ls[3], rs[3];
	
	for (int i = 0; i < 3; i++)
	{
		ls[i] = (int)floor(ss[i]);
		rs[i] = ss[i] - ls[i];
	}
	
	sx = ls[0] + rs[0];
	sy = ls[1] + rs[1];
	sz = ls[2] + rs[2];
	
	float xProps[2];
	float yProps[2];
	float zProps[2];

	xProps[0] = std::max(1 + ls[0] - sx, 0.f);
	xProps[1] = std::max(sx - ls[0], 0.f);

	yProps[0] = std::max(1 + ls[1] - sy, 0.f);
	yProps[1] = std::max(sy - ls[1], 0.f);

	zProps[0] = std::max(1 + ls[2] - sz, 0.f);
	zProps[1] = std::max(sz - ls[2], 0.f);
	
	for (int r = 0; r < 2; r++)
	{
		for (int q = 0; q < 2; q++)
		{
			for (int p = 0; p < 2; p++)
			{
				int sx1 = ls[0] + p;
				int sy1 = ls[1] + q;
				int sz1 = ls[2] + r;

				long idx = index(sx1, sy1, sz1);
				float prop = xProps[p] * yProps[q] * zProps[r];
				_data[idx].value[0] += prop * density;
			}	
		}
	}
}

void ElementSegment::multiply(float scale)
{
	for (size_t i = 0; i < nn(); i++)
	{
		_data[i].value[0] *= scale;
		_data[i].value[1] *= scale;
	}
}

float ElementSegment::sum()
{
	float val = 0;
	for (size_t i = 0; i < nn(); i++)
	{
		val += _data[i].value[0];
		val += _data[i].value[1];
	}
	
	return val;
}