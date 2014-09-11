#include "AssetManager.h"
#include "LuaFunctions.h"
#include "Entity.h"
#include "Component.h"
//#include "Tween.h"
#include "Game.h"
#include "Window.h"

using namespace df;

AssetManager* Game::getAssetManager() {
    return &asset_library;
}

void Game::setMousePosition(Vec2d mp) {
    _mouse_position = mp;
}

void Game::init() {
    _view_size = Vec2d(800, 600);
    storage = new StorageComponent();
    
    _audio_device = alcOpenDevice(NULL);
    _audio_context = alcCreateContext(_audio_device, NULL);
    alcMakeContextCurrent(_audio_context);
    alListener3f(AL_POSITION, 0, 0, 0);
    alListener3f(AL_VELOCITY, 0, 0, 0);
    alListener3f(AL_ORIENTATION, 0, 0, -1);
    
    game_state = luaL_newstate();
    luaL_openlibs(game_state);
    luaL_openlib(game_state, "game", lua_game_functions, 0);
    lua_pop(game_state, 1);
    lua_insertpath(game_state);
    
    lua_newtable(game_state);
    lua_pushstring(game_state, getPath().c_str());
    lua_setfield(game_state, -2, "datapath");
    data_reference = luaL_ref(game_state, LUA_REGISTRYINDEX);

    insertData(game_state);
	luaL_loadfile(game_state, (getPath() + "/data/scripts/" + "init.lua").c_str());
	if(lua_pcall(game_state, 0, LUA_MULTRET, 0))
		std::cerr << lua_tostring(game_state, -1) << "\n";
	retrieveData(game_state);
}

void Game::cleanup() {
	alcDestroyContext(_audio_context);
	alcCloseDevice(_audio_device);
	for(auto e : _entities) {
		delete e.second;
	}
	delete storage;
	lua_close(game_state);
}

//void Game::addTween(Tween<float> t) {
	//_tweens.push_back(t);
//}

//void Game::addTweenSelf(std::string target_field, float target_value, float target_time, int itype, int etype) {
	//float* f;
	//if(target_field == "cam_trans.x")
		//f = cam_translate.getXRef();
	//else if(target_field == "cam_trans.y")
		//f = cam_translate.getYRef();
	//else if(target_field == "cam_trans.z")
		//f = cam_translate.getZRef();
	//else if(target_field == "cam_rot.x")
		//f = cam_rotate.getXRef();
	//else if(target_field == "cam_rot.y")
		//f = cam_rotate.getYRef();
	//else if(target_field == "cam_rot.z")
		//f = cam_rotate.getZRef();
	//else if(target_field == "cam_scale.x")
		//f = cam_scale.getXRef();
	//else if(target_field == "cam_scale.y")
		//f = cam_scale.getYRef();
	//else if(target_field == "cam_scale.z")
		//f = cam_scale.getZRef();
	//else if(target_field == "back_color.r")
		//f = bg_color.getXRef();
	//else if(target_field == "back_color.g")
		//f = bg_color.getYRef();
	//else if(target_field == "back_color.b")
		//f = bg_color.getZRef();
	//else {
		//std::cerr << "Error: Couldn't find " << target_field << "\n";
		//return;
	//}

	//Tween<float> t(f, target_value, target_time, itype, etype);
	//_tweens.push_back(t);
//}

void Game::setWindow(Window* w) {
	main_window = w;
}

void Game::run(float delta_time) {
	_time_passed += delta_time;
	if(_time_passed >= FPS) {
		_fps = _frames;
		_frames = 0;
		while(_time_passed >= FPS)
			_time_passed -= FPS;
	}
	++_frames;
	
	sys_physics.run(delta_time);
	sys_timer.run(delta_time);
	
	for(auto e : _entities)
		if(e.second->getAlive())
			e.second->update(delta_time);
	
	//for(auto i = _tweens.begin(); i != _tweens.end(); ++i) {
		//(*i).run(delta_time);
		//if(i->finished) {
			//_tweens.erase(i);
			//--i;
		//}	
	//}
	
	for(int i = 0; i < 3; ++i) {
		if(_mouse_buttons[i] == 0)
			_mouse_buttons[i] = 2;
		else if(_mouse_buttons[i] == 1)
			_mouse_buttons[i] = 3;
	}
	
	for(int i = 0; i < 350; ++i) {
		if(keyboard_keys[i] == 0)
			keyboard_keys[i] = 2;
		else if(keyboard_keys[i] == 1)
			keyboard_keys[i] = 3;
	}
	
	main_window->run(delta_time);
}

void Game::insertData(lua_State* ls) {
	lua_rawgeti(game_state, LUA_REGISTRYINDEX, data_reference);
	if(lua_isnil(game_state, -1)) {
		std::cerr << "ERROR: Failed to insert game data!\n";
		exit(1);
	}
	lua_pushstring(ls, title.c_str());
	lua_setfield(ls, -2, "window_title");
	lua_setglobal(ls, "gamedata");
	luaL_unref(ls, LUA_REGISTRYINDEX, data_reference);
	
	lua_newtable(ls);
	insertVec3d(ls, "trans", cam_translate);
	insertVec3d(ls, "rot", cam_rotate);
	insertVec3d(ls, "scale", cam_scale);
	insertVec3d(ls, "color", bg_color);
	insertVec2d(ls, "view_size", _view_size);
	lua_setglobal(ls, "camera");
	
	lua_pushstring(ls, getPath().c_str());
	lua_setglobal(ls, "datapath");
}

