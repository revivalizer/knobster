#include "pch.h"
#include <iostream>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#pragma warning(push)
#pragma warning(disable: 4510 4512 4610 4127 4701 4703)
#include "optionparser.h"
#pragma warning(pop)

int curFrame;

void SetupFBO(ZRenderer& renderer, ZFBO* fbo, ZTexture* depthTexture = nullptr, ZTexture* colorTexture0 = nullptr, ZTexture* colorTexture1 = nullptr, ZTexture* colorTexture2 = nullptr, ZTexture* colorTexture3 = nullptr)
{
	uint32_t numColorBuffers = 0;

	renderer.BindFBO(fbo);

	if (depthTexture)
	{
		fbo->AttachTexture2D(ZFBO::kDepth, depthTexture, 0);
	}

	if (colorTexture0)
	{
		fbo->AttachTexture2D(ZFBO::kColor0, colorTexture0, 0);
		numColorBuffers = 1;
	}

	if (colorTexture1)
	{
		fbo->AttachTexture2D(ZFBO::kColor1, colorTexture1, 0);
		numColorBuffers = 2;
	}

	if (colorTexture2)
	{
		fbo->AttachTexture2D(ZFBO::kColor2, colorTexture2, 0);
		numColorBuffers = 3;
	}

	if (colorTexture3)
	{
		fbo->AttachTexture2D(ZFBO::kColor3, colorTexture3, 0);
		numColorBuffers = 4;
	}

	uint32_t buffers[] = {kGLDrawBufferColor0, kGLDrawBufferColor01, kGLDrawBufferColor012, kGLDrawBufferColor0123};
	
	if (numColorBuffers > 0)
		renderer.BindDrawBuffers(buffers[numColorBuffers-1]);
	else
		renderer.BindDrawBuffers(0);
}

class ZMaterial 
{
public:
	ZMaterial()
		: checkTime(-1)
		, updateTime(-1)
	{
	}

	int32_t checkTime;
	int32_t updateTime;

	virtual void Update() {};
	virtual void Activate(ZRenderer& renderer) { renderer; };

	ZProgramEx* program;
};


class ZPPPassthrough : public ZMaterial 
{
public:
	ZPPPassthrough()
	{
		program = new ZProgramEx();
		program->vs = new ZShaderEx(kVertexShader);
//`		program->vs->sources.push_back(new ZWatchFile("shaders/"))
		tex = -1; loc_tex = -1;
	}

	virtual void Update()
	{
		if (checkTime==curFrame)
				return;
		
		checkTime = curFrame; 
		program->Update(); 
		
		if (updateTime < program->linkTime)
		{
			updateTime = curFrame;
			loc_tex = ZUniform::GetLoc(*program, "tex");
		} 
	}

	virtual void Activate(ZRenderer& renderer)
	{
		Update();
		renderer.UseProgram(*program);
		program->SetCommonLocations(renderer);
		ZUniform::SetInt(loc_tex, tex);
	}

	int32_t tex; ZUniLoc loc_tex;
};

char* pointpassthrough =
	"#version 330\n"
	"layout(location = 0) in vec4 position;\n"
	"void main()\n"
	"{\n"
	"	gl_Position = position;\n"
	"}\n";

char* emitquad = "#version 330 \n"
	"layout(points) in;\n"
	"layout(triangle_strip, max_vertices = 4) out;\n"
	"out vec2 pos;\n"
	"void main()\n"
	"{\n"
	"	vec3 o = vec3(1.0, 0.0, -1.0);\n"
	"	vec4 p = gl_in[0].gl_Position;\n"
	"	gl_Position = p + o.xzyy;\n"
	"	pos = vec2(1.0, 0.0);\n"
	"	EmitVertex();\n"
	"	gl_Position = p + o.xxyy;\n"
	"	pos = vec2(1.0, 1.0);\n"
	"	EmitVertex();\n"
	"	gl_Position = p + o.zzyy;\n"
	"	pos = vec2(0.0, 0.0);\n"
	"	EmitVertex();\n"
	"	gl_Position = p + o.zxyy;\n"
	"	pos = vec2(0.0, 1.0);\n"
	"	EmitVertex();\n"
	"}\n";

