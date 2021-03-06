#ifndef DF_ENTITY
#define DF_ENTITY
#include "Util.h"

namespace df {
class Component;
class Game;

class Entity {
public:
    Entity() { }
    Entity(unsigned id);
    ~Entity();
    Component* getComponent(std::string cmp_id);
    void setComponent(std::string cmp_id, Component* cmp);
    void addComponent(std::string cmp_id, lua_State* ls);
    void setAlive(bool alive);
    bool getAlive();
    bool getDestroyed();
    bool getPersist();
    unsigned getId();
    unsigned getType();
    void reset();
    void destroy();
    void update(float dt);
    void insert(lua_State* ls);
    void retrieve(lua_State* ls);
    void setUpScr(std::string scr);
    std::string getUpScr();
    void setEnterScr(std::string scr);
    std::string getEnterScr();
    void setColScr(std::string scr);
    std::string getColScr();
    void setLeaveScr(std::string scr);
    std::string getLeaveScr();
    void setCreScr(std::string scr);
    std::string getCreScr();
    void setDrawScr(std::string scr);
    std::string getDrawScr();
//:TODO: 13.09.14 22:37:39, Hugues Ross
// Finish moving transformations to the entity class
    const float* getPosition();
    void setPosition(glm::vec3 n_position);
    const float* getQuaternionRotation();
    void setQuaternionRotation(glm::quat n_rotation);
    const float* getEulerRotation();
    void setEulerRotation(glm::vec3 n_rotation);
    const float* getScale();
    void setScale(glm::vec3 n_scale);
protected:
    lua_State* _state;
    std::string _create_script;
    std::string _update_script;
    std::string enter_script;
    std::string _collide_script;
    std::string leave_script;
    std::string _draw_script;
    unsigned _id;
    unsigned type = 0;
    bool _is_alive;
    bool _to_create = true;
    bool _to_destroy = false;
    bool persist = false;
    std::map<std::string, Component*> _components;
    float position[3] = {0, 0, 0};
    float rotation[4] = {1, 0, 0, 0};
    float scale[3] = {0, 0, 0};
};

}
#endif
