#ifndef DF_SYSTEM
#define DF_SYSTEM
#include "Util.h"
#include "CollisionPair.h"

namespace df {
class Component;
class GraphicsComponent;
class PhysicsComponent;
class TimerComponent;

class System {
public:
	virtual void run(float delta_time) = 0;
	virtual bool addComponent(unsigned id, Component* component) = 0;
	virtual void remove(unsigned id) = 0;
};

class GraphicsSystem : public System {
public:
	GraphicsSystem();
	void run(float delta_time);
	bool addComponent(unsigned id, Component* component);
	void remove(unsigned id);
protected:
	std::map<unsigned, GraphicsComponent*> _components;
};

class PhysicsSystem : public System {
public:
    PhysicsSystem();
    ~PhysicsSystem();
    void run(float delta_time);
    bool addComponent(unsigned id, Component* component);
    void remove(unsigned id);
protected:
    std::map<unsigned, PhysicsComponent*> _components;
    btBroadphaseInterface* _broad_phase;
    btDefaultCollisionConfiguration* _collision_config;
    btCollisionDispatcher* _collision_dispatch;
    btSequentialImpulseConstraintSolver* _solver;
    btDiscreteDynamicsWorld* _world;
    btRigidBody* _test_sphere;
    std::vector<CollisionPair> collisions;
};

void physicsTickCallback(btDynamicsWorld *world, btScalar timeStep);

class TimerSystem : public System {
public:
	TimerSystem() {}
	void run(float delta_time);
	bool addComponent(unsigned id, Component* component);
	void remove(unsigned id);
protected:
	std::map<unsigned, TimerComponent*> _components;
};
}
#endif
