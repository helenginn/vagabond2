#version 330 core

in vec3 normal;
in vec3 position;
in vec4 color;
in vec4 extra;
in vec2 tex;

uniform mat4 projection;
uniform mat4 model;
uniform vec3 centre;

out vec4 vPos;
out vec3 dPos;
out vec4 vColor;
out vec3 vNormal;
out vec2 vTex;

void main()
{
    vec4 pos = vec4(position[0], position[1], position[2], 1.0);
	gl_Position = projection * model * pos;
	mat3 rot = mat3(model);
	vNormal = rot * normal;
	vTex = tex;
	vColor = color;
	vPos = vec4(rot * vec3(extra), 1.);
	dPos = vec3(model * pos) - centre;
}


