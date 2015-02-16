#include "pch.h"
#include "embeddableshaders.h"

zhashmap<const char*, const char*> paths(101);
zhashmap<const char*, ZWatchFile*> files(101);
zhashmap<const char*, ZShaderEx*>  shaders(101);

zhashmap<const char*, ZProgramEx*> programs(101);

char* concat(const char* str1, const char* str2)
{
	uint32_t len = zstrlen(str1) + zstrlen(str2);
	char* s = new char[len+1];
	char* t = s;

	while (*str1)
		*t++ = *str1++;

	while (*str2)
		*t++ = *str2++;

	*t = '\0';
	return s;
}

void InitShaderSys()
{
	// Setup paths, file, shaders, programs
	// .. paths (reference from short path to actual path with a preprended string)
	#include "shadersys/shaderdefreset.h"
	#undef SHADER
	#define SHADER(PATH, TYPE) paths.insert(PATH, concat("shaders/", PATH));
	#include "shaders.h"

	#ifndef STANDALONE
		// .. files - ZWatchFiles for files
		#include "shadersys/shaderdefreset.h"
		#undef SHADER
		#define SHADER(PATH, TYPE) {const char* expath; paths.find(PATH, &expath); files.insert(PATH, new ZWatchFile(expath)); }
		#include "shaders.h"
	#else
		for (uint32_t i=0; i<g_embeddableShaderCount; i++)
		{
			auto file = new ZWatchFile(nullptr);
			files.insert(g_embeddableShaderPaths[i], file);
			file->data = g_embeddableShaders[i];
		}
	#endif

	// .. shaders
	#include "shadersys/shaderdefreset.h"
	#undef SHADER
	#define SHADER(PATH, TYPE) { ZShaderEx* shader = new ZShaderEx(TYPE); ZWatchFile* file; files.find(PATH, &file); shader->sources.push_back(file); shaders.insert(PATH, shader); }
	#include "shaders.h"

	// .. programs
	#include "shadersys/shaderdefreset.h"
	#undef P2
	#undef P3
	#define P2(name, vsname, fsname)         { ZProgramEx* program = new ZProgramEx(); ZShaderEx* vs; shaders.find(vsname, &vs); ZShaderEx* fs; shaders.find(fsname, &fs); program->vs = vs; program->fs = fs; programs.insert(name, program); }
	#define P3(name, vsname, gsname, fsname) { ZProgramEx* program = new ZProgramEx(); ZShaderEx* vs; shaders.find(vsname, &vs); ZShaderEx* gs; shaders.find(gsname, &gs); ZShaderEx* fs; shaders.find(fsname, &fs); program->vs = vs; program->gs = gs; program->fs = fs; programs.insert(name, program); }
	#include "shaders.h"
}
