#include "EntityMotionState.h"
#include "Entity.h"

using namespace df;

EntityMotionState::EntityMotionState(btTransform transform) {
    original_transform = transform;
}

void EntityMotionState::getWorldTransform(btTransform& transform) const {
    if(!owner) {
	transform = original_transform;
	return;
    }

    const float* position = owner->getPosition();
    transform.setOrigin(btVector3(position[0], position[1], position[2]));
    const float* rotation = owner->getQuaternionRotation();
    transform.setRotation(btQuaternion(rotation[0], rotation[1], rotation[2], rotation[3]));
}

void EntityMotionState::setWorldTransform(const btTransform& transform) {
    if(owner) {
	btVector3 pos = transform.getOrigin();
	owner->setPosition(glm::vec3(pos.x(), pos.y(), pos.z()));
	btQuaternion quat = transform.getRotation();
	owner->setQuaternionRotation(glm::quat(quat.w(), quat.x(), quat.y(), quat.z()));
    } else {
	original_transform = transform;
    }
}

void EntityMotionState::registerEntity(Entity* n_owner) {
    owner = n_owner;
    setWorldTransform(original_transform);
}
