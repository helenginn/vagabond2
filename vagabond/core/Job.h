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

#ifndef __vagabond__Job__
#define __vagabond__Job__

#include <vector>
#include <map>
#include <queue>
#include <thread>
#include "Atom.h"
#include "AtomMap.h"

class ThreadWorker;
class BondSequence;
class MapTransfer;
struct Result;

enum SequenceState
{
	SequenceInPreparation,
	SequenceIdle,
	SequenceCalculateReady, // also idle!
	SequenceSuperposeReady,
	SequenceShiftReady,
	SequencePositionsReady,
};

enum JobType
{
	JobNotSpecified =            0,
	JobExtractPositions =        1 << 0,
	JobCalculateDeviations =     1 << 1,
	JobCalculateMapSegment =     1 << 2,
	JobCalculateMapCorrelation = 1 << 3,
};

typedef std::map<Atom *, Atom::WithPos> AtomPosMap;

struct CustomVector
{
	float *mean = nullptr;
	int sample_num = 0;
	int size = 0;
	
	void allocate_vector(int n)
	{
		mean = new float[n];
		memset(mean, '\0', n * sizeof(float));
		size = n;
	}
	
	void destroy_vector()
	{
		delete [] mean;
		mean = nullptr;
	}
};

struct CustomInfo
{
	CustomVector *vecs;
	int nvecs;
	
	/** allocates vectors for figuring out perturbations ofspace.
	 * @param n how many different perturbations (one per discrete conformer)
	 * @param size dimension number for calculations
	 * @param size number of samplers per discrete perturbation */
	void allocate_vectors(int n, int size, int sample_num)
	{
		vecs = new CustomVector[n];
		
		int cumulative = 0;
		for (size_t i = 0; i < n; i++)
		{
			vecs[i].allocate_vector(size);
			cumulative += sample_num;
			vecs[i].sample_num = cumulative;
		}
		
		nvecs = n;
	}
	
	void destroy_vectors()
	{
		for (size_t i = 0; i < nvecs; i++)
		{
			vecs[i].destroy_vector();
		}
		
		delete [] vecs;
		nvecs = 0;
	}
};

class MiniJob;

struct Job
{
	CustomInfo custom;

	int ticket;
	JobType requests;

	Result *result = nullptr;
	std::vector<MiniJob *> miniJobs;
	
	void destroy()
	{
		miniJobs.clear();
		std::vector<MiniJob *>().swap(miniJobs);
		custom.destroy_vectors();
		delete this;
	}
};

class MiniJob
{
public:
	Job *job;
	
	void setJob(Job *j)
	{
		job = j;
//		job->miniJobs.push_back(this);
	}
};

class ElementSegment;

/*
class MiniJobMap : public MiniJob
{
public:
	ElementSegment *segment = nullptr;
	std::string ele;
	
	MiniJobMap(std::string _ele)
	{
		ele = _ele;
	}
};
*/

class MiniJobSeq : public MiniJob
{
public:
	BondSequence *seq;
};

struct Result
{
	int ticket;
	JobType requests;
	AtomPosMap aps;
	double deviation;
	AtomMap *map;

	std::mutex handout;
	
	void setFromJob(Job *job)
	{
		requests = job->requests;
		ticket = job->ticket;
		job->result = this;
	}
	
	void transplantPositions()
	{
		AtomPosMap::iterator it;
		for (it = aps.begin(); it != aps.end(); it++)
		{
			it->second.ave /= (float)it->second.samples.size();
			it->first->setDerivedPositions(it->second);
		}
	}
	
	void destroy()
	{
		aps.clear();
		delete map;

		delete this;
	}
};

#endif
