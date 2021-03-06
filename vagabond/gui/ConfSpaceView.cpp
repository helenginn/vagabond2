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

#include "RulesMenu.h"
#include "LineSeries.h"
#include "ConfSpaceView.h"
#include "Axes.h"
#include "ColourLegend.h"
#include "IconLegend.h"
#include "SerialRefiner.h"
#include "ClusterView.h"
#include "AddModel.h"

#include <vagabond/utils/FileReader.h>
#include <vagabond/gui/elements/AskYesNo.h>
#include <vagabond/gui/elements/ChooseRange.h>
#include <vagabond/gui/elements/ImageButton.h>
#include <vagabond/gui/elements/TextButton.h>
#include <vagabond/gui/elements/Menu.h>
#include <vagabond/c4x/ClusterSVD.h>
#include <vagabond/c4x/ClusterTSNE.h>

#include <vagabond/core/Entity.h>
#include <vagabond/core/Environment.h>
#include <vagabond/core/Metadata.h>

ConfSpaceView::ConfSpaceView(Scene *prev, Entity *ent) 
: Scene(prev),
Mouse3D(prev),
IndexResponseView(prev)
{
	_entity = ent;
}

ConfSpaceView::~ConfSpaceView()
{
	deleteObjects();
}

void ConfSpaceView::askToFoldIn(int extra)
{
	std::string str = "Found " + i_to_str(extra) + " unrefined molecules.\n";
	str += "Refine torsion angles to atom positions\nnow?";
	AskYesNo *askyn = new AskYesNo(this, str, "fold_in", this);
	setModal(askyn);
}

void ConfSpaceView::setup()
{
	IndexResponseView::setup();

	size_t extra = _entity->checkForUnrefinedMolecules();

	if (extra > 0)
	{
		_extra = extra;
		askToFoldIn(extra);
	}
	else
	{
		removeRules();
		showClusters();
		applyRules();
		showRulesButton();
	}
	
	{
		Text *text = new Text(_entity->name());
		text->setCentre(0.5, 0.85);
		addObject(text);
	}
}

void ConfSpaceView::showClusters()
{
	MetadataGroup angles = _entity->makeTorsionDataGroup();
	angles.setWhiteList(_whiteList);
	angles.write(_entity->name() + ".csv");
	angles.normalise();
	
	ClusterView *view = new ClusterView();
	addIndexResponder(view);
	view->setIndexResponseView(this);
	view->setConfSpaceView(this);

	if (_tsne)
	{
		ClusterTSNE<MetadataGroup> *cx = new ClusterTSNE<MetadataGroup>(angles);
		cx->cluster();
		view->setCluster(cx);
	}
	else
	{
		ClusterSVD<MetadataGroup> *cx = new ClusterSVD<MetadataGroup>(angles);
		cx->cluster();
		view->setCluster(cx);
	}

	_centre = view->centroid();
	_translation = -_centre;
	_translation.z -= 10;
	updateCamera();

	addObject(view);

	_view = view;
}

void ConfSpaceView::showRulesButton()
{
	ImageButton *b = new ImageButton("assets/images/palette.png", this);
	b->resize(0.1);
	b->setRight(0.95, 0.09);
	b->setReturnTag("rules");

	Text *t = new Text("settings");
	t->resize(0.6);
	t->setRight(0.95, 0.15);
	addObject(b);
	addObject(t);

	{
		TextButton *t = new TextButton("t-SNE", this);
		t->setReturnTag("tsne");
		t->resize(0.6);
		t->setRight(0.95, 0.22);
		addObject(t);
	}
}

void ConfSpaceView::chooseGroup(Rule *rule, bool inverse)
{
	MetadataGroup *mg = _view->cluster()->dataGroup();
	std::vector<HasMetadata *> whiteList;

	for (size_t i = 0; i < mg->objectCount(); i++)
	{
		HasMetadata *hm = mg->object(i);
		Metadata::KeyValues kv = hm->metadata();
		std::string expected = rule->headerValue();
		std::string value;
		
		if (kv.count(rule->header()) > 0)
		{
			value = kv.at(rule->header()).text();
		}
		else if (!rule->ifAssigned())
		{
			continue;
		}

		bool hit = false;

		if (rule->ifAssigned() && value.length())
		{
			hit = true;
		}
		else if (!rule->ifAssigned())
		{
			hit = (value == expected);
		}

		if (inverse)
		{
			hit = !hit;
		}

		if (hit)
		{
			whiteList.push_back(hm);
		}
	}
	
	std::cout << "Collected " << whiteList.size() << " molecules." << std::endl;
	
	ConfSpaceView *view = new ConfSpaceView(this, _entity);
	view->setWhiteList(whiteList);
	view->show();
}

void ConfSpaceView::executeSubset(float min, float max)
{
	MetadataGroup *mg = _view->cluster()->dataGroup();
	std::vector<HasMetadata *> whiteList;

	for (size_t i = 0; i < mg->objectCount(); i++)
	{
		HasMetadata *hm = mg->object(i);
		Metadata::KeyValues kv = hm->metadata();
		
		if (kv.count(_colourRule->header()) > 0)
		{
			float num = kv.at(_colourRule->header()).number();
			
			if (num >= min && num <= max)
			{
				whiteList.push_back(hm);
			}
		}
	}
	
	std::cout << "Collected " << whiteList.size() << " molecules." << std::endl;
	
	ConfSpaceView *view = new ConfSpaceView(this, _entity);
	view->setWhiteList(whiteList);
	view->show();
}

