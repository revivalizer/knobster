local common = dofile("../64klibs/premake/common.lua")

solution "knobster"
	configurations { "Debug", "Release" }

	common.GenerateStandardSetup()

	includedirs { "../64klibs" }
	includedirs { "." } -- for config

	project "knobster"
		kind "WindowedApp"
		debugdir "" -- neccesary becaues default debug dir is the dir containing the proj file, which is no longer in the expected location due to 'location' command

		files "main.cpp"
		files { "pch.cpp", "pch.h" }
		files "config.h"
		files "shadersys/*"
		files "watchfile.*"

		buildoptions { "/wd4189" } -- 4189 needed fpr stb_image_write

		pchheader "pch.h"
		pchsource "pch.cpp"

		links { "base" }
		links { "opengl" }
		links { "render" }

		links { "OpenGL32", "glu32" }

		common.GenerateStandardConfigurations()

	include "../64klibs/base"
	include "../64klibs/opengl"
	include "../64klibs/render"