char* passthrough = 
	"#version 330\n"
	"in vec2 pos;\n"
	"uniform sampler2D tex;\n"
	"void main()\n"
	"{\n"
	"	gl_FragColor = texture2D(tex, pos);\n"
	"}\n";

char* generatorprelude =
	"#version 330\n"
	"in vec2 pos;\n"
	"uniform vec2 pxSize;\n"
	"uniform float v;\n"
	"uniform float oversample;\n";

char* generatordisplay =
	"void main()\n"
	"{\n"
		"gl_FragColor = gamma(generate(pos, v));\n"
	"} \n";

char* generatoroversample =
	"float lanczos(float x, float a)\n"
	"{\n"
		"if (x < -a || x > a)\n"
			"return 0.0;\n"
		"if (x==0.f)\n"
			"return 1.f;\n"
"\n"
		"float pi = 3.1415926;\n"
		"return a*sin(pi*x)*sin(pi*x/a) / (x*x*pi*pi);\n"
	"}\n"
"\n"
"\n"
	"float hermite(float x)\n"
	"{\n"
		"float ax = abs(x);\n"
"\n"
		"if (x > 1.0)\n"
			"return 0.0;\n"
"\n"
		"return 2.0*x*x*x - 3.0*x*x + 1.0;\n"
	"}\n"

	"void main()\n"
	"{\n"
		"float a = 1.0;\n"
		"float delta = 2.0*a/(oversample-1.0);\n"
"\n"
		"vec4 s = vec4(0.0);\n"
		"float accWeight = 0.0;\n"
"\n"
		"for (int y=0; y<oversample; y++)\n"
		"{\n"
			"for (int x=0; x<oversample; x++)\n"
			"{\n"
				"vec2 offset = vec2(-a + float(x)*delta, -a + float(y)*delta);\n"
				"float d = sqrt(offset.x*offset.x + offset.y*offset.y);\n"
				"vec2 p = pos + pxSize*offset;\n"
				"float weight = hermite(d);\n"
				"s += generate(p, v)*weight;\n"
				"accWeight += weight;\n"
			"}\n"
		"}\n"
"\n"
		"s /= accWeight;\n"
"\n"
		"gl_FragColor = gamma(s);\n"
	"}\n";


class ZMPassthrough : public ZMaterial 
{
public:
	ZMPassthrough()
	{
		program = new ZProgramEx();
		program->vs = new ZShaderEx(kVertexShader);
		program->vs->sources.push_back(new ZStaticString(pointpassthrough));
		program->gs = new ZShaderEx(kGeometryShader);
		program->gs->sources.push_back(new ZStaticString(emitquad));
		program->fs = new ZShaderEx(kFragmentShader);
		program->fs->sources.push_back(new ZStaticString(passthrough));
		tex = -1; loc_tex = -1;
	}

	virtual void Update()
	{
		if (checkTime==curFrame)
				return;
		
		checkTime = curFrame; 
		program->Update(); 
		
		if (updateTime < program->linkTime)
		{
			updateTime = curFrame;
			loc_tex = ZUniform::GetLoc(*program, "tex");
		} 
	}

	virtual void Activate(ZRenderer& renderer)
	{
		Update();
		renderer.UseProgram(*program);
		program->SetCommonLocations(renderer);
		ZUniform::SetInt(loc_tex, tex);
	}

	int32_t tex; ZUniLoc loc_tex;
};

