#ifndef DF_COLLISION_PAIR
#define DF_COLLISION_PAIR
#include "Util.h"

namespace df {
enum collision_pair_type { COLLISION_ENTER = 0, COLLISION_SUSTAIN, COLLISION_LEAVE };

struct CollisionPair {
    int id_1;
    int id_2;
    collision_pair_type type = COLLISION_ENTER;
    bool operator==(df::CollisionPair rval);

    bool operator==(btPersistentManifold* rval);
};

df::CollisionPair make_cpair(btPersistentManifold* pair);
}
#endif
