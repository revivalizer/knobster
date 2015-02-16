#version 330

in vec2 pos;

uniform sampler2D input0;

uniform float exposure;
uniform int mode;

void main()
{
	if (mode==0)
	{
		vec3 texColorIn = texture2D(input0, pos).xyz;
		vec3 texColorExp = texColorIn * exposure;  // Exposure adjustment
		texColorExp = texColorExp/(1.0+texColorExp);
		vec3 retColor = pow(texColorExp, vec3(1.0/2.2));

		gl_FragData[0] = vec4(retColor, 1.0);
	}
	else if (mode==1)
	{
		vec3 texColorIn = texture2D(input0, pos).xyz;
		vec3 texColorExp = texColorIn * exposure;  // Exposure adjustment
		vec3 x = max(vec3(0.0), texColorExp-0.004);
		vec3 retColor = (x*(6.2*x+0.5))/(x*(6.2*x+1.7)+0.06);

		vec3 finalColor = retColor;

		gl_FragData[0] = vec4(finalColor, 1.0);
	}

}
