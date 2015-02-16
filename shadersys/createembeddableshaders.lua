function getfiledata(filename)
	local f = assert(io.open(filename, "r"))
    local t = f:read("*all")
    f:close()

	return t
end

function out(str)
	outfile:write(str.."\n")
end

outfile = io.open("embeddableshaders.cpp", "w+")
	out("#include \"pch.h\"\n")

	local shaderfiledata = getfiledata("../shaders.h")
	--print(shaderfile)
	--local m = string.match("SHADER(\"abs", 'SHADER%(\"(%a+)')
	--local m = string.match(shaderfile, 'SHADER%(\"([%a%.]+)')

	local count = 0

	local pattern = 'SHADER%(%s*\"([%w%p]+)\"%s*,%s*(%w+)%s*%)'

	-- Output shaders paths
	out("char *g_embeddableShaderPaths[] = {")
	for name,type in string.gfind(shaderfiledata, pattern) do
		print(name)
		out("\t\""..name.."\",");
		count = count + 1
	end
	out("};\n");

	-- Output shader types
	out("uint32_t g_embeddableShaderTypes[] = {")
	--for type in string.gfind(shaderfiledata, 'SHADER%(%s*\"([%a%p]+)\"%s*,%s*(%w+)%s%)') do
	for name,type in string.gfind(shaderfiledata, pattern) do
		out("\t"..type..",")
	end
	out("};\n");

	-- Output shaders
	out("char *g_embeddableShaders[] = {")
	for name,type in string.gfind(shaderfiledata, pattern) do
		local shader = getfiledata("../shaders/"..name)
		for line in shader:gmatch("[^\r\n]+") do
			line = line:gsub("\"", "\\\"")
			out("\t\""..line.."\\n\"")
		end
		out(",")
	end
	out("};\n");

	-- Output counts
	out("uint32_t g_embeddableShaderCount = "..count..";")

outfile:close()

outfile = io.open("embeddableshaders.h", "w+")
	out("extern char *g_embeddableShaderPaths[];")
	out("extern uint32_t g_embeddableShaderTypes[];")
	out("extern char *g_embeddableShaders[];")
	out("extern uint32_t g_embeddableShaderCount;")
outfile:close()
