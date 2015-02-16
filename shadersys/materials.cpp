#include "pch.h"

// Constructors
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
	NAME::NAME() \
	{ \
		programs.find(PROGNAME, &program);
#define UNI_MAT4(NAME)  NAME = IdentityMatrix(); loc_##NAME = -1;
#define UNI_INT(NAME)   NAME = -1;               loc_##NAME = -1;
#define UNI_FLOAT(NAME) NAME = -1.f;             loc_##NAME = -1;
#define UNI_VEC2(NAME)  NAME = ZVector(0.f);     loc_##NAME = -1;
#define UNI_VEC3(NAME)  NAME = ZVector(0.f);     loc_##NAME = -1;
#define UNI_VEC4(NAME)  NAME = ZVector(0.f);     loc_##NAME = -1;
#define MAT_END() }
#include "shaders.h"

// Update
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
	void NAME::Update() \
	{ \
		if (checkTime==curFrame) \
			return;\
\
		checkTime = curFrame; \
\
		program->Update();\
\
		if (updateTime < program->linkTime) \
		{ \
			updateTime = curFrame;
#define UNI_MAT4(NAME)  loc_##NAME = ZUniform::GetLoc(*program, #NAME);
#define UNI_INT(NAME)   loc_##NAME = ZUniform::GetLoc(*program, #NAME);
#define UNI_FLOAT(NAME) loc_##NAME = ZUniform::GetLoc(*program, #NAME);
#define UNI_VEC2(NAME)  loc_##NAME = ZUniform::GetLoc(*program, #NAME);
#define UNI_VEC3(NAME)  loc_##NAME = ZUniform::GetLoc(*program, #NAME);
#define UNI_VEC4(NAME)  loc_##NAME = ZUniform::GetLoc(*program, #NAME);
#define MAT_END() } }
#include "shaders.h"

// Activate
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
	void NAME::Activate(ZRenderer& renderer) \
	{ \
		Update(); \
		renderer.UseProgram(*program);\
		program->SetCommonLocations(renderer);
#define UNI_MAT4(NAME)  ZUniform::SetMat4(loc_##NAME, NAME);
#define UNI_INT(NAME)   ZUniform::SetInt(loc_##NAME, NAME);
#define UNI_FLOAT(NAME) ZUniform::SetFloat(loc_##NAME, NAME);
#define UNI_VEC2(NAME)  ZUniform::SetVec2(loc_##NAME, NAME);
#define UNI_VEC3(NAME)  ZUniform::SetVec3(loc_##NAME, NAME);
#define UNI_VEC4(NAME)  ZUniform::SetVec4(loc_##NAME, NAME);
#define MAT_END() } 
#include "shaders.h"

