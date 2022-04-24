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

#ifndef __vagabond__Model__
#define __vagabond__Model__

#include <string>
#include <iostream>

#include <json/json.hpp>
using nlohmann::json;

class Model
{
public:
	Model();
	
	void setFilename(std::string file);
	
	const std::string entityForChain(std::string id) const;
	
	void setName(std::string name)
	{
		_name = name;
	}

	const std::string &filename() const
	{
		return _filename;
	}

	const std::string &name() const
	{
		return _name;
	}

	friend void to_json(json &j, const Model &value);
	friend void from_json(const json &j, Model &value);
private:
	std::string _filename;
	std::string _name;

	std::map<std::string, std::string> _chain2Entity;
};

inline void to_json(json &j, const Model &value)
{
	j["name"] = value._name;
	j["filename"] = value._filename;
	j["chain_to_entity"] = value._chain2Entity;
}

inline void from_json(const json &j, Model &value)
{
	value._name = j.at("name");
	value._filename = j.at("filename");
	
	try
	{
		value._chain2Entity = j.at("chain_to_entity");
	}
	catch (...)
	{
		std::cout << "Error proccessing json, probably old version" << std::endl;
	}
}



#endif
