#include "pch.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

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
	"uniform float v;\n";

char* generatordisplay =
	"void main()\n"
	"{\n"
		"gl_FragColor = gamma(generate(pos, v));\n"
	"} \n";

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

class ZMGenerate : public ZMaterial 
{
public:
	ZMGenerate()
	{
		program = new ZProgramEx();
		program->vs = new ZShaderEx(kVertexShader);
		program->vs->sources.push_back(new ZStaticString(pointpassthrough));
		program->gs = new ZShaderEx(kGeometryShader);
		program->gs->sources.push_back(new ZStaticString(emitquad));
		program->fs = new ZShaderEx(kFragmentShader);
		program->fs->sources.push_back(new ZStaticString(generatorprelude));
		program->fs->sources.push_back(new ZWatchFile("shaders/knob.frag"));
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


int main(int argc, char* argv[])
{
	argc, argv;

	// Init window
	ZGLWindow window;

	bool isFullscreen = false;
	uint32_t windowWidth = 640, windowHeight = 480;

	window.Open(windowWidth, windowHeight, isFullscreen);

	// Limit to 16:9 format.
	uint32_t viewportWidth = zmin(windowWidth, windowHeight);
	uint32_t viewportHeight = zmin(windowHeight, windowWidth);

	InitOpenGLExtensions();

	auto renderer = ZRenderer();
	renderer.nullFBO = ZFBO::NullFBO(viewportWidth, viewportHeight);

	ZTime timer;

	wglSwapIntervalEXT(1);

	ZMGenerate knobMat;
	ZMPassthrough passthrough;

	auto mainFBO = ZFBO(ZFBODescriptor(viewportWidth, viewportHeight));
	auto colorTex  = ZTexture(ZTextureDescriptor::Texture2D(viewportWidth, viewportHeight, kTextureFormatRGBA16));

	uint8_t data[480*480*4];

	while (window.isRunning)
	{
		curFrame++;

		window.HandleEvents();

		double time = timer.Get();

/*		renderer.Reset();

		renderer.pm() = IdentityMatrix();
		renderer.vm() = IdentityMatrix();
		renderer.mm() = IdentityMatrix();

		renderer.GetTextureUnit(0).BindTexture(  final    , GetNearestSampler());
		//renderer.GetTextureUnit(0).BindTexture(ball->curPos, GetNearestSampler());
		passthrough.tex = 0;
		passthrough.Activate(renderer);

		renderer.UnbindFBO();

		renderer.SetBlendMode(kBlendOff);
		renderer.SetZBufferMode(kZBufferOff);

		uint32_t yOffset = (windowHeight - viewportHeight)/2;
		uint32_t xOffset = (windowWidth - viewportWidth)/2;

		glViewport(xOffset, yOffset, viewportWidth, viewportHeight);
		glScissor(xOffset, yOffset, viewportWidth, viewportHeight);

		ZPointCloud<ZPoint>::RenderSinglePoint();
		*/
		renderer.Reset();
		//renderer.pm() = PerspectiveMatrix(60.0f, float(viewportWidth)/float(viewportHeight), 1.0f, 180.0f);
		renderer.pm() = IdentityMatrix();
		renderer.vm() = IdentityMatrix();
		renderer.mm() = IdentityMatrix();

		SetupFBO(renderer, &mainFBO, nullptr, &colorTex);

		renderer.Clear(kClearDepthBuffer | kClearColorBuffer);
		renderer.SetBlendMode(kBlendOff);
		renderer.SetCullingMode(kCullingShowFrontAndBack);
		renderer.SetZBufferMode(kZBufferOff);
//		knobMat.iGlobalTime = (float)time;
		knobMat.Update();
		knobMat.Activate(renderer);
		ZPointCloud<ZPoint>::RenderSinglePoint();

		renderer.UnbindFBO();

		renderer.GetTextureUnit(0).BindTexture(&colorTex);
		//renderer.GetTextureUnit(0).BindTexture(ball->curPos, GetNearestSampler());
		passthrough.tex = 0;
		passthrough.Activate(renderer);

		ZPointCloud<ZPoint>::RenderSinglePoint();

		if (curFrame==2)
		{
			glReadPixels(0, 0, 480, 480, GL_RGBA, GL_UNSIGNED_BYTE, data);
			stbi_write_png("out.png", 480, 480, 4, data + 479*480*4, -480*4);
		}

		// Swap buffers
		window.SwapBuffers();
	}

	ExitProcess(0); // This is neccesary to shut down song thread
}