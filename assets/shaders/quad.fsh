precision lowp float;
varying vec4 vPos;
varying vec2 vTex;

uniform sampler2D pic_tex;

void main()
{
	vec4 result = texture(pic_tex, vTex);
	gl_FragColor = result;
}


