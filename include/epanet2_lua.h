/*
 ******************************************************************************
 Project:      OWA EPANET
 Version:      2.3
 Module:       epanet2_lua.h
 Description:  Lua scripting function declaration
 Authors:      see AUTHORS
 Copyright:    see AUTHORS
 License:      see LICENSE
 Last Updated: 08/19/2026
 ******************************************************************************
 */

#ifdef LUA_SCRIPTING

#ifndef EPANET2_LUA_H
#define EPANET2_LUA_H

#ifdef _WIN32
  #define STDCALL __stdcall
  #ifdef epanet2_EXPORTS
    #define LUADLLEXPORT __declspec(dllexport)
  #else
    #define LUADLLEXPORT __declspec(dllimport)
  #endif
#else
  #define STDCALL
  #define LUADLLEXPORT __attribute__((visibility("default")))
#endif

#if defined(__cplusplus)
extern "C" {
#endif

// The EPANET Project wrapper object
typedef struct Project *EN_Project;

// Pointer to a Lua engine struct
typedef struct LuaEngine *LuaEnginePtr;

LUADLLEXPORT void* STDCALL EN_getlua(EN_Project ph);
LUADLLEXPORT void STDCALL EN_setlua(EN_Project ph, LuaEnginePtr lua);

#if defined(__cplusplus)
}
#endif

#endif //EPANET2_LUA_H

#endif // LUA_SCRIPTING