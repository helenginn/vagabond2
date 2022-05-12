#version 330 core

in vec4 vPos;
in vec4 vColor;
in vec2 vTex;

uniform sampler2D pic_tex;

out vec4 FragColor;

void main()
{
	vec2 frag = gl_PointCoord;
	vec2 xy = vec2(frag[0], frag[1]);
	xy.x /= vTex.x;				// stores number of points
	xy.x += vTex.y / vTex.x;	// stores point index
	vec4 tex = texture(pic_tex, xy);
	if (tex[3] < 0.05)
	{
		discard;
	}

	vec4 temp = vColor;
	temp.a *= tex.a;
	FragColor = temp;
}


