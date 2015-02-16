#version 330

uniform mat4 pm;   // projection
uniform mat4 vm;   // view
uniform mat4 mm;   // model
uniform mat3 nmvm; // normal
uniform mat4 mvm;  // modelview
uniform mat4 mvpm; // modelviewprojection

in vec4 objectPos;
in vec4 worldPos;
in vec4 viewPos;
in vec3 objectNormal;
in vec3 viewNormal;

void main()
{
	gl_FragColor = vec4(1.0, 0.5, 0.5, 1.0);
}
