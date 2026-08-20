/*
 ******************************************************************************
 Project:      OWA EPANET
 Version:      2.3
 Module:       luascript.h
 Description:  prototypes of the Lua scripting engine's interface to the toolkit
 Authors:      see AUTHORS
 Copyright:    see AUTHORS
 License:      see LICENSE
 Last Updated: 08/19/2026
 ******************************************************************************
*/

#ifndef LUASCRIPT_H
#define LUASCRIPT_H
#ifdef LUA_SCRIPTING

typedef struct Project* EN_Project;

int luascript_open(EN_Project pr);
void luascript_close(EN_Project pr);
int luascript_runIteration(EN_Project pr, int *changed);

int luascript_addScriptLine(EN_Project pr, char *line);
const char *luascript_getScript(EN_Project pr);
int luascript_parseScript(EN_Project pr);
void luascript_setChanged(EN_Project pr);

#endif // LUA_SCRIPTING
#endif // LUASCRIPT_H