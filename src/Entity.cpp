#include "Entity.h"
#include "Game.h"
#include "Component.h"
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

Component* Entity::getComponent(std::string cmp_id) {
	if (_components.find(cmp_id) != _components.end())
		return _components[cmp_id];
	return NULL;
}

void Entity::setComponent(std::string cmp_id, Component* cmp) {
	cmp->_owner_id = _id;
	_components[cmp_id] = cmp;
}

bool Entity::getAlive() {
	return _is_alive;
}

void Entity::setAlive(bool alive) {
	_to_destroy = !alive;
}

bool Entity::getDestroyed() {
	return _to_destroy;
}

bool Entity::getPersist() {
	return persist;
}

unsigned Entity::getId() {
	return _id;
}

unsigned Entity::getType() {
	return type;
}

void Entity::reset() {
	_is_alive = true;
	_to_create = true;
	for(auto i : _components) {
		delete i.second;
	}
	_components.clear();
	_components["storage"] = new StorageComponent;
	_create_script.clear();
	_update_script.clear();
	_collide_script.clear();
	_draw_script.clear();
}

std::string Entity::getUpScr() {
	return _update_script;
}

void Entity::setUpScr(std::string scr) {
	_update_script = scr;
}

std::string Entity::getColScr() {
	return _collide_script;
}

void Entity::setColScr(std::string scr) {
	_collide_script = scr;
}

void Entity::setCreScr(std::string scr) {
	_create_script = scr;
}

std::string Entity::getCreScr() {
	return _create_script;
}

void Entity::setDrawScr(std::string scr) {
	_draw_script = scr;
}

std::string Entity::getDrawScr() {
	return _draw_script;
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
