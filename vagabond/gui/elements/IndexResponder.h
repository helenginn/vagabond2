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

#ifndef __vagabond__IndexResponder__
#define __vagabond__IndexResponder__

#include <vagabond/gui/elements/Renderable.h>
#include "IndexResponseView.h"

class IndexResponder : public Renderable
{
public:
	virtual ~IndexResponder() {};
	
	void setIndexResponseView(IndexResponseView *irv)
	{
		_view = irv;
	}

	virtual void interacted(int idx, bool hover, bool left) = 0;
	virtual void reindex() = 0;

	virtual size_t requestedIndices() = 0;

	virtual size_t indexOffset()
	{
		if (_view == nullptr)
		{
			return 0;
		}

		return _view->indexOffset(this);
	}
protected:
	IndexResponseView *_view = nullptr;

};

#endif
