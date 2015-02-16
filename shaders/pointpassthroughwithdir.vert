#version 330

layout(location = 0) in vec4 position;
layout(location = 1) in vec4 direction;

out vec4 dir;

void main()
{
	gl_Position = position;
dir = direction;
}