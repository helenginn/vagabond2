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

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#else
#include <curl/curl.h>
#include <vagabond/utils/extra_curl_utils.h>
#endif

#include <iostream>
#include <vagabond/utils/FileReader.h>

#include "EntityFromSequence.h"
#include "AddEntity.h"

#include <vagabond/gui/elements/TextEntry.h>
#include <vagabond/gui/elements/TextButton.h>
#include <vagabond/gui/elements/AskForText.h>
#include <vagabond/gui/elements/BadChoice.h>

EntityFromSequence::EntityFromSequence(Scene *prev) : Scene(prev)
{

}

EntityFromSequence::~EntityFromSequence()
{
	pthread_join(_thread, NULL);

	deleteObjects();
}

void EntityFromSequence::setup()
{
	addTitle("Entity from sequence");
	
	{
		TextButton *tb = new TextButton("From UniProt ID", this);
		tb->setReturnTag("uniprot");
		tb->setLeft(0.2, 0.3);
		addObject(tb);
	}
}

void EntityFromSequence::handleError()
{
	BadChoice *bc =  new BadChoice(this, "Connection error");
	this->setModal(bc);
}

void EntityFromSequence::processResult(std::string seq)
{
	if (seq.rfind("Sequence", 0) == std::string::npos)
	{
		std::cout << "here" << std::endl;
		BadChoice *bc = new BadChoice(this, "Sequence could not be obtained");
		setModal(bc);
		return;
	}
	else
	{
		size_t it = seq.find("\n");
		it++;
		std::string next = &seq[it];
		
		while (next.back() == '\n')
		{
			next.pop_back();
		}
		
		if (next.length() == 0)
		{
			return;
		}

		AddEntity *addEntity = new AddEntity(_previous, next);
		back();
		addEntity->show();
	}
}

std::string EntityFromSequence::toURL(std::string query)
{
	std::string url = "https://www.uniprot.org/uniprot/?query=id:";
	url += query;
	url += "&columns=sequence&format=tab";

	return url;
}

void EntityFromSequence::buttonPressed(std::string tag, Button *button)
{
	if (tag == "uniprot")
	{
		AskForText *aft = new AskForText(this, "Enter UniProt ID:",
		                                 "uniprot_id", this, TextEntry::Id);
		setModal(aft);
		return;
	}

	if (tag == "uniprot_id")
	{
		TextEntry *te = static_cast<TextEntry *>(button);
		std::string text = te->scratch();
		load(text);

	}

	Scene::buttonPressed(tag, button);
}

void EntityFromSequence::render()
{
	Scene::render();

	if (_process)
	{
		processResult(_result);
		_result = "";
		_process = false;
	}
}
