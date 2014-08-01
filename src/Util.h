#ifndef DF_UTIL
#define DF_UTIL
#include <memory>
#include <random>
#include <string>
#include <map>
#include <vector>
#include <stack>
#include <iostream>
#include <forward_list>
#include <unistd.h>

#ifdef WINDOWS
	#include <freetype/ft2build.h>
	#include <windows.h>
#else
	#include <freetype2/ft2build.h>
#endif
#include FT_FREETYPE_H
#include FT_GLYPH_H
#include <png.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <AL/al.h>
#include <AL/alc.h>

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

extern "C" {
#include <lua5.1/lua.h>
#include <lua5.1/lualib.h>
#include <lua5.1/lauxlib.h>
}
#include <btBulletDynamicsCommon.h>
#ifdef WINDOWS
	#include <BulletCollision/Gimpact/btGImpactCollisionAlgorithm.h>
#else
	#include <bullet/BulletCollision/Gimpact/btGImpactCollisionAlgorithm.h>
#endif

#include "Vec2d.h"
#include "Vec3d.h"
#define FPS 60
#define PI 3.14159265
#define DEGTORAD PI / 180.0
#define COLLISION_GRID_SCALE 64

std::string getPath();
void lua_insertpath(lua_State* ls);
void insertVec2d(lua_State* ls, std::string key, Vec2d v);
void insertVec3d(lua_State* ls, std::string key, Vec3d v);
void retrieveVec2d(lua_State* ls, std::string key, Vec2d& v);
void retrieveVec3d(lua_State* ls, std::string key, Vec3d& v);
float clamp(float val, float low, float high);

#endif
