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

#include "Model.h"
#include "Molecule.h"
#include "Chain.h"
#include "File.h"
#include "AtomContent.h"
#include "Environment.h"
#include "EntityManager.h"
#include "SequenceComparison.h"
#include "../utils/FileReader.h"

Model::Model()
{

}

void Model::setFilename(std::string file)
{
	_filename = file;
	
	if (_name.length() == 0)
	{
		_name = getBaseFilename(_filename);
	}

}

const std::string Model::entityForChain(std::string id) const
{
	if (_chain2Entity.count(id))
	{
		return _chain2Entity.at(id);
	}

	return "";
}

void Model::swapChainToEntity(std::string id, std::string entity)
{

	std::cout << "Switching entity for molecule." << std::endl;
	if (_chain2Molecule.count(id))
	{
		Molecule *mol = _chain2Molecule[id];
		if (mol != nullptr)
		{
			std::cout << "Found old molecule of entity " << 
			mol->entity_id() << std::endl;
			std::cout << "Purging old molecule from environment" << std::endl;
			_chain2Entity.erase(id);
		}
	}

	std::cout << "Assigning new entity, " << entity << std::endl;
	_chain2Entity[id] = entity;
}

void Model::setEntityForChain(std::string id, std::string entity)
{
	if (_chain2Entity.count(id) && _chain2Entity[id] != entity)
	{
		swapChainToEntity(id, entity);
	}
	else
	{
		_chain2Entity[id] = entity;
	}
}

bool Model::hasEntity(std::string entity)
{
	std::map<std::string, std::string>::iterator it;
	
	for (it = _chain2Entity.begin(); it != _chain2Entity.end(); it++)
	{
		if (it->second == entity)
		{
			return true;
		}
	}
	
	return false;
}

Molecule *Model::moleculeFromChain(Chain *ch)
{
	std::string entity = entityForChain(ch->id());
	if (entity == "")
	{
		return nullptr;
	}

	Molecule mc(name(), entity, ch->fullSequence());
	_molecules.push_back(mc);
	
	Molecule &ref = _molecules.back();

	ref.setChain(ch->id());
	ref.getTorsionRefs(ch);
	
	return &_molecules.back();
}

void Model::unload()
{
	if (_currentFile)
	{
		delete _currentAtoms;
		delete _currentFile;
	}

	_currentFile = nullptr;
	_currentAtoms = nullptr;
}

void Model::createMolecules()
{
	std::map<std::string, std::string>::iterator it;
	int extra = 0;
	
	for (it = _chain2Entity.begin(); it != _chain2Entity.end(); it++)
	{
		std::string id = it->first;
		if (_chain2Molecule.count(id) > 0)
		{
			continue;
		}
		
		if (_currentFile == nullptr)
		{
			_currentFile = File::loadUnknown(_filename);
			_currentAtoms = _currentFile->atoms();
		}

		Chain *ch = _currentAtoms->chain(id);
		
		if (ch)
		{
			moleculeFromChain(ch);
			extra++;
		}
	}
	
	unload();
}

void Model::autoAssignEntities()
{
	EntityManager *eManager = Environment::entityManager();
	load();

	for (size_t i = 0; i < _currentAtoms->chainCount(); i++)
	{
		float best_match = 0;
		Entity *best_entity = nullptr;
		Chain *ch = _currentAtoms->chain(i);
		Sequence *compare = ch->fullSequence();

		for (size_t i = 0; i < eManager->objectCount(); i++)
		{
			Entity &ent = eManager->object(i);
			Sequence *master = ent.sequence();

			SequenceComparison *sc = new SequenceComparison(master, compare);
			float match = sc->match();
			
			if (match > best_match)
			{
				best_match = match;
				best_entity = &ent;
			}
			
			delete sc;
		}

		if (best_match > 0.8)
		{
			std::cout << "Chain " << ch->id() << " in model " << name() <<
			 " becomes " << best_entity->name() << std::endl;
			setEntityForChain(ch->id(), best_entity->name());
		}
	}
	
	housekeeping();
}

const Metadata::KeyValues Model::metadata() const
{
	Metadata *md = Environment::metadata();
	const Metadata::KeyValues kv = *md->values(_name, _filename);

	return kv;
}

void Model::housekeeping()
{
	for (Molecule &mc : _molecules)
	{
		std::string ch = mc.chain_id();
		_chain2Molecule[ch] = &mc;
		mc.setModel(this);
	}
	
	if (_chain2Molecule.size() >= _chain2Entity.size())
	{
		return;
	}

	createMolecules();
}

void Model::insertTorsions()
{
	for (Molecule &mc : _molecules)
	{
		mc.insertTorsionAngles(_currentAtoms);
	}
}

void Model::extractTorsions()
{
	std::cout << "Extracting torsions from model " << name() << std::endl;
	for (Molecule &mc : _molecules)
	{
		mc.extractTorsionAngles(_currentAtoms);
	}
}

void Model::load()
{
	_currentFile = File::loadUnknown(_filename);
	_currentAtoms = _currentFile->atoms();
	
	insertTorsions();

	_currentAtoms->setResponder(this);
}

void Model::refine()
{
	if (_currentAtoms == nullptr)
	{
		load();
	}

	_currentAtoms->alignAnchor();
	_currentAtoms->refinePositions();
}

size_t Model::moleculeCountForEntity(std::string entity_id)
{
	std::map<std::string, std::string>::iterator it;
	size_t count = 0;
	
	for (it = _chain2Entity.begin(); it != _chain2Entity.end(); it++)
	{
		std::string name = it->second;
		
		if (name == entity_id)
		{
			count++;
		}
	}

	return count;
}

void Model::finishedRefinement()
{
	extractTorsions();

	if (_responder)
	{
		_responder->modelReady();
	}

	unload();
}

Model Model::autoModel(std::string filename)
{
	Model m;
	m.setFilename(filename);

	return m;
}

void Model::throwOutMolecule(Molecule *mol)
{
	std::list<Molecule>::iterator it = _molecules.begin();

	for (Molecule &m : _molecules)
	{
		if (mol == &m)
		{
			_molecules.erase(it);
			return;
		}
		
		it++;
	}
}

void Model::throwOutEntity(Entity *ent)
{
	std::list<Molecule>::iterator it = _molecules.begin();

	std::map<std::string, std::string>::iterator jt;
	for (jt = _chain2Entity.begin(); jt != _chain2Entity.end(); jt++)
	{
		if (jt->second == ent->name())
		{
			std::cout << "Removing " << jt->first << " as instance of " << 
			"entity " << ent->name() << " from " << name() << std::endl;
			_chain2Entity.erase(jt);
		}
	}

	for (Molecule &m : _molecules)
	{
		if (m.entity_id() == ent->name())
		{
			std::string chain = m.chain_id();
			_chain2Molecule.erase(chain);
			std::cout << "Found created molecule " << chain << " to remove "
			"from " << name() << std::endl;

			_molecules.erase(it);
			return;
		}
		
		it++;
	}
}
