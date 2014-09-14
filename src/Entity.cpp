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
	cmp->owner = this;
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

std::string Entity::getEnterScr() {
	return enter_script;
}

void Entity::setEnterScr(std::string scr) {
	enter_script = scr;
}

std::string Entity::getColScr() {
	return _collide_script;
}

void Entity::setColScr(std::string scr) {
	_collide_script = scr;
}

std::string Entity::getLeaveScr() {
	return leave_script;
}

void Entity::setLeaveScr(std::string scr) {
	leave_script = scr;
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
	lua_newtable(ls);
	lua_pushnumber(ls, position[0]);
	lua_setfield(ls, -2, "x");
	lua_pushnumber(ls, position[1]);
	lua_setfield(ls, -2, "y");
	lua_pushnumber(ls, position[2]);
	lua_setfield(ls, -2, "z");
	lua_setfield(ls, -2, "position");
	lua_newtable(ls);
	lua_pushnumber(ls, rotation[0]);
	lua_setfield(ls, -2, "w");
	lua_pushnumber(ls, rotation[1]);
	lua_setfield(ls, -2, "x");
	lua_pushnumber(ls, rotation[2]);
	lua_setfield(ls, -2, "y");
	lua_pushnumber(ls, rotation[3]);
	lua_setfield(ls, -2, "z");
	lua_setfield(ls, -2, "rotation");
	lua_newtable(ls);
	lua_pushnumber(ls, scale[0]);
	lua_setfield(ls, -2, "x");
	lua_pushnumber(ls, scale[1]);
	lua_setfield(ls, -2, "y");
	lua_pushnumber(ls, scale[2]);
	lua_setfield(ls, -2, "z");
	lua_setfield(ls, -2, "scale");

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

	lua_getfield(ls, -1, "position");
	lua_getfield(ls, -1, "x");
	position[0] = lua_tonumber(ls, -1);
	lua_getfield(ls, -2, "y");
	position[1] = lua_tonumber(ls, -1);
	lua_getfield(ls, -3, "z");
	position[2] = lua_tonumber(ls, -1);
	lua_pop(ls, 4);

	lua_getfield(ls, -1, "rotation");
	lua_getfield(ls, -1, "w");
	scale[0] = lua_tonumber(ls, -1);
	lua_getfield(ls, -2, "x");
	scale[1] = lua_tonumber(ls, -1);
	lua_getfield(ls, -3, "y");
	scale[2] = lua_tonumber(ls, -1);
	lua_getfield(ls, -4, "z");
	scale[3] = lua_tonumber(ls, -1);
	lua_pop(ls, 5);

	lua_getfield(ls, -1, "scale");
	lua_getfield(ls, -1, "x");
	scale[0] = lua_tonumber(ls, -1);
	lua_getfield(ls, -2, "y");
	scale[1] = lua_tonumber(ls, -1);
	lua_getfield(ls, -3, "z");
	scale[2] = lua_tonumber(ls, -1);
	lua_pop(ls, 4);

	if(!_to_destroy) {
		lua_getfield(ls, -1, "alive");
		_to_destroy = !lua_toboolean(ls, -1);
		lua_pop(ls, 1);
	}
}

const float* Entity::getPosition() {
    return position;
}

void Entity::setPosition(glm::vec3 n_position) {
    position[0] = n_position[0];
    position[1] = n_position[1];
    position[2] = n_position[2];
}

const float* Entity::getQuaternionRotation() {
    return rotation;
}

void Entity::setQuaternionRotation(glm::quat n_rotation) {
    rotation[0] = n_rotation[0];
    rotation[1] = n_rotation[1];
    rotation[2] = n_rotation[2];
    rotation[3] = n_rotation[3];
}

const float* Entity::getEulerRotation() {
    return glm::value_ptr(glm::eulerAngles(glm::quat(rotation[0], rotation[1], rotation[2], rotation[3])));
}

void Entity::setEulerRotation(glm::vec3 n_rotation) {
    glm::quat nq_rotation(n_rotation);
    rotation[0] = nq_rotation[0];
    rotation[1] = nq_rotation[1];
    rotation[2] = nq_rotation[2];
    rotation[3] = nq_rotation[3];
}

const float* Entity::getScale() {
    return scale;
}

void Entity::setScale(glm::vec3 n_scale) {
    scale[0] = n_scale[0];
    scale[1] = n_scale[1];
    scale[2] = n_scale[2];
}
