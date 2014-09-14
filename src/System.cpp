#include "System.h"
#include "Game.h"
#include "Component.h"
#include "CollisionPair.h"

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

void GraphicsSystem::remove(unsigned id) {
    _components.erase(id);
}

PhysicsSystem::PhysicsSystem() {
    _broad_phase = new btDbvtBroadphase();
    _collision_config = new btDefaultCollisionConfiguration();
    _collision_dispatch = new btCollisionDispatcher(_collision_config);
    btGImpactCollisionAlgorithm::registerAlgorithm(_collision_dispatch);
    _solver = new btSequentialImpulseConstraintSolver;
    _world = new btDiscreteDynamicsWorld(_collision_dispatch, _broad_phase, _solver, _collision_config);
    _world->setGravity(btVector3(0, 10, 0));
    collisions = new std::vector<CollisionPair>();
    _world->setInternalTickCallback(physicsTickCallback, collisions);
}

PhysicsSystem::~PhysicsSystem() {
	delete _collision_config;
	delete _collision_dispatch;
	delete _solver;
	//delete _world;
}

bool PhysicsSystem::addComponent(unsigned id, Component* component){
	PhysicsComponent* cmp = (PhysicsComponent*)component;
	_components[id] = cmp;
	_world->addRigidBody(cmp->body, cmp->collision_mask, cmp->collision_hit);
	cmp->body->setUserPointer(&cmp->_owner_id);
	return true;
}

void PhysicsSystem::run(float delta_time) {
    //collision_manifolds.clear();
    _world->stepSimulation(delta_time / FPS, 10);
    
    for(unsigned i = 0; i < collisions->size(); ++i) {
	Entity* ob1 = game->getEntity(collisions->at(i).id_1);
	Entity* ob2 = game->getEntity(collisions->at(i).id_2);
	lua_State* s1;
	lua_State* s2;
	std::string scr1, scr2;
	switch(collisions->at(i).type){
	    case COLLISION_ENTER:
		collisions->at(i).type = COLLISION_SUSTAIN;
		scr1 = ob1->getEnterScr();
		scr2 = ob2->getEnterScr();
	    break;
	    case COLLISION_SUSTAIN:
		scr1 = ob1->getColScr();
		scr2 = ob2->getColScr();
	    break;
	    case COLLISION_LEAVE:
		--i;
		scr1 = ob1->getLeaveScr();
		scr2 = ob2->getLeaveScr();
		collisions->erase(collisions->begin() + i);
	    break;
	}

	if(!scr1.empty()) {
	    s1 = game->getState();
	    lua_newtable(s1);
	    ob1->insert(s1);
	    lua_setglobal(s1, "this");
	    lua_newtable(s1);
	    ob2->insert(s1);
	    lua_setglobal(s1, "other");
	    game->insertData(s1);
	    luaL_loadfile(s1,(getPath() + "/data/scripts/" + scr1).c_str());
	    if(lua_pcall(s1, 0, LUA_MULTRET, 0))
		    std::cerr << lua_tostring(s1, -1) << "\n";
	    lua_getglobal(s1, "this");
	    ob1->retrieve(s1);
	    lua_getglobal(s1, "other");
	    ob2->retrieve(s1);
	    game->retrieveData(s1);
	}
	
	if(!scr2.empty()) {
	    s2 = game->getState();
	    lua_newtable(s2);
	    ob2->insert(s2);
	    lua_setglobal(s2, "this");
	    lua_newtable(s2);
	    ob1->insert(s2);
	    lua_setglobal(s2, "other");
	    game->insertData(s2);
	    luaL_loadfile(s2,(getPath() + "/data/scripts/" + scr2).c_str());
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

void PhysicsSystem::remove(unsigned id) {
    if(_components.find(id) != _components.end()) {
	_world->removeRigidBody(_components[id]->body);
	_components.erase(id);
    }
}

void df::physicsTickCallback(btDynamicsWorld *world, btScalar timeStep) {
    int manifold_count = world->getDispatcher()->getNumManifolds();
    std::vector<CollisionPair>* collisions = (std::vector<CollisionPair>*) world->getWorldUserInfo();
    unsigned len = collisions->size();
    bool* found_c = new bool[len];
    
    for(int i = 0; i < manifold_count; ++i) {
	btPersistentManifold* contactManifold = world->getDispatcher()->getManifoldByIndexInternal(i);
	bool found = false;
	for(unsigned j = 0; j < len; ++j) {
	    if(collisions->at(j) == contactManifold) {
		found = true;
		found_c[j] = true;
		break;
	    }
	}
	if(!found) {
	    collisions->push_back(make_cpair(contactManifold));
	}
    }
    for(unsigned j = 0; j < len; ++j) {
	collisions->at(j).type = COLLISION_LEAVE;
    }
    delete found_c;
}

int PhysicsSystem::getFirstRaycastResult(const btVector3& from, const btVector3& to, short mask, short collides) {
    btCollisionWorld::ClosestRayResultCallback back(from, to);
    _world->rayTest(from, to, back);
    int target;
    if(back.m_collisionObject) {
	target = *(unsigned*)back.m_collisionObject->getUserPointer();
    } else
	target = -1;
    return target;
}

//:TODO: 12.09.14 16:23:21, Hugues Ross
// implement this
//unsigned* PhysicsSystem::getAllRaycastResults(const btVector3& from, constbtVector3& to) {
    //btCollisionWorld::AllHitsRayResultCallback back(from, to);
//}

bool TimerSystem::addComponent(unsigned id, Component* component){
	_components[id] = (TimerComponent*)component;
	return true;
}

void TimerSystem::run(float delta_time) {
	for(auto tmr_cmp : _components) {
		tmr_cmp.second->run(delta_time);
	}
}

void TimerSystem::remove(unsigned id) {
    _components.erase(id);
}
