/*
 ******************************************************************************
 Project:      OWA EPANET
 Version:      2.3
 Module:       luaevents.c
 Description:  calls the handler functions defined by a project's Lua script
 Authors:      see AUTHORS
 Copyright:    see AUTHORS
 License:      see LICENSE
 Last Updated: 08/19/2026
 ******************************************************************************
*/

#ifdef LUA_SCRIPTING
#include "epanet2_2.h"
#include "epanet2_lua.h"
#include "luatypes.h"
#include "luaevents.h"
 
#define FMT88  "Lua script error in %s: %s"

static const char *event_name[LUA_EVENT_MAX] = {
    "on_open",
    "on_close",
    "on_hydraulics_solved",
    "on_hydraulic_step"
};

// Script errors are only reported when status reporting is enabled
static int statusReportingEnabled(EN_Project pr)
{
    double level = EN_NO_REPORT;

    if (EN_getoption(pr, EN_STATUS_REPORT, &level) != 0) return EN_FALSE;
    return (int)level != EN_NO_REPORT;
}

int luascript_onEvent(EN_Project pr, LuaEvent event, int *changed)
{
    if (changed != NULL) *changed = EN_FALSE;

    struct LuaEngine* lua = (struct LuaEngine *)EN_getlua(pr);
    if (lua == NULL || lua->engine == NULL || lua->script == NULL)
    {
        return 0;
    }

    lua->changed = EN_FALSE;

    lua_getglobal(lua->engine, event_name[event]);
    int event_defined = lua_isfunction(lua->engine, -1);
    if (!event_defined)
    {
        lua_pop(lua->engine, 1);
        return 0;
    }

    lua->timed_event = (event == LUA_EVENT_HYDRAULIC_STEP ||
                            event == LUA_EVENT_HYDRAULICS_SOLVED);
    int execution_result = lua_pcall(lua->engine, 0, 0, 0);
    lua->timed_event = EN_FALSE;

    if (execution_result != LUA_OK)
    {
        if (statusReportingEnabled(pr))
        {
            char msg[EN_MAXMSG + 1];
            snprintf(msg, EN_MAXMSG, FMT88, event_name[event],
                lua_tostring(lua->engine, -1));
            EN_writeline(pr, msg);
        }
        lua_pop(lua->engine, 1);
        return 313;
    }

    if (changed != NULL) *changed = lua->changed;
    return 0;
}
#endif // LUA_SCRIPTING
