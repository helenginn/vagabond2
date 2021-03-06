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

#ifndef __vagabond__ClusterTSNE__
#define __vagabond__ClusterTSNE__

#include "Cluster.h"

template <class DG>
class ClusterTSNE : public Cluster<DG>
{
public:
	ClusterTSNE(DG &dg);
	~ClusterTSNE();

	virtual void cluster();

	virtual size_t displayableDimensions()
	{
		return 3;
	}
private:
	PCA::Matrix probabilityMatrix(int i, float sigma);
	PCA::Matrix probabilityMatrix(PCA::Matrix &sigmas);
	float findSigma(int row, float perp);
	float incrementResult(float &scale, float &learning);
	
	/* returns row=1 col=n vector of sigmas */
	PCA::Matrix findSigmas(float target);
	float startingSigma();
	float perplexity(PCA::Matrix &m);

	PCA::Matrix gradients(PCA::Matrix &prob);
	float qDistanceValue(int i, int j);
	PCA::Matrix qMatrix();

	void prepareResults(float s);
	float averagePQDiff();

	PCA::Matrix _distances;
	PCA::Matrix _ps;
	PCA::Matrix _lastResult;
	PCA::Matrix _tmp;
};

#include "ClusterTSNE.cpp"

#endif
