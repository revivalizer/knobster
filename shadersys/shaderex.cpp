#include "pch.h"

ZShaderEx::ZShaderEx(ZShaderType type) : ZShader(type)
	, compileTime(-1)
	, checkTime(-1)
{
}


ZShaderEx::~ZShaderEx(void)
{
}

void ZShaderEx::Update()
{
	if (checkTime==curFrame)
		return;

	checkTime = curFrame;

	bool mustCompile = false;

	for (uint32_t i=0; i<sources.size(); i++)
	{
		sources[i]->Update();

		if (compileTime < sources[i]->readTime)
			mustCompile = true;
	}

	if (mustCompile)
	{
		compileTime = curFrame;

		// Build string array
		const char* strings[16];

		for (uint32_t i=0; i<sources.size(); i++)
			strings[i] = sources[i]->data;

		Compile(strings, sources.size());

		if (!DidCompile())
			OutputDebugString(GetCompileErrors());
	}
}
