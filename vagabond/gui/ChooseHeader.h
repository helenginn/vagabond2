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

#ifndef __vagabond__ChooseHeader__
#define __vagabond__ChooseHeader__

#include <vagabond/core/Responder.h>
#include <vagabond/gui/elements/ListView.h>
#include <set>

class Entity;

class ChooseHeader : public ListView,
public HasResponder<Responder<ChooseHeader>>
{
public:
	ChooseHeader(Scene *prev, bool choose = true);
	~ChooseHeader();
	
	void setHeaders(std::set<std::string> &headers)
	{
		_headers.clear();
		for (const std::string &h : headers)
		{
			_headers.push_back(h);
		}
	}
	
	void setCanDelete(bool del)
	{
		_canDelete = del;
	}
	
	void setTitle(std::string title)
	{
		_title = title;
	}

	void setEntity(std::string name);
	virtual void setup();

	virtual size_t lineCount();
	virtual Renderable *getLine(int i);

	virtual void buttonPressed(std::string tag, Button *button = nullptr);
private:
	Entity *_entity;
	std::vector<std::string> _headers;
	std::vector<std::string> _assigned;
	std::string _title;
	bool _choose = false;
	bool _canDelete = false;


};

#endif
