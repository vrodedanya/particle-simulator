#include "luafunctions.h"
#include <stdexcept>

double LuaFUNC::get_numberFromTable(lua_State* lvm, const char* variable)
{
	double result = 0;
	lua_getfield(lvm, -1, variable);
	if (lua_isnumber(lvm, -1))
	{
		result = (double) lua_tonumber(lvm, -1);
		lua_pop(lvm, 1);
		return result;
	}
	else
	{
		throw std::runtime_error("[Lua]: Variable isn't number");
	}
}

void LuaFUNC::get_tableFromTable(lua_State* lvm, const char* table)
{
	lua_getfield(lvm, -1, table);
	if (!lua_istable(lvm, -1))
	{
		throw std::runtime_error("[Lua]: Variable isn't table");
	}
}
