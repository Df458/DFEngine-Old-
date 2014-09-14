#include "Component.h"
#include "Game.h"
#include "Tween.h"
#include "Model.h"

using namespace df;

GraphicsComponent::GraphicsComponent() {
	color = Vec3d(1, 1, 1);
	program = game->getAssetManager()->getProgram("default");
	model = game->getAssetManager()->getModel("default");
	texture = game->getAssetManager()->getTexture("default");
}

void GraphicsComponent::_runSub(float delta_time) {
    glm::mat4 model_transform = 
    glm::translate(glm::mat4(1.0f), glm::make_vec3(owner->getPosition()) * 10.0f) *
    glm::mat4_cast(glm::make_quat(owner->getQuaternionRotation())) *
    glm::scale(glm::mat4(1.0f), glm::make_vec3(owner->getScale()));
    glm::mat4 view_transform = game->getCameraView();
    glm::mat4 projection_transform = game->getCameraProjection();
    glm::vec4 blend_color = {color.x, color.y, color.z, alpha};

    drawModel(&model, texture, program, blend_color, model_transform, view_transform, projection_transform);
}

void TimerComponent::_runSub(float delta_time) {
	for(auto i = _timers.begin(); i != _timers.end(); ++i) {
		if(i->second.time < 0 || i->second.func.empty())
			continue;
		
		i->second.time -= delta_time / FPS;
		if(i->second.time <= 0) {
			i->second.time = -1;
			lua_State* ls = game->getState();
			//ls = luaL_newstate();
			//luaL_openlibs(ls);
			//luaL_openlib(ls, "game", lua_game_functions, 0);
				
			Entity* owner = game->getEntity(_owner_id);
				
			lua_newtable(ls);
			owner->insert(ls);
			lua_setglobal(ls, "this");
			game->insertData(ls);
			lua_insertpath(ls);
			luaL_loadfile(ls,(getPath() + "/data/scripts/" + i->second.func).c_str());
			if(lua_pcall(ls, 0, LUA_MULTRET, 0))
				std::cerr << lua_tostring(ls, -1) << "\n";
			lua_getglobal(ls, "this");
			owner->retrieve(ls);
			game->retrieveData(ls);
		}
	}
}

void TimerComponent::_insertDataSub(lua_State* ls) {
		lua_newtable(ls);
		for(auto i : _timers) {
			lua_pushinteger(ls, i.first);
			lua_newtable(ls);
			lua_pushnumber(ls, i.second.time);
			lua_setfield(ls, -2, "time");
			lua_pushstring(ls, i.second.func.c_str());
			lua_setfield(ls, -2, "script");
			lua_settable(ls, -3);
		}
		lua_setfield(ls, -2, "timers");
	}
void TimerComponent::_retrieveDataSub(lua_State* ls) {
		lua_getfield(ls, -1, "timers");
		lua_pushnil(ls);
		while(lua_next(ls, -2)) {
			lua_getfield(ls, -1, "time");
			lua_getfield(ls, -2, "script");
			_timers[lua_tointeger(ls, -4)].time = lua_tonumber(ls, -2);
			_timers[lua_tointeger(ls, -4)].func = lua_tostring(ls, -1);
			lua_pop(ls, 3);
		}
		lua_pop(ls, 1);
	}
