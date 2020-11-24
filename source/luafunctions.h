#ifndef LUAFUNC_H
#define LUAFUNC_H

extern "C" {
	#include "lua.h"
	#include "lualib.h"
	#include "lauxlib.h"
}

namespace LuaFUNC
{

	double get_numberFromTable(lua_State* lvm, const char* variable);

	void get_tableFromTable(lua_State* lvm, const char* table);

}

#endif
