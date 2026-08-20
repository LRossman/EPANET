/*
 ******************************************************************************
 Project:      OWA EPANET
 Version:      2.3
 Module:       luafuncs.h
 Description:  prototype for registering the toolkit's Lua-facing API
 Authors:      see AUTHORS
 Copyright:    see AUTHORS
 License:      see LICENSE
 Last Updated: 08/19/2026
 ******************************************************************************
*/

#ifdef LUA_SCRIPTING
#ifndef LUA_FUNCS_H
#define LUA_FUNCS_H

#include "minilua.h"
typedef struct Project* EN_Project;

void luafuncs_register(lua_State *L, EN_Project pr);

#endif // LUA_FUNCS_H
#endif // LUA_SCRIPTING
