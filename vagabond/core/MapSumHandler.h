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

#ifndef __vagabond__MapSumHandler__
#define __vagabond__MapSumHandler__

#include "Handler.h"
#include "BondCalculator.h"

class MiniJobMap;
class AtomSegment;

class MapSumHandler : public Handler
{
public:
	MapSumHandler(BondCalculator *calculator = nullptr);
	~MapSumHandler();
	
	void setMapHandler(MapTransferHandler *handler)
	{
		_mapHandler = handler;
	}
	
	void setMapCount(int maps)
	{
		_mapCount = maps;
	}
	
	void setThreads(int threads)
	{
		_threads = threads;
	}
	
	size_t segmentCount()
	{
		return _mapPool.members.size();
	}
	
	struct MapJob
	{
		AtomSegment *segment;
		Job *job;
		std::atomic<int> summed{0};
	};

	MiniJobMap *acquireMiniJob(MapJob *&mj);
	void transferJob(MiniJobMap *mini);
	void returnMiniJob(MapJob *mj);
	void returnSegment(AtomSegment *segment);
	void setup();

	void start();
	void finish();
private:
	void finishThreads();
	void createSegments();
	void prepareThreads();

	MapJob *acquireMapJob(Job *job);
	
	std::map<int, MapJob *> _ticketMap;
	std::mutex _handout;
	std::mutex _ticketHandout;
	
	Pool<AtomSegment *> _mapPool;
	Pool<MiniJobMap *> _miniJobPool;

	BondCalculator *_calculator = nullptr;
	MapTransferHandler *_mapHandler = nullptr;
	
	int _threads = 1;
	int _mapCount = 1;
};

#endif
