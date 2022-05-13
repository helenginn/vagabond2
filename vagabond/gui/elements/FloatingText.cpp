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

#include <iostream>
#include "FloatingText.h"

FloatingText::FloatingText(std::string text, float mult) : Text(text)
{
	setVertexShaderFile("assets/shaders/floating_box.vsh");
	correctBox(mult);
	setUsesProjection(true);
}

void FloatingText::correctBox(float mult)
{
	glm::vec3 centre = centroid();

	for (Snow::Vertex &v : _vertices)
	{
		v.extra = glm::vec4(v.pos - centre, 0);
		v.extra *= mult;
		
#ifdef __EMSCRIPTEN__
		v.extra *= 10;
#endif
		v.extra.y += 0.5;
		v.pos = centre;
	}
}
