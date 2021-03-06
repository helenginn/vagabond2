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

#ifndef __vagabond__Axes__
#define __vagabond__Axes__

#include <vagabond/gui/elements/IndexResponder.h>
#include <vagabond/c4x/Cluster.h>

class Scene;
class Molecule;
class MetadataGroup;

class Axes : public IndexResponder, public ButtonResponder
{
public:
	Axes(Cluster<MetadataGroup> *group, Molecule *m);
	
	void setScene(Scene *scene)
	{
		_scene = scene;
	}

	virtual void interacted(int idx, bool hover, bool left = true);
	virtual void reindex();
	virtual void reorient(int i, Molecule *mol);
	virtual void click(bool left = true);
	virtual bool mouseOver();
	virtual void unmouseOver();

	virtual size_t requestedIndices();

	void buttonPressed(std::string tag, Button *button);
private:
	std::vector<float> getTorsionVector(int axis);
	void prepareAxes();
	void refreshAxes();
	void loadAxisExplorer(int idx);

	Cluster<MetadataGroup> *_cluster = nullptr;
	Molecule *_molecule = nullptr;
	Scene *_scene = nullptr;
	int _lastIdx = -1;
	
	Molecule *_targets[3];

	std::vector<glm::vec3> _dirs;
};

#endif
