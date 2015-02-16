#pragma once

class ZShaderEx : public ZShader
{
public:
	ZShaderEx(ZShaderType type);
	virtual ~ZShaderEx(void);

	zvector<ZSource*> sources;

	int32_t compileTime;
	int32_t checkTime;

	void Update();
};