class ZMDisplay : public ZMaterial 
{
public:
	ZMDisplay(const char* generatorPath)
	{
		program = new ZProgramEx();
		program->vs = new ZShaderEx(kVertexShader);
		program->vs->sources.push_back(new ZStaticString(pointpassthrough));
		program->gs = new ZShaderEx(kGeometryShader);
		program->gs->sources.push_back(new ZStaticString(emitquad));
		program->fs = new ZShaderEx(kFragmentShader);
		program->fs->sources.push_back(new ZStaticString(generatorprelude));
		program->fs->sources.push_back(new ZWatchFile(generatorPath));
		program->fs->sources.push_back(new ZStaticString(generatordisplay));
		v = 0.f; loc_v = -1;
	}

	virtual void Update()
	{
		if (checkTime==curFrame)
				return;
		
		checkTime = curFrame; 
		program->Update(); 
		
		if (updateTime < program->linkTime)
		{
			updateTime = curFrame;
			loc_v = ZUniform::GetLoc(*program, "v");
		} 
	}

	virtual void Activate(ZRenderer& renderer)
	{
		Update();
		renderer.UseProgram(*program);
		program->SetCommonLocations(renderer);
		ZUniform::SetFloat(loc_v, v);
	}

	float v; ZUniLoc loc_v;
};

class ZMOversample : public ZMaterial 
{
public:
	ZMOversample(const char* generatorPath)
	{
		program = new ZProgramEx();
		program->vs = new ZShaderEx(kVertexShader);
		program->vs->sources.push_back(new ZStaticString(pointpassthrough));
		program->gs = new ZShaderEx(kGeometryShader);
		program->gs->sources.push_back(new ZStaticString(emitquad));
		program->fs = new ZShaderEx(kFragmentShader);
		program->fs->sources.push_back(new ZStaticString(generatorprelude));
		program->fs->sources.push_back(new ZWatchFile(generatorPath));
		program->fs->sources.push_back(new ZStaticString(generatoroversample));
		v = 0.f; loc_v = -1;
		oversample = 0.f; loc_oversample = -1;
	}

	virtual void Update()
	{
		if (checkTime==curFrame)
				return;
		
		checkTime = curFrame; 
		program->Update(); 
		
		if (updateTime < program->linkTime)
		{
			updateTime = curFrame;
			loc_v = ZUniform::GetLoc(*program, "v");
			loc_oversample = ZUniform::GetLoc(*program, "oversample");
		} 
	}

	virtual void Activate(ZRenderer& renderer)
	{
		Update();
		renderer.UseProgram(*program);
		program->SetCommonLocations(renderer);
		ZUniform::SetFloat(loc_v, v);
		ZUniform::SetFloat(loc_oversample, oversample);
	}

	float v; ZUniLoc loc_v;
	float oversample; ZUniLoc loc_oversample;
};


struct Arg: public option::Arg
{
  static void printError(const char* msg1, const option::Option& opt, const char* msg2)
  {
    fprintf(stderr, "%s", msg1);
    fwrite(opt.name, opt.namelen, 1, stderr);
    fprintf(stderr, "%s", msg2);
  }

  static option::ArgStatus Unknown(const option::Option& option, bool msg)
  {
    if (msg) printError("Unknown option '", option, "'\n");
    return option::ARG_ILLEGAL;
  }

  static option::ArgStatus Required(const option::Option& option, bool msg)
  {
    if (option.arg != 0)
      return option::ARG_OK;

    if (msg) printError("Option '", option, "' requires an argument\n");
    return option::ARG_ILLEGAL;
  }

  static option::ArgStatus NonEmpty(const option::Option& option, bool msg)
  {
    if (option.arg != 0 && option.arg[0] != 0)
      return option::ARG_OK;

    if (msg) printError("Option '", option, "' requires a non-empty argument\n");
    return option::ARG_ILLEGAL;
  }

  static option::ArgStatus Numeric(const option::Option& option, bool msg)
  {
    char* endptr = 0;
    if (option.arg != 0 && strtol(option.arg, &endptr, 10)){};
    if (endptr != option.arg && *endptr == 0)
      return option::ARG_OK;

    if (msg) printError("Option '", option, "' requires a numeric argument\n");
    return option::ARG_ILLEGAL;
  }
};

