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

#include "ThreadMiniJobForSequence.h"
#include "BondSequenceHandler.h"
#include "BondSequence.h"

ThreadMiniJobForSequence::ThreadMiniJobForSequence(BondSequenceHandler *h)
{
	_handler = h;
}

void ThreadMiniJobForSequence::start()
{
	do
	{
		SequenceState state = SequenceIdle;

		MiniJobSeq *job = _handler->acquireMiniJobSeq();
		if (job == nullptr)
		{
			break;
		}

		BondSequence *seq = _handler->acquireSequence(state);
		
		if (seq == nullptr)
		{
			_handler->signalFinishMiniJobSeq();
			break;
		}

		seq->setMiniJobSeqInfo(job);
	}
	while (!_finish);
}
