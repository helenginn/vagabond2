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

#include "MetadataView.h"
#include "ChooseHeader.h"
#include <vagabond/utils/FileReader.h>
#include <vagabond/core/Metadata.h>
#include <vagabond/core/Environment.h>
#include <vagabond/gui/elements/TextButton.h>
#include <vagabond/gui/elements/ImageButton.h>

MetadataView::MetadataView(Scene *prev, Metadata *md) : Scene(prev)
{
	_md = md;

}

MetadataView::~MetadataView()
{
	deleteObjects();

	delete _md;
	_md = nullptr;
}

void MetadataView::setup()
{
	addTitle("Metadata summary - " + _md->source());

	{
		Text *t = new Text("Number of entries");
		t->setLeft(0.2, 0.3);
		addObject(t);
	}
	{
		std::string num = i_to_str(_md->entryCount());
		Text *t = new Text(num);
		t->setLeft(0.8, 0.3);
		addObject(t);
	}

	{
		std::string str = "View " + i_to_str(_md->headerCount()) + " categories";
		Text *t = new Text(str);
		t->setLeft(0.2, 0.4);
		addObject(t);
	}
	{
		ImageButton *t = ImageButton::arrow(-90., this);
		t->setReturnTag("headers");
		t->setCentre(0.8, 0.4);
		addObject(t);
	}

	
	{
		TextButton *t = new TextButton("Add to database", this);
		t->setCentre(0.5, 0.8);
		t->setReturnTag("add");
		addObject(t);
	}
}

void MetadataView::buttonPressed(std::string tag, Button *button)
{
	if (tag == "add")
	{
		Metadata &master = *Environment::metadata();
		master += *_md;
		back();
	}
	else if (tag == "headers")
	{
		ChooseHeader *ch = new ChooseHeader(this, false);
		std::set<std::string> headers = _md->headers();
		ch->setHeaders(headers);
		ch->show();
	}
	
	Scene::buttonPressed(tag, button);
}
