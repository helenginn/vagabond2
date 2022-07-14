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

#ifndef __practical__MainMenu__
#define __practical__MainMenu__

#include <vagabond/gui/elements/Scene.h>
#include <vagabond/core/FileManager.h>

class MainMenu : public Scene, public Responder<FileManager>
{
public:
	MainMenu();
	
	virtual ~MainMenu();

	virtual void setup();
	virtual void doThings();
	virtual void buttonPressed(std::string tag, Button *button = nullptr);
	virtual void respond();
private:
	void checkForJson();

	bool _checkFiles = false;
};

#endif
