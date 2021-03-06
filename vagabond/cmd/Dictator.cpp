// breathalyser
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

#include "../utils/FileReader.h"
#include "Dictator.h"
#include "CmdWorker.h"
#include <vagabond/core/FileManager.h>
#include <vagabond/core/Environment.h>
#include <vagabond/core/Metadata.h>
#include <iostream>
#include <unistd.h>

std::map<std::string, std::string> Dictator::_properties;
std::map<std::string, std::string> Dictator::_commands;

void Dictator::makeCommands()
{
	_commands["load"] = "Comma- or space-separated list of files to load";
	_commands["add"] = "Load metadata CSV file with 'model' or 'filename' column";
	_commands["environment"] = ("Link to json file (usually rope.json) to"\
	                            "restore RoPE environment");
	_commands["automodel"] = ("Auto-model atom coordinate files according "\
	                          "to existing entities matching at least 80% "
	                          "sequence and default names");
	_commands["rescan"] = ("Rescan existing models for existing entities"
	                          " matching at least 80% sequence and default names");
	_commands["get-files-native-app"] = ("load files from three directories up in "
	                                     "the Mac Rope app file");
}

void splitCommand(std::string command, std::string *first, std::string *last)
{
	size_t equal = command.find('=');
	if (equal == std::string::npos)
	{
		*first = command;
		*last = "";
		return;
	}

	*first = command.substr(0, equal);
	
	if (command.length() <= equal + 1)
	{
		*last = "";
		return;
	}

	*last = command.substr(equal + 1, std::string::npos);
}

Dictator::Dictator()
{
	_worker = NULL;
	_currentJob = -1;
	makeCommands();
}

void Dictator::setup()
{
	if (_worker != nullptr)
	{
		delete _worker;
	}

	_worker = new CmdWorker(this);
}

void Dictator::getFilesNativeApp()
{
	std::vector<std::string> globs = glob_pattern("*");

	FileManager *fm = Environment::fileManager();
	for (std::string &g : globs)
	{
		loadFiles(g);
	}
}

void escape_filename(std::string &file)
{
	for (size_t i = 0; i < file.size(); i++)
	{
		if (file[i] == ' ')
		{
			file.insert(i, "\\");
			i++;
		}
	}
}

void Dictator::loadFiles(std::string &last)
{
	FileManager *fm = Environment::fileManager();
	std::vector<std::string> files = split(last, ',');

	for (size_t i = 0; i < files.size(); i++)
	{
		std::string file = files[i];
		escape_filename(file);

		if (!fm->acceptFile(file))
		{
			std::cout << "File not found/accessible: " << 
			file << std::endl;
		}
		else
		{
			std::cout << "Found file: " << file << std::endl;
		}
	}
}

void Dictator::processRequest(std::string &first, std::string &last)
{
	if (first == "load" || first == "")
	{
		loadFiles(last);
	}

	if (first == "environment")
	{
		Environment::env().load(last);
	}

	if (first == "automodel")
	{
		Environment::env().autoModel();
	}

	if (first == "get-files-native-app")
	{
		getFilesNativeApp();
	}

	if (first == "rescan")
	{
		Environment::rescanModels();
	}

	if (first == "add")
	{
		File *file = File::loadUnknown(last);
		
		File::Type type = File::Nothing;
		
		if (file)
		{
			type = file->cursoryLook();
		}

		if (type & File::Meta)
		{
			*Environment::metadata() += *file->metadata();
		}
		else
		{
			std::cout << "File " << last << " is not readable or is "
			"not metadata." << std::endl;
		}
	}
}

bool Dictator::checkForFile(std::string &first, std::string &last)
{
	if (last != "")
	{
		return false;
	}
	
	if (file_exists(first))
	{
		loadFiles(first);
		return true;
	}
	
	return false;
}

bool Dictator::sanityCheck(std::string &first, std::string &last)
{
	if (_commands.count(first) == 0)
	{
		std::cout << "Command not recognised: " << first << std::endl;
		return false;
	}

	return true;
}

void Dictator::processNextArg(std::string arg)
{
	std::cout << std::endl;
	std::cout << "Processing next arg: " << "\"" << arg << 
	"\"" << std::endl;
	std::string first, last;
	splitCommand(arg, &first, &last);
	std::cout << first << " " << last << std::endl;
	
	if (checkForFile(first, last))
	{
		return;
	}
	
	if (sanityCheck(first, last))
	{
		processRequest(first, last);
	}
}

bool Dictator::nextJob()
{
	_currentJob++;

	if ((size_t)_currentJob >= _args.size())
	{
		_currentJob--;
		return false;
	}

	processNextArg(_args[_currentJob]);
	return true;
}

void Dictator::start()
{
	if (_worker == nullptr)
	{
		setup();
	}

	_currentJob = -1;
	_thread = new std::thread(&CmdWorker::start, _worker);
}

