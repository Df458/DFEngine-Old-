#ifndef DF_SCOPE_H
#define DF_SCOPE_H

namespace df {

class Entity;

class Scope {
public:
    void add(unsigned, Entity*);
    void remove(unsigned);
    void clear();
    friend Entity::Entity(unsigned);
    friend void	Entity::destroy();
protected:
    glm::vec3 translation;
    glm::quat rotation;
    glm::vec3 scale;
    std::map<unsigned, Entity*> entities;
};

}
#endif
