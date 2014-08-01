#include "Util.h"

std::string getPath(){
	char buf[1024];
#ifdef WINDOWS
    GetModuleFileName(NULL, buf, 1024);
    std::string::size_type pos = std::string(buf).find_last_of("\\/");
    return std::string(buf).substr(0, pos);
#elif __GNUC__
    ssize_t len = ::readlink("/proc/self/exe", buf, sizeof(buf)-1);
    if (len != -1) {
	buf[len] = '\0';
    std::string s = std::string(buf);
    return s.erase(s.rfind("/"));
    }else{
		std::cerr << "Error: Could not determine working directory\n";
		return "";
    }
#endif
}

void lua_insertpath(lua_State* ls) {
	lua_getglobal(ls, "package");
    lua_getfield(ls, -1, "path");
    std::string path = lua_tostring(ls, -1);
    path = getPath() + "/data/scripts/?.lua;" + path;
    lua_pop(ls, 1);
    lua_pushstring(ls, path.c_str());
    lua_setfield(ls, -2, "path");
    lua_pop(ls, 1);
}

void insertVec2d(lua_State* ls, std::string key, Vec2d v) {
	lua_newtable(ls);
	lua_pushnumber(ls, v.x);
	lua_setfield(ls, -2, "x");
	lua_pushnumber(ls, v.y);
	lua_setfield(ls, -2, "y");
	
	lua_setfield(ls, -2, key.c_str());
}

void insertVec3d(lua_State* ls, std::string key, Vec3d v) {
	lua_newtable(ls);
	lua_pushnumber(ls, v.x);
	lua_setfield(ls, -2, "x");
	lua_pushnumber(ls, v.y);
	lua_setfield(ls, -2, "y");
	lua_pushnumber(ls, v.z);
	lua_setfield(ls, -2, "z");
	
	lua_setfield(ls, -2, key.c_str());
}

void retrieveVec2d(lua_State* ls, std::string key, Vec2d& v) {
	lua_getfield(ls, -1, key.c_str());
	lua_getfield(ls, -1, "x");
	v.x = lua_tonumber(ls, -1);
	lua_getfield(ls, -2, "y");
	v.y = lua_tonumber(ls, -1);
	lua_pop(ls, 3);
}

void retrieveVec3d(lua_State* ls, std::string key, Vec3d& v) {
	lua_getfield(ls, -1, key.c_str());
	lua_getfield(ls, -1, "x");
	v.x = lua_tonumber(ls, -1);
	lua_getfield(ls, -2, "y");
	v.y = lua_tonumber(ls, -1);
	lua_getfield(ls, -3, "z");
	v.z = lua_tonumber(ls, -1);
	lua_pop(ls, 4);
}

float clamp(float val, float low, float high) {
	if(val < low)
		val = low;
	else if(val > high)
		val = high;
	return val;
}
