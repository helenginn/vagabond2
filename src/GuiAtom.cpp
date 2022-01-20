#include "GuiAtom.h"
#include "matrix_functions.h"
#include "Icosahedron.h"
#include "Atom.h"

GuiAtom::GuiAtom() : Renderable()
{
	setUsesProjection(true);
	setVertexShaderFile("assets/shaders/with_matrix.vsh");
	setFragmentShaderFile("assets/shaders/box.fsh");
	
	_template = new Icosahedron();
	_template->triangulate();
	_template->setColour(0.5, 0.5, 0.5);
	_template->resize(0.3);
}

void GuiAtom::addPosition(glm::vec3 position)
{
	if (!is_glm_vec_sane(position))
	{
		throw std::runtime_error("position contains nan or vec values");
	}

	_template->setPosition(position);
	appendObject(_template);
}

size_t GuiAtom::verticesPerAtom()
{
	return _template->vertexCount();
}

void GuiAtom::render(SnowGL *gl)
{
	glEnable(GL_DEPTH_TEST);
	glClear(GL_DEPTH_BUFFER_BIT);
	
	Renderable::render(gl);

	glDisable(GL_DEPTH_TEST);
}
