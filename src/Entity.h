#ifndef DF_ENTITY
#define DF_ENTITY
#include "Util.h"
#include "Component.h"

namespace df {

class Game;

class Entity {
public:
	Entity() { }
	Entity(unsigned id);
	~Entity();
	Component* getComponent(std::string cmp_id) { if (_components.find(cmp_id) != _components.end()) return _components[cmp_id]; return NULL; }
	void setComponent(std::string cmp_id, Component* cmp) { cmp->_owner_id = _id; _components[cmp_id] = cmp; }
	void addComponent(std::string cmp_id, lua_State* ls);
	void setAlive(bool alive) { _to_destroy = !alive; }
	bool getAlive() { return _is_alive; }
	bool getDestroyed() { return _to_destroy; }
	bool getPersist() { return persist; }
	unsigned getId() { return _id; }
	unsigned getType() { return type; }
	void reset() { _is_alive = true; _to_create = true; for(auto i : _components) { delete i.second; } _components.clear(); _components["storage"] = new StorageComponent; _create_script.clear(); _update_script.clear();  _collide_script.clear(); _draw_script.clear();}
	void destroy();
	void update(float dt);
	void insert(lua_State* ls);
	void retrieve(lua_State* ls);
	void setUpScr(std::string scr) { _update_script = scr; }
	std::string getUpScr() { return _update_script; }
	void setColScr(std::string scr) { _collide_script = scr; }
	std::string getColScr() { return _collide_script; }
	void setCreScr(std::string scr) { _create_script = scr; }
	std::string getCreScr() { return _create_script; }
	void setDrawScr(std::string scr) { _draw_script = scr; }
	std::string getDrawScr() { return _draw_script; }
protected:
	lua_State* _state;
	std::string _create_script;
	std::string _update_script;
	std::string _collide_script;
	std::string _draw_script;
	unsigned _id;
	unsigned type = 0;
	bool _is_alive;
	bool _to_create = true;
	bool _to_destroy = false;
	bool persist = false;
	std::map<std::string, Component*> _components;
};

}
#endif