void ConfSpaceView::buttonPressed(std::string tag, Button *button)
{
	if (tag == "choose_reorient_molecule")
	{
		setInformation("choose molecule to reorient axis direction...");
		_status = Reorienting;
	}
	if (tag == "choose_subset")
	{
		std::string str = "Focus on " + _colourRule->header() + " subset:";
		ChooseRange *cr = new ChooseRange(this, str, "execute_subset", 
		                                  this, true);
		cr->setRange(_colourRule->min(), _colourRule->max(), 100);
		setModal(cr);
	}

	if (tag == "execute_subset") // from colour
	{
		ChooseRange *cr = static_cast<ChooseRange *>(button->returnObject());
		float min = cr->min();
		float max = cr->max();
		executeSubset(min, max);
		std::cout << "chosen " << min << " to " << max << std::endl;
	}

	if (tag == "choose_group" || tag == "choose_inverse") // from icon
	{
		Rule *rule = static_cast<Rule *>(button->returnObject());
		chooseGroup(rule, (tag == "choose_inverse"));
	}

	if (tag == "align_axes")
	{
		AskYesNo *askyn = new AskYesNo(this, "Prioritise PCA axes to best\n"
		                               "represent the colour legend?", 
		                               "align_axes", this);
		setModal(askyn);
	}
	if (tag == "yes_align_axes")
	{
		std::string key = _colourRule->header();
		_view->prioritiseMetadata(key);
		applyRules();
	}
	
	if (tag == "tsne")
	{
		ConfSpaceView *view = new ConfSpaceView(this, _entity);
		view->setWhiteList(_whiteList);
		view->setTSNE(true);
		view->show();
	}

	if (tag == "yes_fold_in")
	{
		// refine extra molecules
		SerialRefiner *refiner = new SerialRefiner(this, _entity);
		refiner->setExtra(_extra);
		refiner->show();
	}
	if (tag == "no_fold_in")
	{
		// show conf space
		showClusters();
		applyRules();
		showRulesButton();
	}
	
	if (tag == "view_model")
	{
		Molecule *m = static_cast<Molecule	*>(button->returnObject());
		
		if (m)
		{
			AddModel *am = new AddModel(this, m->model());
			am->setDeleteAllowed(false);
			am->show();
		}
	}
	
	if (tag == "set_as_reference")
	{
		Molecule *m = static_cast<Molecule	*>(button->returnObject());
		createReference(m);
	}
	
	if (tag == "rules")
	{
		RulesMenu *menu = new RulesMenu(this);
		menu->setEntityId(_entity->name());
		menu->setData(_view->cluster()->dataGroup());
		menu->show();
	}

	Scene::buttonPressed(tag, button);
}

void ConfSpaceView::refresh()
{
	if (_view == nullptr)
	{
		showClusters();
		showRulesButton();
	}

	applyRules();
}

void ConfSpaceView::applyRule(const Rule &r)
{
	if (r.type() == Rule::LineSeries)
	{
		LineSeries *ls = new LineSeries(_view, r);
		addObject(ls);
		_temps.push_back(ls);
	}
	else
	{
		_view->applyRule(r);
	}
	
	if (r.type() == Rule::VaryColour)
	{
		ColourLegend *legend = new ColourLegend(r.scheme(), this);
		legend->setCentre(0.5, 0.1);
		legend->setTitle(r.header());
		legend->setLimits(r.min(), r.max());
		addObject(legend);
		_temps.push_back(legend);
		_colourRule = &r;
	}
}

void ConfSpaceView::removeRules()
{
	_colourRule = nullptr;

	for (size_t i = 0; i < _temps.size(); i++)
	{
		removeObject(_temps[i]);
		delete _temps[i];
	}

	_temps.clear();
}

void ConfSpaceView::applyRules()
{
	_view->reset();
	removeRules();

	IconLegend *il = new IconLegend(this);

	const Ruler &ruler = Environment::metadata()->ruler();

	for (const Rule &r : ruler.rules())
	{
		applyRule(r);
		il->addRule(&r);
	}
	
	il->makePoints();
	il->setCentre(0.8, 0.5);
	addObject(il);
	_temps.push_back(il);
}

void ConfSpaceView::prepareMenu(HasMetadata *hm)
{
	Menu *m = new Menu(this);
	m->setReturnObject(hm);
	m->addOption("view model", "view_model");
	m->addOption("set as reference", "set_as_reference");
	double x = _lastX / (double)_w; double y = _lastY / (double)_h;
	m->setup(x, y);

	setModal(m);
}

void ConfSpaceView::createReference(Molecule *m)
{
	if (_axes != nullptr)
	{
		removeObject(_axes);
		removeResponder(_axes);
		delete _axes;
		_axes = nullptr;
	}

	_axes = new Axes(_view->cluster(), m);
	_axes->setScene(this);
	_axes->setIndexResponseView(this);
	addObject(_axes);
	addIndexResponder(_axes);
}


void ConfSpaceView::reorientToMolecule(Molecule *mol)
{
	if (_axes == nullptr)
	{
		return;
	}
	
	setInformation("");
	_status = Nothing;
	
	_axes->reorient(-1, mol);
}
