#pragma once

class ZProgramEx : public ZProgram
{
public:
	ZProgramEx(void);
	virtual ~ZProgramEx(void);

	int32_t linkTime;
	int32_t checkTime;

	ZShaderEx* vs;
	ZShaderEx* gs;
	ZShaderEx* fs;

	void Update();
};

