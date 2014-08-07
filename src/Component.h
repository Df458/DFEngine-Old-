#ifndef DF_COMPONENT
#define DF_COMPONENT
#include "Util.h"
#include "Model.h"

namespace df {

class Entity;

enum component_type {COMPONENT_BASIC = 0, 
					 COMPONENT_GRAPHICS,
					 COMPONENT_PHYSICS,
					 COMPONENT_RECTPHYSICS, 
					 COMPONENT_CIRCLEPHYSICS, 
					 COMPONENT_MESHPHYSICS, 
					 COMPONENT_STORAGE, 
					 COMPONENT_TIMER, 
					 COMPONENT_TYPE_COUNT};
const std::string component_type_str[COMPONENT_TYPE_COUNT] = { "basic", "graphics", "physics", "rectangle physics", "circle physics", "mesh physics", "storage", "timer" };

enum collision_type { COLLISION_NONE, COLLISION_RECT, COLLISION_CIRCLE, COLLISION_MESH };

class Component {
public:
	virtual ~Component() {};
	void run(float delta_time) { _runSub(delta_time); }
	void* retrieve(std::string key) { return _retrieveSub(key); }
	void insertData(lua_State* ls) { _insertDataSub(ls); }
	void retrieveData(lua_State* ls) { return _retrieveDataSub(ls); }
	friend class Entity;
	virtual void _runSub(float delta_time) = 0;
	virtual void* _retrieveSub(std::string key) = 0;
	virtual void _insertDataSub(lua_State* ls) = 0;
	virtual void _retrieveDataSub(lua_State* ls) = 0;
	unsigned _owner_id = 0;
};

class GraphicsComponent : public Component {
public:
	GraphicsComponent();
	virtual void _runSub(float delta_time);
	virtual void* _retrieveSub(std::string key) { 
		if(key == "color.r") return &color.x;
		if(key == "color.g") return &color.y;
		if(key == "color.b") return &color.z;
		if(key == "alpha") return &alpha;
		return nullptr;
	}
	virtual void _insertDataSub(lua_State* ls) {
		lua_newtable(ls);
		lua_pushnumber(ls, color.x);
		lua_setfield(ls, -2, "r");
		lua_pushnumber(ls, color.y);
		lua_setfield(ls, -2, "g");
		lua_pushnumber(ls, color.z);
		lua_setfield(ls, -2, "b");
		lua_pushnumber(ls, alpha);
		lua_setfield(ls, -2, "a");
	
		lua_setfield(ls, -2, "color");
		
		insertVec3d(ls, "scale", scale);
		
		lua_pushinteger(ls, texture);
		lua_setfield(ls, -2, "texture");
		
		lua_pushinteger(ls, uv);
		lua_setfield(ls, -2, "uv");
		
		lua_pushinteger(ls, program);
		lua_setfield(ls, -2, "shader_program");
	}
	virtual void _retrieveDataSub(lua_State* ls) {
		lua_getfield(ls, -1, "color");
		lua_getfield(ls, -1, "r");
		color.x = lua_tonumber(ls, -1);
		lua_getfield(ls, -2, "g");
		color.y = lua_tonumber(ls, -1);
		lua_getfield(ls, -3, "b");
		color.z = lua_tonumber(ls, -1);
		lua_getfield(ls, -4, "a");
		alpha = lua_tonumber(ls, -1);
		lua_pop(ls, 5);
		
		retrieveVec3d(ls, "scale", scale);
		
		lua_getfield(ls, -1, "texture");
		texture = lua_tointeger(ls, -1);
		lua_getfield(ls, -2, "uv");
		uv = lua_tointeger(ls, -1);
		lua_getfield(ls, -3, "shader_program");
		program = lua_tointeger(ls, -1);
		lua_pop(ls, 3);
	}
	Vec3d color;
	float alpha = 1;
	GLuint program;
	Model model;
	GLuint texture;
	GLuint uv;
	Vec3d scale = Vec3d(1, 1, 1);
};

class PhysicsComponent : public Component {
public:
	PhysicsComponent(btRigidBody* body_n) { body = body_n; }
	~PhysicsComponent() { delete body; }
	virtual void _runSub(float delta_time) {}
	virtual void* _retrieveSub(std::string key) { 
		return nullptr;
	}
	virtual void _insertDataSub(lua_State* ls) {
		btTransform trans;
		btVector3 linear_vel = body->getLinearVelocity();
		btVector3 angular_vel = body->getAngularVelocity();
		btVector3 linear_factor = body->getLinearFactor();
		body->getMotionState()->getWorldTransform(trans);
		
		lua_newtable(ls);
		lua_pushnumber(ls, trans.getOrigin().getX());
		lua_setfield(ls, -2, "x");
		lua_pushnumber(ls, trans.getOrigin().getY());
		lua_setfield(ls, -2, "y");
		lua_pushnumber(ls, trans.getOrigin().getZ());
		lua_setfield(ls, -2, "z");

		lua_setfield(ls, -2, "position");
		
		lua_newtable(ls);
		lua_pushnumber(ls, trans.getRotation().getAxis().getX());
		lua_setfield(ls, -2, "x");
		lua_pushnumber(ls, trans.getRotation().getAxis().getY());
		lua_setfield(ls, -2, "y");
		lua_pushnumber(ls, trans.getRotation().getAxis().getZ());
		lua_setfield(ls, -2, "z");
		lua_pushnumber(ls, trans.getRotation().getW());
		lua_setfield(ls, -2, "w");

		lua_setfield(ls, -2, "rotation");
		
		lua_newtable(ls);
		lua_pushnumber(ls, linear_vel.getX());
		lua_setfield(ls, -2, "x");
		lua_pushnumber(ls, linear_vel.getY());
		lua_setfield(ls, -2, "y");
		lua_pushnumber(ls, linear_vel.getZ());
		lua_setfield(ls, -2, "z");

		lua_setfield(ls, -2, "velocity");
		
		lua_newtable(ls);
		lua_pushnumber(ls, angular_vel.getX());
		lua_setfield(ls, -2, "x");
		lua_pushnumber(ls, angular_vel.getY());
		lua_setfield(ls, -2, "y");
		lua_pushnumber(ls, angular_vel.getZ());
		lua_setfield(ls, -2, "z");

		lua_setfield(ls, -2, "angular_velocity");
		
		lua_newtable(ls);
		lua_pushnumber(ls, linear_factor.getX());
		lua_setfield(ls, -2, "x");
		lua_pushnumber(ls, linear_factor.getY());
		lua_setfield(ls, -2, "y");
		lua_pushnumber(ls, linear_factor.getZ());
		lua_setfield(ls, -2, "z");

		lua_setfield(ls, -2, "linear_factor");
		
		lua_pushnumber(ls, body->getLinearDamping());
		lua_setfield(ls, -2, "linear_damp");
		
		lua_newtable(ls);
		lua_pushnumber(ls, body->getGravity().getX());
		lua_setfield(ls, -2, "x");
		lua_pushnumber(ls, body->getGravity().getY());
		lua_setfield(ls, -2, "y");
		lua_pushnumber(ls, body->getGravity().getZ());
		lua_setfield(ls, -2, "z");

		lua_setfield(ls, -2, "gravity");
		
		lua_pushnumber(ls, body->getFriction());
		lua_setfield(ls, -2, "friction");
		
		btBroadphaseProxy* proxy = body->getBroadphaseProxy();
		if(proxy) {
			lua_pushinteger(ls, proxy->m_collisionFilterMask);
			lua_setfield(ls, -2, "collision_mask");
		
			lua_pushinteger(ls, proxy->m_collisionFilterGroup);
			lua_setfield(ls, -2, "collision_group");
		}
	}
	virtual void _retrieveDataSub(lua_State* ls) {
		btTransform trans;
		btTransform old_trans;
		body->getMotionState()->getWorldTransform(trans);
		body->getMotionState()->getWorldTransform(old_trans);
		
		lua_getfield(ls, -1, "position");
		lua_getfield(ls, -1, "x");
		lua_getfield(ls, -2, "y");
		lua_getfield(ls, -3, "z");
		trans.setOrigin(btVector3(lua_tonumber(ls, -3), lua_tonumber(ls, -2),lua_tonumber(ls, -1)));
		lua_pop(ls, 4);
		
		/*lua_getfield(ls, -1, "rotation");
		lua_getfield(ls, -1, "x");
		lua_getfield(ls, -2, "y");
		lua_getfield(ls, -3, "z");
		lua_getfield(ls, -4, "w");
		trans.setRotation(btQuaternion(btVector3(lua_tonumber(ls, -4), lua_tonumber(ls, -3),lua_tonumber(ls, -2)), lua_tonumber(ls, -1)));
		lua_pop(ls, 5);*/
		
		if(trans.getOrigin() != old_trans.getOrigin() || trans.getRotation() != old_trans.getRotation()) {
			body->getMotionState()->setWorldTransform(trans);
			body->setWorldTransform(trans);
		}
		
		lua_getfield(ls, -1, "velocity");
		lua_getfield(ls, -1, "x");
		lua_getfield(ls, -2, "y");
		lua_getfield(ls, -3, "z");
		btVector3 vel(lua_tonumber(ls, -3), lua_tonumber(ls, -2), lua_tonumber(ls, -1));
		if(body->getLinearVelocity() != vel)
			body->setLinearVelocity(vel);
		lua_pop(ls, 4);
		
		lua_getfield(ls, -1, "angular_velocity");
		lua_getfield(ls, -1, "x");
		lua_getfield(ls, -2, "y");
		lua_getfield(ls, -3, "z");
		btVector3 avel(lua_tonumber(ls, -3), lua_tonumber(ls, -2), lua_tonumber(ls, -1));
		if(body->getAngularVelocity() != avel)
			body->setAngularVelocity(avel);
		lua_pop(ls, 4);
		
		lua_getfield(ls, -1, "linear_factor");
		lua_getfield(ls, -1, "x");
		lua_getfield(ls, -2, "y");
		lua_getfield(ls, -3, "z");
		btVector3 lfac(lua_tonumber(ls, -3), lua_tonumber(ls, -2), lua_tonumber(ls, -1));
		if(body->getLinearFactor() != lfac) {
			body->setLinearFactor(lfac);
		}
		lua_pop(ls, 4);
		
		lua_getfield(ls, -1, "gravity");
		lua_getfield(ls, -1, "x");
		lua_getfield(ls, -2, "y");
		lua_getfield(ls, -3, "z");
		btVector3 grav(lua_tonumber(ls, -3), lua_tonumber(ls, -2), lua_tonumber(ls, -1));
		if(body->getGravity() != grav)
			body->setGravity(grav);
		lua_pop(ls, 4);
		
		lua_getfield(ls, -1, "linear_damp");
		btScalar ldamp = lua_tonumber(ls, -1);
		if(ldamp != body->getLinearDamping())
			body->setDamping(ldamp, body->getAngularDamping());
		lua_pop(ls, 1);
		
		lua_getfield(ls, -1, "friction");
		btScalar fric = lua_tonumber(ls, -1);
		if(fric != body->getFriction())
			body->setFriction(fric);
		lua_pop(ls, 1);
		
		btBroadphaseProxy* proxy = body->getBroadphaseProxy();
		if(proxy) {
			lua_getfield(ls, -1, "collision_mask");
			proxy->m_collisionFilterMask = lua_tointeger(ls, -1);
			lua_pop(ls, 1);
			lua_getfield(ls, -1, "collision_group");
			proxy->m_collisionFilterGroup = lua_tointeger(ls, -1);
			lua_pop(ls, 1);
		}
	}
	
