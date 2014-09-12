#include "CollisionPair.h"

df::CollisionPair df::make_cpair(btPersistentManifold* pair) {
    const btCollisionObject* o1 = pair->getBody0();
    const btCollisionObject* o2 = pair->getBody1();
    CollisionPair cpair;
    cpair.id_1 = (*(int*)o1->getUserPointer());
    cpair.id_2 = (*(int*)o2->getUserPointer());
    return cpair;
}

bool df::CollisionPair::operator==(df::CollisionPair rval) {
    return (id_1 == rval.id_1 && id_2 == rval.id_2) || (id_1 == rval.id_2 && id_2 == rval.id_1);
}

bool df::CollisionPair::operator==(btPersistentManifold* rval) {
    return *this == df::make_cpair(rval);
}
