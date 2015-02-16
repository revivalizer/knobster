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

	InitShaderSys();

	ZTime timer;

	wglSwapIntervalEXT(1);

	ZMKnob knobMat;
	ZPPPassthrough passthrough;

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
		knobMat.iGlobalTime = (float)time;
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

#pragma function(memset)
extern "C" void * __cdecl memset(void *pTarget, int value, size_t cbTarget) {
	char v = (char)value;

    _asm {
    push ecx
    push edi

    mov al, v
    mov ecx, cbTarget
    mov edi, pTarget
    rep stosb

    pop edi
    pop ecx
    }
    return pTarget;
}
    //void *  __cdecl memset(_Out_writes_bytes_all_(_Size) void * _Dst, _In_ int _Val, _In_ size_t _Size);