	btRigidBody* body;
	short collision_mask = 0;
	short collision_hit  = 0;
};

class StorageComponent : public Component {
public:
	~StorageComponent() {}
	virtual void _runSub(float delta_time) { }
	virtual void* _retrieveSub(std::string key) { return nullptr; }
	virtual void _insertDataSub(lua_State* ls) {
		lua_newtable(ls);
		for(auto i : _numbers) {
			lua_pushnumber(ls, i.second);
			lua_setfield(ls, -2, i.first.c_str());
		}
		for(auto i : _text) {
			lua_pushstring(ls, i.second.c_str());
			lua_setfield(ls, -2, i.first.c_str());
		}
		for(auto i : _bools) {
			lua_pushboolean(ls, i.second);
			lua_setfield(ls, -2, i.first.c_str());
		}
		lua_setfield(ls, -2, "exdata");
	}
	virtual void _retrieveDataSub(lua_State* ls) {
		lua_getfield(ls, -1, "exdata");
		lua_pushnil(ls);
		while(lua_next(ls, -2)) {
			if(lua_isnumber(ls, -1))
				_numbers[lua_tostring(ls, -2)] = lua_tonumber(ls, -1);
			else if(lua_isstring(ls, -1))
				_text[lua_tostring(ls, -2)] = lua_tostring(ls, -1);
			else if(lua_isboolean(ls, -1))
				_bools[lua_tostring(ls, -2)] = lua_toboolean(ls, -1);
			lua_pop(ls, 1);
		}
		lua_pop(ls, 1);
	}
protected:
	std::map<std::string, float> _numbers;
	std::map<std::string, std::string> _text;
	std::map<std::string, bool> _bools;
};

struct timer {
	float time = -1;
	std::string func = "";
};

class TimerComponent : public Component {
public:
	TimerComponent() {}
	~TimerComponent() { _timers.clear(); }
	virtual void _runSub(float delta_time);
	virtual void* _retrieveSub(std::string key) { return nullptr; }
	virtual void _insertDataSub(lua_State* ls);
	virtual void _retrieveDataSub(lua_State* ls);
protected:
	std::map<unsigned, timer> _timers;
};

}
#endif
