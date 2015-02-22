// generate : knobster -g -k generators/fm8_button.frag -w 19 -h 10 -o 15 -n 2 -f fm8_button.png
// display  : knobster -d -k generators/fm8_button.frag -w 19 -h 10 -o 15

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

vec2 translate(vec2 p, vec2 o)
{
	return p - o;
}

vec2 rotate(vec2 p, float a)
{
	mat2 rot = mat2(cos(a), -sin(a), sin(a), cos(a));

	return rot*p;
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

vec2 rboxdir(vec2 p, vec2 b, float r)
{
	vec2 d = p;

	if (d.x > 0.0)
		d.x = 0.3 + d.x - min(d.x, b.x);
	else
		d.x = 0.3 + d.x - max(d.x, -b.x);

	if (d.y > 0.0)
		d.y = 0.3 + d.y - min(d.y, b.y);
	else
		d.y = 0.3 + d.y - max(d.y, -b.y);

	return d;
}

float rbox( vec2 p, vec2 b, float r)
{
  return length(max(abs(p)-b,0.0))-r;
}

// main generation function
// * pos is the rendered point, normalized
// * v is the normalized knob position
vec4 generate(vec2 pos, float v)
{
	// center origo, make 1 px = 1 unit
	vec2 p = pos * vec2(19.0, 10.0) - vec2(9.5, 5.0);

	// black base color
	vec4 col = vec4(0.0);

	// distance from outer edge
	float r = max(0.0, rbox(p, vec2(6.5, 2.0), 3.0)+3.0);

	// normal of bezel
	vec2 dir = rboxdir(p, vec2(6.5, 2.0), 3.0);

	// outline
	col = overlay(col, inrange(r, 2.0, 3.0)*grey(0.1, 1.0));

	// bezel
	col = overlay(col, inrange(r, 0.5, 2.0)*grey(0.27+saturate(0.5*dot(normalize(dir), vec2(0.0, 1.0))), 1.0));

	// button center base color
	col = overlay(col, inside(r, 0.5)*grey(0.63, 1.0));

	// shade center
	float shadedist = max(0.0, rbox(translate(p, vec2(0.0, 4.5)), vec2(2.5, 0.0), 1.0));
	col -= 0.23*inside(r, 0.5)*exp(-shadedist*shadedist/15.0);

	// if button pressed
	if (v > 0.5)
	{
		// darken
		shadedist = max(0.0, rbox(translate(p, vec2(0.0, -3.5)), vec2(5.0, 3.5), 0.0));

		col = overlay(col, inside(r, 2.0)*grey(0.0, 0.0 + 0.5*saturate(0.1 + 0.3*shadedist)));

	}
	// enable this to draw pixel grid in display mode
	if (false)
	{
		if (fract(p.x + 0.5) < 0.06 || fract(p.y+0.5) < 0.0667)
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