enum  optionIndex { UNKNOWN, ACTION, GENERATORFILE, WIDTH, HEIGHT, OVERSAMPLING, NUM, OUTFILE };

enum
{
	kActionDisplay = 0,
	kActionGenerate = 1,
};

const option::Descriptor usage[] = {
{ UNKNOWN, 0,"", "",        Arg::Unknown, "USAGE: knobster [options]\n\n"
                                          "Options:" },
{ ACTION,        kActionDisplay,  "d", "display",       Arg::None,     "  -d <arg>, \t--display            \tAction: display high res version of knob." },
{ ACTION,        kActionGenerate, "g", "generate",      Arg::None,     "  -g <arg>, \t--generate           \tAction: generate downsampled version of knob." },
{ GENERATORFILE, 0,               "k", "knobfile",     Arg::Required, "  -k <arg>, \t--knobfile=<arg>     \tPath to GLSL generator file." },
{ WIDTH,         0,               "w", "width",        Arg::Numeric,  "  -w <arg>, \t--width=<arg>        \tWidth in pixels of output images." },
{ HEIGHT,        0,               "h", "height",       Arg::Numeric,  "  -h <arg>, \t--height=<arg>       \tHeight in pixels of output images." },
{ OVERSAMPLING,  0,               "o", "oversampling", Arg::Numeric,  "  -o <arg>, \t--oversampling=<arg> \tOversampling amount in both generator and display." },
{ NUM,           0,               "n", "num",          Arg::Numeric,  "  -n <arg>, \t--num=<arg>          \tNumber of images in strip." },
{ OUTFILE,       0,               "f", "file",         Arg::Required, "  -f <arg>, \t--file=<arg>         \tPNG output file path." },
  
{ UNKNOWN, 0,"", "",        Arg::None,
 "\nExamples:\n"
 "  knobster -d -k knob.frag -w 29 -h 29 -o 15"
 "  knobster -g -k knob.frag -w 29 -h 29 -o 15 -n 128 -f knob.png"
},
{ 0, 0, 0, 0, 0, 0 } };

