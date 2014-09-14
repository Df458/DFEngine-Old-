#ifndef ENTITY_MOTION_STATE
#define ENTITY_MOTION_STATE
#include "Util.h"

namespace df {

class Entity;

class EntityMotionState : public btMotionState {
public:
    EntityMotionState(btTransform transform);
    virtual void getWorldTransform(btTransform& transform) const;
    virtual void setWorldTransform(const btTransform& transform);
    void registerEntity(Entity* n_owner);
private:
    df::Entity* owner = NULL;
    btTransform original_transform;
};

}
#endif
