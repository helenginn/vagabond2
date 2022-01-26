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

#ifndef __vagabond__BondCalculator__
#define __vagabond__BondCalculator__

#include <climits>
#include <vector>
#include <queue>
#include "Handler.h"

class Atom;
class BondSequenceHandler;

class BondCalculator : public Handler
{
public:
	BondCalculator();
	~BondCalculator();
	
	enum PipelineType
	{
		PipelineNotSpecified,
		PipelineAtomPositions,
		PipelineCalculatedMaps,
	};
	
	void submitJob(Job &job);
	void submitResult(Result *result);
	
	void setTotalSamples(size_t total)
	{
		_totalSamples = total;
	}
	
	void setPipelineType(PipelineType type)
	{
		_type = type;
	}
	
	/** @param max must be explicitly specified as non-zero positive */
	void setMaxSimultaneousThreads(size_t max)
	{
		_maxThreads = max;
	}
	
	/** @param size UINT_MAX if unlimited, otherwise maximum bytes as limit 
	 * for map calculations */
	void setMaxMemoryGuideline(unsigned long max)
	{
		_maxMemory = max;
	}
	
	void addAnchorExtension(Atom *atom, size_t bondCount = UINT_MAX);

	void setup();
	void start();
	void finish();
	
	BondSequenceHandler *sequenceHandler()
	{
		return _sequenceHandler;
	}
	
	struct AnchorExtension
	{
		Atom *atom;
		size_t count;
	};

	Job *acquireJob();
	Result *acquireResult();
private:
	void sanityCheckPipeline();
	void sanityCheckThreads();
	void setupSequenceHandler();
	void prepareThreads();

	PipelineType _type;
	size_t _maxThreads;
	size_t _maxMemory;
	size_t _totalSamples;
	std::atomic<long int> _max_id;
	std::atomic<long int> _running;
	
	Pool<Job *> _jobPool;
	Pool<Result *> _resultPool;
	Pool<Result *> _recyclePool;

	BondSequenceHandler *_sequenceHandler;
	
	std::vector<AnchorExtension> _atoms;
};

#endif