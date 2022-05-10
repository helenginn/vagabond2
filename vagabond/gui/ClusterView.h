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

#ifndef __vagabond__ClusterView__
#define __vagabond__ClusterView__

#include <vagabond/gui/elements/Renderable.h>
#include <vagabond/c4x/Cluster.h>

class Rule;
class MetadataGroup;

class ClusterView : public Renderable
{
public:
	ClusterView();

	void setCluster(Cluster<MetadataGroup> *cx);
	
	Cluster<MetadataGroup> *cluster()
	{
		return _cx;
	}

	void makePoints();
	void applyRule(const Rule &r);
	void applyVaryColour(const Rule &r);
protected:
	virtual void extraUniforms();
private:
	Cluster<MetadataGroup> *_cx = nullptr;

	float _size = 20;
};

#endif