void Game::retrieveData(lua_State* ls) {
	lua_getglobal(ls, "camera");
	retrieveVec3d(ls, "trans", cam_translate);
	retrieveVec3d(ls, "rot", cam_rotate);
	retrieveVec3d(ls, "scale", cam_scale);
	retrieveVec3d(ls, "color", bg_color);
	retrieveVec2d(ls, "view_size", _view_size);
	lua_pop(ls, 1);
	
	lua_getglobal(ls, "gamedata");
	lua_getfield(ls, -1, "window_title");
	title = lua_tostring(ls, -1);
	if(main_window != NULL)
		main_window->setTitle(title);
	lua_pop(ls, 1);
	data_reference = luaL_ref(ls, LUA_REGISTRYINDEX);
	if(data_reference == LUA_REFNIL) {
		std::cerr << "ERROR: Failed to retrieve game data!\n";
		exit(1);
	}
}

void Game::killAll() {
	for(auto e : _entities) {
		Entity* entity = e.second;
		
		if(entity->getAlive() && !entity->getPersist()) {
			//std::cerr << "Killing " << e.first << "...\n";
			entity->setAlive(false);
		}
	}
}

void Game::draw(float delta_time, glm::mat4 nview, glm::mat4 nprojection) {
	view = nview;
	projection = nprojection;

	sys_graphics.run(delta_time);
	for(auto e : _entities) {
		Entity* entity = e.second;
		if(!entity->getDrawScr().empty() && entity->getAlive() && !entity->getDestroyed()) {
			lua_State* s1 = game_state;
		
			lua_newtable(s1);
			entity->insert(s1);
			lua_setglobal(s1, "this");
			insertData(s1);
			luaL_loadfile(s1,(getPath() + "/data/scripts/" + entity->getDrawScr()).c_str());
			if(lua_pcall(s1, 0, LUA_MULTRET, 0))
				std::cerr << lua_tostring(s1, -1) << "\n";
			lua_getglobal(s1, "this");
			entity->retrieve(s1);
			retrieveData(s1);
		}
	}
}

void Game::keyEvent(int key, int scancode, int action, int modifiers) {
	if(key < 350 && key > -1) {
		keyboard_keys[key] = action;
	}
}

void Game::mouseEvent(Vec2d position, int button, int action, int modifiers) {
	_mouse_position = position;
	if(button < 3) {
		_mouse_buttons[button] = action;
	}
}

unsigned Game::generateId() {
	if(!_free_ids.empty()) {
		unsigned return_val = _free_ids.top();
		_free_ids.pop();
		_entities[return_val]->reset();
		return return_val;
	}

	if(_next_id == UINT_MAX){
		std::cerr << "Error: Ids depleted!\n";
		return UINT_MAX;
	}
	
	_entities[_next_id] = new Entity(_next_id);
	//_entities[_next_id]._state = game_state;
	
	++_next_id;
	return _next_id - 1;
}

void Game::addComponent(unsigned entity_id, Component* cmp, std::string cmp_id) {
	if(_entities[entity_id]->getAlive()) {
		_entities[entity_id]->setComponent(cmp_id, cmp);
		int cmp_type;
		for(cmp_type = 0; cmp_type < COMPONENT_TYPE_COUNT && component_type_str[cmp_type] != cmp_id; ++cmp_type);
		switch(cmp_type) {
			case COMPONENT_GRAPHICS:
				sys_graphics.addComponent(entity_id, cmp);
			break;
			case COMPONENT_PHYSICS:
			case COMPONENT_RECTPHYSICS:
			case COMPONENT_CIRCLEPHYSICS:
			case COMPONENT_MESHPHYSICS:
				sys_physics.addComponent(entity_id, cmp);
			break;
			case COMPONENT_TIMER:
				sys_timer.addComponent(entity_id, cmp);
			break;
		}
	}
}

void Game::foreach(int type, std::string scr) {
	for(auto e : _entities) {
		glUseProgram(0);
		lua_State* ls = luaL_newstate();
		luaL_openlibs(ls);
		luaL_openlib(ls, "game", lua_game_functions, 0);
		lua_insertpath(ls);
		Entity* entity = e.second;
		if((type < 0 || (int)entity->getType() == type) && entity && entity->getAlive() && !entity->getDestroyed()) {
			lua_newtable(ls);
			entity->insert(ls);
			lua_setglobal(ls, "this");
			//insertData(game_state);
			luaL_loadfile(ls, (getPath() + "/data/scripts/" + scr).c_str());
			if(lua_pcall(ls, 0, LUA_MULTRET, 0))
				std::cerr << lua_tostring(ls, -1) << "\n";
			lua_getglobal(ls, "this");
			entity->retrieve(ls);
			//retrieveData(game_state);
		}
	}
}

Component* Game::getComponent(unsigned entity_id, std::string cmp_id) {
	if(!_entities[entity_id] || !_entities[entity_id]->getAlive()) {
		return nullptr;
	}
	
	return _entities[entity_id]->getComponent(cmp_id);
}
