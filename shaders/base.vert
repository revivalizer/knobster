#version 330

layout(location = 0) in vec4 pos;
layout(location = 1) in vec3 normal;

uniform mat4 pm;   // projection
uniform mat4 vm;   // view
uniform mat4 mm;   // model
uniform mat3 nmvm; // normal
uniform mat4 mvm;  // modelview
uniform mat4 mvpm; // modelviewprojection

out vec4 objectPos;
out vec4 worldPos;
out vec4 viewPos;
out vec3 objectNormal;
out vec3 viewNormal;

void main()
{
	objectPos = pos;
	worldPos = mm * pos;
	viewPos = mvm * pos;
	
	objectNormal = normal;
	viewNormal = normalize(nmvm * normal);

	gl_Position = mvpm * pos;
}



