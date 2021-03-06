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

#ifndef __vagabond__SequenceView__
#define __vagabond__SequenceView__

#include <vagabond/gui/elements/ForwardBackward.h>
#include <vagabond/gui/elements/ButtonResponder.h>

class IndexedSequence;
class Residue;
class Sequence;
class TextButton;

class SequenceView : public ForwardBackward
{
public:
	SequenceView(Scene *prev, IndexedSequence *sequence);

	static TextButton *button(Sequence *seq, ButtonResponder *caller);

	virtual void setup();
	virtual void refresh();
	virtual void buttonPressed(std::string tag, Button *button = nullptr);
	virtual void addExtras(TextButton *t, Residue *r);
	virtual void handleResidue(Button *button, Residue *r) {};
private:
	virtual size_t unitsPerPage();
	size_t linesPerPage();
	void getLineLimits();
	void loadLines();

	IndexedSequence *_sequence;

	int _maxLines;
	int _residuesPerPage;
};

#endif
