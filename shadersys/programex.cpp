#include "pch.h"

ZProgramEx::ZProgramEx(void) : ZProgram()
	, vs(nullptr)
	, gs(nullptr)
	, fs(nullptr)
	, checkTime(-1)
	, linkTime(-1)
{
}


ZProgramEx::~ZProgramEx(void)
{
}

void ZProgramEx::Update()
{
	if (checkTime==curFrame)
		return;

	checkTime = curFrame;

	bool mustLink = false;

	vs->Update();
	mustLink |= linkTime < vs->compileTime;

	if (gs)
	{
		gs->Update();
		mustLink |= linkTime < gs->compileTime;
	}

	fs->Update();
	mustLink |= linkTime < fs->compileTime;

	if (mustLink)
	{
		linkTime = curFrame;

		if (gs)
			Link(*vs, *gs, *fs);
		else
			Link(*vs, *fs);

		if (!DidLink())
			OutputDebugString(GetLinkErrors());
	}
}
