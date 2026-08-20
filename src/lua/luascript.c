/*
 ******************************************************************************
 Project:      OWA EPANET
 Version:      2.3
 Module:       luascript.c
 Description:  manages a project's Lua scripting engine
 Authors:      see AUTHORS
 Copyright:    see AUTHORS
 License:      see LICENSE
 Last Updated: 08/19/2026
 ******************************************************************************
*/

#ifdef LUA_SCRIPTING
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "luatypes.h"
#include "luascript.h"
#include "luafuncs.h"
#include "luaevents.h"
#include "epanet2_2.h"
#include "epanet2_lua.h"

#define FMT86  "Lua script error while parsing: %s"
#define FMT87  "Lua script error: %s"

// Script errors are only reported when status reporting is enabled
static int statusReportingEnabled(EN_Project pr)
{
    double level = EN_NO_REPORT;

    if (EN_getoption(pr, EN_STATUS_REPORT, &level) != 0) return EN_FALSE;
    return (int)level != EN_NO_REPORT;
}

void luascript_setChanged(EN_Project pr)
{
    struct LuaEngine* lua = (struct LuaEngine *)EN_getlua(pr);
    if (lua != NULL) lua->changed = EN_TRUE;
}

int luascript_addScriptLine(EN_Project pr, char *line)
{
    size_t line_len = strlen(line);
    struct LuaEngine* lua = (struct LuaEngine *)EN_getlua(pr);

    if (lua == NULL)
    {
        return 311;
    }
    
    if (lua->script == NULL)
    {
        lua->script = malloc(line_len+1);
        if (lua->script == NULL)
        {
            return 101;
        }
        
        memcpy(lua->script, line, line_len);
        lua->script[line_len] = '\0';
    }
    else
    {
        size_t prev_len = strlen(lua->script);
        char *buffer = realloc(lua->script, prev_len + line_len + 1);
        if (buffer == NULL)
        {
            return 101;
        }

        memcpy(buffer + prev_len, line, line_len);
        buffer[line_len + prev_len] = '\0';
        lua->script = buffer;
    }

    return 0;
}

const char *luascript_getScript(EN_Project pr)
{
    struct LuaEngine* lua = (struct LuaEngine *)EN_getlua(pr);
    if (lua == NULL) return NULL;
    return lua->script;
}

int luascript_open(EN_Project pr)
{
    struct LuaEngine* lua = (struct LuaEngine*)calloc(1, sizeof(struct LuaEngine));
    EN_setlua(pr, lua);
    if (lua == NULL)
    {
        return 101;
    }

    lua->global_closure_ref = LUA_NOREF;

    lua->engine = luaL_newstate();
    if (lua->engine == NULL)
    {
        return 310;
    }

    luaL_openlibs(lua->engine);
    luafuncs_register(lua->engine, pr);

    return 0;
}

static int run_lua_script(EN_Project pr, int *changed)
{
    if (changed != NULL) *changed = EN_FALSE;
    struct LuaEngine* lua = (struct LuaEngine *)EN_getlua(pr);
    if (lua == NULL || lua->engine == NULL || lua->global_closure_ref == LUA_NOREF)
    {
        return 0;
    }

    lua->changed = EN_FALSE;

    lua_rawgeti(lua->engine, LUA_REGISTRYINDEX, lua->global_closure_ref);
    if (lua_pcall(lua->engine, 0, 0, 0) != LUA_OK)
    {
        if (statusReportingEnabled(pr))
        {
            char msg[EN_MAXMSG + 1];
            snprintf(msg, EN_MAXMSG, FMT87, lua_tostring(lua->engine, -1));
            EN_writeline(pr, msg);
        }
        lua_pop(lua->engine, 1);
        return 313;
    }

    if (changed != NULL) *changed = lua->changed;
    return 0;
}

int luascript_parseScript(EN_Project pr)
{
    struct LuaEngine* lua = (struct LuaEngine *)EN_getlua(pr);
    if (lua == NULL || lua->engine == NULL)
    {
        return 311;
    }

    if (lua->script == NULL)
    {
        return 0;
    }

    if (luaL_loadstring(lua->engine, lua->script) != LUA_OK)
    {
        if (statusReportingEnabled(pr))
        {
            char msg[EN_MAXMSG + 1];
            snprintf(msg, EN_MAXMSG, FMT86, lua_tostring(lua->engine, -1));
            EN_writeline(pr, msg);
        }
        lua_pop(lua->engine, 1);
        return 312;
    }
    lua->global_closure_ref = luaL_ref(lua->engine, LUA_REGISTRYINDEX);

    // The load-time evaluation runs against a network that has not been
    // solved yet, so an error in it is reported but left to be raised by
    // the first pass of the run proper
    run_lua_script(pr, NULL);
    lua->changed = EN_FALSE;

    return 0;
}

int luascript_runIteration(EN_Project pr, int *changed)
{
    if (changed != NULL) *changed = EN_FALSE;
    struct LuaEngine* lua = (struct LuaEngine *)EN_getlua(pr);
    if (lua == NULL || lua->engine == NULL) return 0;

    lua_getglobal(lua->engine, "on_hydraulic_step");
    int hasHandler = lua_isfunction(lua->engine, -1);
    lua_pop(lua->engine, 1);

    if (hasHandler)
    {
        return luascript_onEvent(pr, LUA_EVENT_HYDRAULIC_STEP, changed);
    }
    return run_lua_script(pr, changed);
}

void luascript_close(EN_Project pr)
{
    struct LuaEngine* lua = (struct LuaEngine *)EN_getlua(pr);    
    if (lua != NULL)
    {
        if (lua->engine != NULL)
        {
            if (lua->global_closure_ref != LUA_NOREF)
            {
                luaL_unref(lua->engine, LUA_REGISTRYINDEX,
                           lua->global_closure_ref);
            }
            lua_close(lua->engine);
        }
        free(lua->script);
        free(lua);
        lua = NULL;
    }
}
#endif // LUA_SCRIPTING