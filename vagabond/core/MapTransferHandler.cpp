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

#include "MapTransferHandler.h"
#include "ThreadMapTransfer.h"
#include "AtomGroup.h"
#include "Atom.h"
#include "ElementSegment.h"
#include "FFTCubicGrid.h"

MapTransferHandler::MapTransferHandler(BondCalculator *calculator)
{
	_calculator = calculator;
	_sumHandler = calculator->sumHandler();
}

MapTransferHandler::~MapTransferHandler()
{
	finish();
}

void MapTransferHandler::supplyAtomGroup(std::vector<Atom *> all, 
                                         std::vector<Atom *> sub)
{
	_all = all;
	_sub = sub;
	
	getRealDimensions(_sub);
}

void MapTransferHandler::getRealDimensions(std::vector<Atom *> &sub)
{
	_min = glm::vec3(+FLT_MAX, +FLT_MAX, +FLT_MAX);
	_max = glm::vec3(-FLT_MAX, -FLT_MAX, -FLT_MAX);

	for (size_t i = 0; i < sub.size(); i++)
	{
		glm::vec3 pos = sub[i]->derivedPosition();
		
		for (size_t j = 0; j < 3; j++)
		{
			_min[j] = std::min(_min[j], pos[j]);
			_max[j] = std::max(_max[j], pos[j]);
		}
	}
	
	if (sub.size() == 0)
	{
		_min = glm::vec3(0.);
		_max = glm::vec3(0.);
	}
	
	_min -= _pad;
	_max += _pad;
}

void MapTransferHandler::supplyElementList(std::map<std::string, int> elements)
{
	std::map<std::string, int>::iterator it;
	for (it = elements.begin(); it != elements.end(); it++)
	{
		std::cout << it->first << " " << it->second << std::endl;
		_elements.push_back(it->first);
	}

}

void MapTransferHandler::allocateSegments()
{
	int nx, ny, nz;
	ElementSegment::findDimensions(nx, ny, nz, _min, _max, _cubeDim);
	
	for (size_t i = 0; i < _elements.size(); i++)
	{
		std::string &ele = _elements[i];

		ElementSegment *seg = new ElementSegment();
		seg->setDimensions(nx, ny, nz);
		seg->setRealDim(_cubeDim);
		seg->setOrigin(_min);
		seg->setElement(ele);
		seg->setStatus(FFT<VoxelElement>::Real);
		seg->makePlans();
		
		_segments.push_back(seg);
		_element2Segment[ele] = seg;
	}
}

void MapTransferHandler::findThreadCount()
{
	_threads = 1;
}

void MapTransferHandler::setup()
{
	allocateSegments();
	findThreadCount();
}

void MapTransferHandler::prepareThreads()
{
	for (size_t j = 0; j < _elements.size(); j++)
	{
		Pool<ElementSegment *> &pool = _pools[_elements[j]];
		ElementSegment *seg = _element2Segment[_elements[j]];
		pool.pushObject(seg);

		for (size_t i = 0; i < _threads; i++)
		{
			/* several calculators */
			ThreadMapTransfer *worker = new ThreadMapTransfer(this, _elements[j]);
			worker->setMapSumHandler(_sumHandler);
			std::thread *thr = new std::thread(&ThreadMapTransfer::start, worker);

			pool.threads.push_back(thr);
			pool.workers.push_back(worker);
		}
	}
}

void MapTransferHandler::start()
{
	_finish = false;

	prepareThreads();
}

void MapTransferHandler::returnSegment(ElementSegment *segment)
{
	std::string ele = segment->elementSymbol();
	Pool<ElementSegment *> &pool = _pools[ele];
	pool.pushObject(segment);
}

ElementSegment *MapTransferHandler::acquireSegment(std::string ele)
{
	Pool<ElementSegment *> &pool = _pools[ele];
	ElementSegment *seg = nullptr;
	pool.acquireObject(seg, _finish);
	return seg;
}

MiniJobMap *MapTransferHandler::acquireMiniJob(std::string ele)
{
	_handout.lock();
	Pool<MiniJobMap *> &pool = _miniJobPools[ele];
	_handout.unlock();
	MiniJobMap *mini = nullptr;
	pool.acquireObject(mini, _finish);
	return mini;
}

MiniJobMap *MapTransferHandler::makeJobForElement(std::string ele,
                                               std::vector<BondSequence::ElePos> 
                                               &epos)
{
	MiniJobMap *mini = new MiniJobMap(ele);

	for (size_t i = 0; i < epos.size(); i++)
	{
		if (strcmp(&ele[0], epos[i].element) != 0)
		{
			continue;
		}
		
		glm::vec3 pos = epos[i].pos;
		mini->positions.push_back(pos);
	}
	
	return mini;
}

void MapTransferHandler::setupMiniJobs(Job *job, 
                                       std::vector<BondSequence::ElePos> &epos)
{
	for (size_t i = 0; i < _elements.size(); i++)
	{
		MiniJobMap *mini = makeJobForElement(_elements[i], epos);
		mini->setJob(job);
		_handout.lock();
		Pool<MiniJobMap *> &pool = _miniJobPools[_elements[i]];
		_handout.unlock();
		pool.pushObject(mini);
	}
}

void MapTransferHandler::finishThreads()
{
	for (size_t i = 0; i < _elements.size(); i++)
	{
		_miniJobPools[_elements[i]].signalThreads();
		_pools[_elements[i]].signalThreads();
	}

	for (size_t i = 0; i < _elements.size(); i++)
	{
		_miniJobPools[_elements[i]].joinThreads();
		_pools[_elements[i]].joinThreads();
	}

	for (size_t i = 0; i < _elements.size(); i++)
	{
		_miniJobPools[_elements[i]].cleanup();
		_pools[_elements[i]].cleanup();
	}
}

void MapTransferHandler::finish()
{
	for (size_t i = 0; i < _elements.size(); i++)
	{
		_pools[_elements[i]].handout.lock();
		_miniJobPools[_elements[i]].handout.lock();
	}
	
	_finish = true;

	for (size_t i = 0; i < _elements.size(); i++)
	{
		_pools[_elements[i]].handout.unlock();
		_miniJobPools[_elements[i]].handout.unlock();
	}

	finishThreads();
}
