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

#include "Display.h"
#include "FileView.h"
#include "FileLine.h"

#ifdef __EMSCRIPTEN__
#include <vagabond/gui/elements/upload.h>
#endif

#include "MetadataView.h"

#include <vagabond/gui/elements/Text.h>
#include <vagabond/gui/elements/TextButton.h>

#include <vagabond/core/File.h>
#include <vagabond/core/FileManager.h>
#include <vagabond/core/AtomContent.h>
#include <vagabond/core/Environment.h>

FileView::FileView(Scene *prev) : ListView(prev)
{
	_manager = Environment::fileManager();
	_manager->setFilterType(File::Nothing);
	_manager->HasResponder<Responder<FileManager>>::setResponder(this);
}

FileView::FileView(FileViewResponder *prev, bool choose) : ListView(prev)
{
	_responder = prev;
	_manager = Environment::fileManager();
	_manager->setFilterType(File::Nothing);
	_manager->HasResponder<Responder<FileManager>>::setResponder(this);
}

FileView::~FileView()
{
}

void FileView::filterForTypes(File::Type type)
{
	_manager->setFilterType(type);
}

void FileView::setup()
{
	addTitle("Vagabond file system");

#ifdef __EMSCRIPTEN__
	{
		TextButton *text = new TextButton("Add", this);
		text->setCentre(0.9, 0.1);
		text->setReturnTag("add");
		addObject(text);
	}
#endif
	
	ListView::setup();
}

void FileView::handleFileWithoutChoice(std::string filename)
{
	File *file = File::loadUnknown(filename);
	File::Type type = file->cursoryLook();

	if (type & File::CompAtoms || type & File::MacroAtoms)
	{
		if (file->atomCount() > 0)
		{
			Display *display = new Display(this);
			display->loadAtoms(file->atoms());
			display->show();
		}
	}
	else if (type & File::Reflections)
	{
		Display *display = new Display(this);
		display->loadDiffraction(file->diffractionData());
		display->show();
	}
	else if (type & File::Meta)
	{
		MetadataView *view = new MetadataView(this, file->metadata());
		view->show();
	}

	delete file;
}

void FileView::returnToResponder(std::string filename)
{
	_responder->fileChosen(filename);
	Scene::back();
}

void FileView::buttonPressed(std::string tag, Button *button)
{
	Scene::buttonPressed(tag, button);

	std::string filename = Button::tagEnd(tag, "file_");

	if (filename.length())
	{
		if (_responder == nullptr)
		{
			handleFileWithoutChoice(filename);
		}
		else
		{
			returnToResponder(filename);
		}
	}

#ifdef __EMSCRIPTEN__
	if (tag == "add")
	{
		upload_file();
	}
#endif
	
	ListView::buttonPressed(tag, button);
}

Renderable *FileView::getLine(int i)
{
	std::string filename = _manager->filtered(i);
	std::string onlyname = getFilename(filename);

	FileLine *line = new FileLine(this, filename, onlyname);
	return line;
}

size_t FileView::lineCount()
{
	int fileCount = _manager->filteredCount();
	return fileCount;
}

void FileView::respond()
{
	std::cout << "Triggered response" << std::endl;
	refreshNextRender();
}
