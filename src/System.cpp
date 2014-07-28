#include "System.h"
#include "Game.h"

using namespace df;

GraphicsSystem::GraphicsSystem() {
}

bool GraphicsSystem::addComponent(unsigned id, Component* component){
	_components[id] = (GraphicsComponent*)component;
	return true;
}

void GraphicsSystem::run(float delta_time) {
	for(auto gfx_cmp : _components) {
		
		gfx_cmp.second->run(delta_time);
	}
}

PhysicsSystem::PhysicsSystem() {
	_broad_phase = new btDbvtBroadphase();
	_collision_config = new btDefaultCollisionConfiguration();
	_collision_dispatch = new btCollisionDispatcher(_collision_config);
	btGImpactCollisionAlgorithm::registerAlgorithm(_collision_dispatch);
	_solver = new btSequentialImpulseConstraintSolver;
	_world = new btDiscreteDynamicsWorld(_collision_dispatch, _broad_phase, _solver, _collision_config);
	_world->setGravity(btVector3(0, 10, 0));
}

PhysicsSystem::~PhysicsSystem() {
	delete _broad_phase;
	delete _collision_config;
	delete _collision_dispatch;
	delete _solver;
	delete _world;
}

bool PhysicsSystem::addComponent(unsigned id, Component* component){
	PhysicsComponent* cmp = (PhysicsComponent*)component;
	_components[id] = cmp;
	_world->addRigidBody(cmp->body, cmp->collision_mask, cmp->collision_hit);
	cmp->body->setUserPointer(&cmp->_owner_id);
	return true;
}

void PhysicsSystem::run(float delta_time) {
	_world->stepSimulation(delta_time / FPS, 10);
	btTransform trans;
	
	int manifold_count = _world->getDispatcher()->getNumManifolds();
	for(int i = 0; i < manifold_count; ++i) {
		btPersistentManifold* contactManifold = _world->getDispatcher()->getManifoldByIndexInternal(i);
		bool collided = false;
		for(int i = 0; i < contactManifold->getNumContacts(); ++i)
			if(contactManifold->validContactDistance(contactManifold->getContactPoint(i)))
				collided = true;
		if(!collided)
			continue;
		
		const btCollisionObject* object0 = contactManifold->getBody0();
		const btCollisionObject* object1 = contactManifold->getBody1();
		
		Entity* ob1 = game->getEntity((*(int*)object0->getUserPointer()));
		Entity* ob2 = game->getEntity((*(int*)object1->getUserPointer()));
		lua_State* s1;
		lua_State* s2;
		/*s1 = luaL_newstate();
		luaL_openlibs(s1);
		luaL_openlib(s1, "game", lua_game_functions, 0);
		lua_insertpath(s1);
		s2 = luaL_newstate();
		luaL_openlibs(s2);
		luaL_openlib(s2, "game", lua_game_functions, 0);
		lua_insertpath(s2);*/
		
		
		
		if(!ob1->getColScr().empty()) {
			s1 = game->getState();
			lua_newtable(s1);
			ob1->insert(s1);
			lua_setglobal(s1, "this");
			lua_newtable(s1);
			ob2->insert(s1);
			lua_setglobal(s1, "other");
			game->insertData(s1);
			luaL_loadfile(s1,(getPath() + "/data/scripts/" + ob1->getColScr()).c_str());
			if(lua_pcall(s1, 0, LUA_MULTRET, 0))
				std::cerr << lua_tostring(s1, -1) << "\n";
			lua_getglobal(s1, "this");
			ob1->retrieve(s1);
			lua_getglobal(s1, "other");
			ob2->retrieve(s1);
			game->retrieveData(s1);
		}
		
		if(!ob2->getColScr().empty()) {
			s2 = game->getState();
			lua_newtable(s2);
			ob2->insert(s2);
			lua_setglobal(s2, "this");
			lua_newtable(s2);
			ob1->insert(s2);
			lua_setglobal(s2, "other");
			game->insertData(s2);
			luaL_loadfile(s2,(getPath() + "/data/scripts/" + ob2->getColScr()).c_str());
			if(lua_pcall(s2, 0, LUA_MULTRET, 0))
				std::cerr << lua_tostring(s2, -1) << "\n";
			lua_getglobal(s2, "this");
			ob2->retrieve(s2);
			lua_getglobal(s2, "other");
			ob1->retrieve(s2);
			game->retrieveData(s2);
		}
	}
}

bool TimerSystem::addComponent(unsigned id, Component* component){
	_components[id] = (TimerComponent*)component;
	return true;
}

void TimerSystem::run(float delta_time) {
	for(auto tmr_cmp : _components) {
		tmr_cmp.second->run(delta_time);
	}
}