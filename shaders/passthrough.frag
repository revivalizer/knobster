#version 330

in vec2 pos;
uniform sampler2D tex;

void main()
{
	gl_FragColor = texture2D(tex, pos);
}
