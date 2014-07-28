#include "Entity.h"
#include "Game.h"
using namespace df;

Entity::Entity(unsigned id) {
	_id = id;
	/*_state = luaL_newstate();
	luaL_openlibs(_state);
	luaL_openlib(_state, "game", lua_game_functions, 0);
	lua_insertpath(_state);*/
	_state = game->game_state;
	_is_alive = true;
	_to_destroy = false;
	
	_components["storage"] = new StorageComponent;
}

Entity::~Entity() {
	for(auto i : _components) {
		delete i.second;
	}
}

void Entity::destroy() {
	//std::cerr << _id << " is now dead.\n";
	game->_free_ids.push(_id);
	game->sys_graphics.remove(_id);
	game->sys_physics.remove(_id);
	game->sys_timer.remove(_id);
	_is_alive = false;
	_to_destroy = false;
	persist = false;
	type = 0;
	//_to_create = true;
}

void Entity::update(float dt) {
	if(_to_destroy) {
		destroy();
	}
	if(_to_create) {
		_to_create = false;
		if(!_create_script.empty()) {
			lua_newtable(_state);
			insert(_state);
			lua_setglobal(_state, "this");
			game->insertData(_state);
			luaL_loadfile(_state, (getPath() + "/data/scripts/" + _create_script).c_str());
			int errs = lua_pcall(_state, 0, LUA_MULTRET, 0);
			while(errs) {
				std::cerr << lua_tostring(_state, -1) << "\n";
				lua_pop(_state, 1);
				--errs;
			}
			lua_getglobal(_state, "this");
			retrieve(_state);
			game->retrieveData(_state);
		}
	}
	if(_update_script.empty())
		return;
	
	lua_pushnumber(_state, dt);
	lua_setglobal(_state, "delta_time");
	lua_newtable(_state);
	
	insert(_state);
	
	lua_setglobal(_state, "this");
	game->insertData(_state);
	luaL_loadfile(_state, (getPath() + "/data/scripts/" + _update_script).c_str());
	if(lua_pcall(_state, 0, LUA_MULTRET, 0))
		std::cerr << lua_tostring(_state, -1) << "\n";
	
	lua_getglobal(_state, "this");
	
	retrieve(_state);
	
	lua_pop(_state, 1);
	
	game->retrieveData(_state);
	if(_to_destroy) {
		destroy();
	}
}

void Entity::insert(lua_State* ls) {
	lua_pushboolean(ls, !_to_destroy);
	lua_setfield(ls, -2, "alive");
	lua_pushnumber(ls, _id);
	lua_setfield(ls, -2, "id");
	lua_pushinteger(ls, type);
	lua_setfield(ls, -2, "otype");
	lua_pushboolean(ls, persist);
	lua_setfield(ls, -2, "persist");
	for(auto i : _components) {
		i.second->insertData(ls);
	}
}

void Entity::retrieve(lua_State* ls) {
	for(auto i : _components) {
		i.second->retrieveData(ls);
	}
	lua_getfield(ls, -1, "persist");
	persist = lua_toboolean(ls, -1);
	lua_pop(ls, 1);
	lua_getfield(ls, -1, "otype");
	type = lua_tointeger(ls, -1);
	lua_pop(ls, 1);
	if(!_to_destroy) {
		lua_getfield(ls, -1, "alive");
		_to_destroy = !lua_toboolean(ls, -1);
		lua_pop(ls, 1);
	}
}
