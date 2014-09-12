#ifndef DF_LUA_FUNCTIONS
#define DF_LUA_FUNCTIONS
#include "Util.h"

int lua_getGameFPS(lua_State* ls);
int lua_loadSound(lua_State* ls);
int lua_loadFont(lua_State* ls);
int lua_loadTexture(lua_State* ls);
int lua_loadShader(lua_State* ls);
int lua_loadModel(lua_State* ls);
int lua_createProgram(lua_State* ls);
int lua_clearTweens(lua_State* ls);
int lua_drawText(lua_State* ls);
int lua_generateID(lua_State* ls);
int lua_addComponent(lua_State* ls);
int lua_setScript(lua_State* ls);
int lua_playSound(lua_State* ls);
int lua_getTexture(lua_State* ls);
int lua_getSound(lua_State* ls);
int lua_getShader(lua_State* ls);
int lua_getProgram(lua_State* ls);
int lua_clearAll(lua_State* ls);
int lua_addTween(lua_State* ls);
int lua_getMouseData(lua_State* ls);
int lua_getRaycast(lua_State* ls);
int lua_getMouseOver(lua_State* ls);
int lua_getKeyboardData(lua_State* ls);
int lua_getObjectData(lua_State* ls);
int lua_updateObjectData(lua_State* ls);
int lua_addForce(lua_State* ls);
int lua_setActiveProgram(lua_State* ls);
int lua_foreach(lua_State* ls);
int lua_setUniformProgramData(lua_State* ls);

const luaL_reg lua_game_functions[] = {
	{"getFPS", lua_getGameFPS},
	{"generateID", lua_generateID},
	{"addComponent", lua_addComponent},
	{"addTween", lua_addTween},
	{"drawText", lua_drawText},
	{"getMouseOver", lua_getMouseOver},
	{"getRaycast", lua_getRaycast},
	{"setScript", lua_setScript},
	{"getObjectData", lua_getObjectData},
	{"updateObjectData", lua_updateObjectData},
	{"playSound", lua_playSound},
	{"getTexture", lua_getTexture},
	{"getSound", lua_getSound},
	{"getShader", lua_getShader},
	{"getProgram", lua_getProgram},
	{"loadTexture", lua_loadTexture},
	{"loadFont", lua_loadFont},
	{"loadSound", lua_loadSound},
	{"loadShader", lua_loadShader},
	{"loadModel", lua_loadModel},
	{"createProgram", lua_createProgram},
	{"addForce", lua_addForce},
	{"setUniformProgramData", lua_setUniformProgramData},
	{"setActiveProgram", lua_setActiveProgram},
	{"getKeyboardData", lua_getKeyboardData},
	{"clearAll", lua_clearAll},
	{"clearTweens", lua_clearTweens},
	{"foreach", lua_foreach},
	{0, 0}
};
#endif
