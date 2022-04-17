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

#include <vagabond/utils/FileReader.h>
#include "FileManager.h"
#include "FileView.h"

FileManager::FileManager()
{
	_view = nullptr;
#ifdef __EMSCRIPTEN__
	_list.push_back("assets/geometry/ATP.cif");
	_list.push_back("assets/examples/2ybh.cif");
	_list.push_back("assets/examples/4cvd.cif");
	_list.push_back("assets/examples/4cvd.pdb");
#endif
}

bool FileManager::acceptFile(std::string filename, bool force)
{
	bool added = false;

	if (file_exists(filename) || force)
	{
		_list.push_back(filename);
		added = true;
	}
	
	if (_view != nullptr)
	{
		_view->refreshFiles();
	}
	
	return added;
}

