/**
* Copyright (C) 2017 chai(neonum)
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is furnished
* to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in all
* copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
* INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
* PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
* HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
* OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
* SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#ifndef __LUAX_H
#define __LUAX_H
#include "libs/lua51/lua.h"
#include "libs/lua51/lauxlib.h"
#include "libs/lua51/lualib.h"

// only for lua 5.1
#if LUA_VERSION_NUM == 501 

#define luax_newstate luaL_newstate 
#define luax_openlibs luaL_openlibs
#define luax_loadbuffer luaL_loadbuffer
#define luax_pcall lua_pcall 
#define luax_setglobal lua_setglobal
#define luax_pop lua_pop
#define luax_newtable lua_newtable
/**
* Get number of args 
*/
#define luax_gettop lua_gettop
/**
* Check userdata type.
*/
#define luax_checktype luaL_checkudata
#define luax_checknumber luaL_checknumber
#define luax_checkstirng luaL_checkstring

#define luax_optnumber luaL_optnumber
/**
* 
*/
#define luax_error luaL_error
/**
* Push value on the top of stack.
*/
#define luax_pushnumber  lua_pushnumber
#define luax_pushstring  lua_pushstring 
#define luax_pushinteger lua_pushinteger
#define luax_pushboolean lua_pushboolean

#define luax_rawseti lua_rawseti
/**
* Set field
*/
#define luax_setfield lua_setfield
#define luax_setfield_(T, L, k, v)\
   do { lua_push##T(L, v); lua_setfield(L, -2, k); } while (0)

#define luax_setfield_number(L, k, v) luax_setfield_(number, L, k, v)
#define luax_setfield_string(L, k, v) luax_setfield_(string, L, k, v)
#define luax_setfield_bool(L, k, v)   luax_setfield_(boolean, L, k, v)
#define luax_setfield_udata(L, k, v)  luax_setfield_(lightuserdata, L, k, v)
#define luax_setfield_cfunc(L, k, v)  luax_setfield_(cfunction, L, k, v)
#define luax_setfield_fstring(L, k, ...)\
   do { lua_pushfstring(L, __VA_ARGS__); lua_setfield(L, -2, k); } while (0)

/**
* 
*/
#define luax_optboolean(L, i, x)\
  (!lua_isnoneornil(L, i) ? lua_toboolean(L, i) : (x))

#define luax_optudata(L, i, name, x)\
  (!lua_isnoneornil(L, i) ? luaL_checkudata(L, i, name) : (x))

inline int luax_newlib(lua_State* L, const luaL_Reg* f)
{
    lua_createtable(L, 0, sizeof(f));
    for (; f && f->name; f++)
    {
        lua_pushcfunction(L, f->func);
        lua_setfield(L, -2, f->name);
    }
    return 1; // leave lib table on top of stack
}

/**
* Register a userdefined lua type with given type name.
*/
inline void luax_newtype(lua_State* L, const char* tname, const luaL_Reg* f)
{
    luaL_newmetatable(L, tname);

    // m.__index = m
    lua_pushvalue(L, -1);
    lua_setfield(L, -2, "__index");

    if (f != 0)
        luaL_register(L, 0, f);

    lua_pop(L, 1); // Pops metatable.
}

/**
* Instance of a type.
*/
inline void* luax_newinstance(lua_State* L, const char* tname, int size)
{
    void* p = lua_newuserdata(L, size);
    luaL_getmetatable(L, tname);
    lua_setmetatable(L, -2);
    return p;
}

/**
* require a module
*/
inline int luax_require(lua_State* L, const char* mname, lua_CFunction openf, int glb) 
{
    openf(L); 
    if (glb)
    {
        lua_setglobal(L, mname);
        return 0; 
    }
    return 1;
}

/**
* Return 1 if match. 
*/
inline int luax_istype(lua_State* L, int idx, const char* tname)
{
    if (lua_getmetatable(L, idx))
    {
        /* get metatable called tname */
        lua_getfield(L, LUA_REGISTRYINDEX, tname); 
        if (lua_rawequal(L, -1, -2))
        {
            lua_pop(L, 2); // pop both metatables
            return 1; 
        }
        lua_pop(L, 2);
        return 0; 
    }
    /* value at idx has no metatable */
    return 0;
}

/**
* To userdata. 
*/
inline void* luax_toudata(lua_State* L, int idx)
{
    void* p = lua_touserdata(L, idx); 
    if (p == NULL)
        luaL_typerror(L, idx, "userdata"); // if p is not userdata
    return p; 
}

#endif

#endif