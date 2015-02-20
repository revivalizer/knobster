// generate : knobster -g -k generators/fm8_knob.frag -w 29 -h 29 -o 15 -n 128 -f fm8_knob.png
// display  : knobster -d -k generators/fm8_knob.frag -w 29 -h 29 -o 15

float saturate(float v)
{
	return clamp(v, 0.0, 1.0);
}

vec4 grey(float v, float a) // convert value and alpha to vec4 color
{
	return vec4(v*0.97, v, v*1.03, a); // slightly blue teint
}

vec4 overlay(vec4 under, vec4 over) // overlay composition using non-premuliplied color values
{
	vec4 c;

	c.rgb = over.rgb*over.a + under.rgb*under.a*(1.0-over.a);
	c.a = over.a + under.a*(1.0 - over.a);

	return c;
}

float insideblurcircle(vec2 p, float r, float b)
{
	float r_ = length(p);
	return mix(1.0, 0.0, saturate((r_ - r + b)/(b*2)));
}

float outsideblurcircle(vec2 p, float r, float b)
{
	return 1.0 - insideblurcircle(p, r, b);
}

vec4 box(vec2 p, float width, float height, vec4 col) // return color if point p is inside origo-centered box with width and height
{
	if (p.x<width && p.x>-width && p.y<height && p.y>-height)
		return col;
	else
		return vec4(0.0);
}

vec2 translate(vec2 p, vec2 o)
{
	return p - o;
}

vec2 rotate(vec2 p, float a)
{
	mat2 rot = mat2(cos(a), -sin(a), sin(a), cos(a));

	return rot*p;
}

float degtorad(float rad)
{
	return rad/180.0*3.1415926;
}

float inside(float r, float max)
{
	if (r<=max)
		return 1.0;

	return 0.0;
}

float outside(float r, float max)
{
	if (r>max)
		return 1.0;

	return 0.0;
}

float inrange(float r, float min, float max)
{
	if (min <= r && r <= max)
		return 1.0;

	return 0.0;
}

vec4 bgbox(vec2 p, float deg, float size) // return black color if p is inside box in background
{
	return outside(length(p), 10.5)*box((translate(rotate(p, degtorad(deg)), vec2(10.5, 0.0))), size, 0.5, grey(0.0, 1.0));
}

vec4 bgshade(vec2 p) // darkens bottom quarter of bg
{
	vec2 rp = rotate(p, degtorad(-135.0));

	if (rp.x >= 0.0 && rp.y >= 0.0)
		return grey(0.2, 1.0);

	return vec4(0.0);
}

// main generation function
// * pos is the rendered point, normalized
// * v is the normalized knob position
vec4 generate(vec2 pos, float v)
{
	vec2 p = pos*29.0-14.5;
	float r = length(p);

	vec4 col = vec4(0.0);

	if (r < 14.5)
	{
		// bg base color
		col = grey(0.55, 1.0);

		// knob base color
		col = overlay(col, inside(r, 9.5)*grey(1.0, 1.0));

		// 1 px rings
		col = overlay(col, inrange(r, 13.5, 14.5)*grey(0.3, 1.0));
		col = overlay(col, inrange(r, 9.5, 10.5)*grey(0.3, 1.0));

		// darken bottom quarter
		col = overlay(col, inrange(r, 10.5, 13.5)*bgshade(p));

		// black markers on bg
		col = overlay(col, bgbox(p, 90.0, 2.0));
		col = overlay(col, bgbox(p, 90.0+135.0, 3.0));
		col = overlay(col, bgbox(p, 90.0-135.0, 3.0));

		// top inset shadow
		col = overlay(col, inrange(r, 10.5, 13.5)*grey(0.0, 0.85*outsideblurcircle(p + vec2(0.0, 1.5), 13.5, 1.2)));

		// shadow under knob
		col = overlay(col, grey(-0.15, 1.0*outside(r, 10.5)*insideblurcircle(p + vec2(0.0, 1.0), 9.5, 2.0)));

		// top knob shade
		vec2 sp = p + vec2(0.0, -3.5);
		col.rgb -= 0.2*inside(r, 7.5)*exp(-length(sp)*length(sp)*0.03);

		// knob outer ring shade
		col.rgb += inrange(r, 7.5, 9.5)*2.0*pow(dot(p/15.0, vec2(0.0, 1.0)*1.0), 3.0);

		// knob marker
		col = overlay(col, box((translate(rotate(p, degtorad(90.0+135.0 - 270.0*v)), vec2(4.0, 0.0))), 2.5, 1.0, grey(0.0, 1.0)));
	}

	// enable this to draw pixel grid in display mode
	if (false)
	{
		if (fract(p.x + 0.5) < 0.06 || fract(p.y+0.5) < 0.06)
			col = overlay(col, grey(0.1, 0.75));
	}

	return col;
}

// gamma correction function
vec4 gamma(vec4 col)
{
	float gamma = 1.0/2.2;
	return vec4(pow(col.rgb, vec3(gamma)), col.a);
}
