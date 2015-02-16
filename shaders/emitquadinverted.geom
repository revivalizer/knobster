#version 330 

layout(points) in;
layout(triangle_strip, max_vertices = 4) out;

out vec2 pos;

void main()
{
	vec3 o = vec3(1.0, 0.0, -1.0);

	vec4 p = gl_in[0].gl_Position;

	gl_Position = p + o.xzyy;
	pos = vec2(1.0, 1.0);
	EmitVertex();

	gl_Position = p + o.xxyy;
	pos = vec2(1.0, 0.0);
	EmitVertex();

	gl_Position = p + o.zzyy;
	pos = vec2(0.0, 1.0);
	EmitVertex();

	gl_Position = p + o.zxyy;
	pos = vec2(0.0, 0.0);
	EmitVertex();
}