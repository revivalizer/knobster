#pragma once

class ZMaterial : public align16
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

// Class declarations
#include "shaderdefreset.h"
#undef MAT_BEGIN
#undef MAT_END
#undef UNI_MAT4
#undef UNI_INT
#undef UNI_FLOAT
#undef UNI_VEC2
#undef UNI_VEC3
#undef UNI_VEC4
#define MAT_BEGIN(NAME, PROGNAME) \
	class NAME : public ZMaterial \
	{ \
	public: \
		NAME(); \
\
		virtual void Update(); \
		virtual void Activate(ZRenderer& renderer);

#define UNI_MAT4(NAME)  ZMatrix NAME; ZUniLoc loc_##NAME;
#define UNI_INT(NAME)   int32_t NAME; ZUniLoc loc_##NAME;
#define UNI_FLOAT(NAME) float NAME;   ZUniLoc loc_##NAME;
#define UNI_VEC2(NAME)  ZVector NAME; ZUniLoc loc_##NAME;
#define UNI_VEC3(NAME)  ZVector NAME; ZUniLoc loc_##NAME;
#define UNI_VEC4(NAME)  ZVector NAME; ZUniLoc loc_##NAME;
#define MAT_END() };
#include "shaders.h"
