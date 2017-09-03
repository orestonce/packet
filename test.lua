packDef = {
	["C2S_PlayerLogin"] = 
	{
		{ type = "string", name="userName", maxLen="MAX_STRING_LENGTH", mark="用户名"},
		{ type = "string", name="password", maxLen="MAX_STRING_LENGTH", mark="密码"},
		{ type = "int", name="randomInt", mark="随机数"},
	}
}

fileDotH = io.open("rpcdata.h", "w")

print = function(str)
	fileDotH:write(str .. "\n")
end

print("#pragma once")
print('#include "localmacro.h"')
print('#include "packet.h"')
print('#include <stdexcept>')

function GenDecode(pDefine, prefixTabArg)
	
	print(prefixTabArg .. "{")
	prefixTab = prefixTabArg .. "\t"
	for _, item in pairs( pDefine ) do
		if item.type == "string" then
			print(prefixTab .. "\t{ // decode string : " .. item.name )
			print(prefixTab .. "\t\tint length = p.ReadInt();")
			print(prefixTab .. "\t\tif ( length < 0 || length >= " .. item.maxLen .. ") " )
			print(prefixTab .. '\t\t\tthrow std::runtime_error("string \'' .. item.name .. '\' is too long ");' )
			print(prefixTab .. "\t\tmemset(".. item.name .. ", 0, ".. item.maxLen .. ");" )
			print(prefixTab .. "\t\tp.ReadByteArray(".. item.name .. ", length);" )
			print(prefixTab .. "\t}")
		elseif item.type == "int" then
			print(prefixTab .. "\t" .. item.name .. " = p.ReadInt();" )
		else
			assert(false);
		end
	end
	print(prefixTabArg .. "}")
end

function GenEncode(pDefine, prefixTabArg)
	
	print(prefixTabArg .. "{")

	prefixTab = prefixTabArg .. "\t"
	for _, item in pairs( pDefine ) do
		if item.type == "string" then
			print(prefixTab .. "{ // encode string : " .. item.name )
			print(prefixTab .. "\tint length = strlen(".. item.name .. "); ")
			print(prefixTab .. "\tif ( length >= ".. item.maxLen .. ") ")
			print(prefixTab .. "\t\tlength = " .. item.maxLen .. " -1 ; ")
			print(prefixTab .. "\tp.WriteInt( length );")
			print(prefixTab .. "\tp.WriteByteArray( " .. item.name .. ", length );")
			print(prefixTab .. "}")
		elseif item.type == "int" then
			print(prefixTab .. "p.WriteInt(".. item.name ..");" )
		else
			assert(false)
		end
	end
	
	print(prefixTab .. "p.Flush();")
	print(prefixTabArg .. "}")
end

function GenDefine(pDefine, prefixTab)
	for _, item in pairs( pDefine ) do
		if item.type == "string" then
			print(prefixTab .. "char " .. item.name .. "[ " .. item.maxLen .. " ] ; // " .. item.mark )
		elseif item.type == "int" then
			print(prefixTab .. "int " .. item.name .. " ; // " .. item.mark )
		else
			assert(false)
		end
	end
end

for pName, pDefine in pairs( packDef ) do
	print("class ".. pName )
	print("{")
	print("public :")
	print("\tvoid Encode(CPacket& p)")
	GenEncode(pDefine, "\t")
	print("\tvoid Decode(CPacket& p)")
	GenDecode(pDefine, "\t")
	print("public :")
	GenDefine(pDefine, "\t")
	print("};")
end

if fileDotH then
	fileDotH:close()
end