int main(int argc, char* argv[])
{
	argc-=(argc>0); argv+=(argc>0); // skip program name argv[0] if present
	option::Stats   stats(usage, argc, argv);
	option::Option* options = new option::Option[stats.options_max];
	option::Option* buffer = new option::Option[stats.buffer_max];
	option::Parser  parse(usage, argc, argv, options, buffer);

	if (parse.error())
		return 1;

	if (argc == 0) {
		option::printUsage(std::cout, usage);
		return 0;
	}

	if (!options[ACTION].name)
		fprintf(stderr, "You must specify either display or generate action.");
	if (!options[GENERATORFILE].name)
		fprintf(stderr, "You must specify a path for a GLSL generator.");
	if (!options[WIDTH].name)
		fprintf(stderr, "You must specify width.");
	if (!options[HEIGHT].name)
		fprintf(stderr, "You must specify height.");
	if (!options[OVERSAMPLING].name)
		fprintf(stderr, "You must specify oversampling amount.");

	auto action = options[ACTION].type();

	if (action==kActionGenerate)
	{
		if (!options[NUM].name)
			fprintf(stderr, "You must specify number of subimages when generating.");
		if (!options[OUTFILE].name)
			fprintf(stderr, "You must specify PNG output file name when generating.");
	}

	auto knobfile = options[GENERATORFILE].arg;
	auto width = atoi(options[WIDTH].arg);
	auto height = atoi(options[HEIGHT].arg);
	auto oversampling = atoi(options[OVERSAMPLING].arg);

	// Init window
	ZGLWindow window;

	bool isFullscreen = false;
	uint32_t windowWidth = action==kActionGenerate ? width : width*oversampling, windowHeight = action==kActionGenerate ? height : height*oversampling;

	window.Open(windowWidth, windowHeight, isFullscreen);

	// Limit to 16:9 format.
	uint32_t viewportWidth = windowWidth;
	uint32_t viewportHeight = windowHeight;

	InitOpenGLExtensions();

	auto renderer = ZRenderer();
	renderer.nullFBO = ZFBO::NullFBO(viewportWidth, viewportHeight);

	ZTime timer;

	wglSwapIntervalEXT(1);

	ZMDisplay knobMat(knobfile);
	ZMOversample knobMatOversample(knobfile);
	ZMPassthrough passthrough;

	auto mainFBO = ZFBO(ZFBODescriptor(viewportWidth, viewportHeight));
	auto colorTex  = ZTexture(ZTextureDescriptor::Texture2D(viewportWidth, viewportHeight, kTextureFormatRGBA16));

//	uint8_t data[480*480*4];

	if (action==kActionDisplay)
	{
		while (window.isRunning)
		{
			curFrame++;

			window.HandleEvents();

			double time = timer.Get();

			renderer.Reset();
			renderer.pm() = IdentityMatrix();
			renderer.vm() = IdentityMatrix();
			renderer.mm() = IdentityMatrix();

			SetupFBO(renderer, &mainFBO, nullptr, &colorTex);

			renderer.Clear(kClearDepthBuffer | kClearColorBuffer);
			renderer.SetBlendMode(kBlendOff);
			renderer.SetCullingMode(kCullingShowFrontAndBack);
			renderer.SetZBufferMode(kZBufferOff);
			knobMat.v = (float)(fmod(time/30.0, 1.0));
			knobMat.Update();
			knobMat.Activate(renderer);
			ZPointCloud<ZPoint>::RenderSinglePoint();

			renderer.UnbindFBO();

			renderer.GetTextureUnit(0).BindTexture(&colorTex);
			passthrough.tex = 0;
			passthrough.Activate(renderer);

			ZPointCloud<ZPoint>::RenderSinglePoint();

			// Swap buffers
			window.SwapBuffers();
		}

	/*		if (curFrame==2)
			{
				glReadPixels(0, 0, 480, 480, GL_RGBA, GL_UNSIGNED_BYTE, data);
				stbi_write_png("out.png", 480, 480, 4, data + 479*480*4, -480*4);
			}*/


	}

	if (action==kActionGenerate)
	{
		auto num = atoi(options[NUM].arg);
		auto outfile = options[OUTFILE].arg;

		uint8_t* data = new uint8_t[width*height*4*num];

		for (int32_t i=0; i<num; i++)
		{
			curFrame++;

			window.HandleEvents();

			double time = timer.Get();

			renderer.Reset();
			renderer.pm() = IdentityMatrix();
			renderer.vm() = IdentityMatrix();
			renderer.mm() = IdentityMatrix();

			SetupFBO(renderer, &mainFBO, nullptr, &colorTex);

			renderer.Clear(kClearDepthBuffer | kClearColorBuffer);
			renderer.SetBlendMode(kBlendOff);
			renderer.SetCullingMode(kCullingShowFrontAndBack);
			renderer.SetZBufferMode(kZBufferOff);
			knobMatOversample.v = float(i)/float(num-1);
			knobMatOversample.oversample = float(oversampling);
			knobMatOversample.Update();
			knobMatOversample.Activate(renderer);
			ZPointCloud<ZPoint>::RenderSinglePoint();

			renderer.UnbindFBO();

			renderer.GetTextureUnit(0).BindTexture(&colorTex);
			passthrough.tex = 0;
			passthrough.Activate(renderer);

			ZPointCloud<ZPoint>::RenderSinglePoint();

			glReadPixels(0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, data+width*height*4*(num-i-1)); // write data backwards
			
			// Swap buffers
			window.SwapBuffers();
		}


		stbi_write_png("out.png", width, height*num, 4, data + (num*height-1)*width*4, -width*4);

	/*		if (curFrame==2)
			{
			}*/


	}

	ExitProcess(0); // This is neccesary to shut down song thread
} 