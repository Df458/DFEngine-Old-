#ifndef DF_GAME
#define DF_GAME
#include "Util.h"
#include "System.h"
#include "Entity.h"
#include "AssetManager.h"
#include "Tween.h"

namespace df{
class StorageComponent;
class Window;

class Game {
public:
	Game() {}
	~Game() {}
	
	void init();
	void cleanup();
	void run(float delta_time);
	void draw(float delta_time, glm::mat4 view, glm::mat4 projection);
	void addTween(Tween<float> t);
	void addTweenSelf(std::string target_field, float target_value, float target_time, int itype, int etype);
	void keyEvent(int key, int scancode, int action, int modifiers);
	void mouseEvent(Vec2d position, int button, int action, int	modifiers);
	void foreach(int type, std::string script);
	int getFPS() { return _fps; }
	Vec2d getMousePosition() { return _mouse_position; }
	Vec3d getBg() { return bg_color; }
	unsigned char* getMouseButtons() { return _mouse_buttons; }
	unsigned char* getKeyboardKeys() { return keyboard_keys; }
	void setMousePosition(Vec2d mp) { _mouse_position = mp; }
	AssetManager* getAssetManager();
	unsigned generateId();
	void addComponent(unsigned entity_id, Component* cmp, std::string cmp_id);
	Component* getComponent(unsigned entity_id, std::string cmp_id);
	Entity* getEntity(unsigned entity_id) { return _entities[entity_id]; }
	void insertData(lua_State* ls);
	void retrieveData(lua_State* ls);
	void killAll();
	void clearTweens() { _tweens.clear(); }
	Vec2d getViewSize() { return _view_size; }
	Vec3d getCameraTranslate() {return cam_translate;}
	Vec3d getCameraRotate() {return cam_rotate;}
	Vec3d getCameraScale() {return cam_scale;}
	lua_State* getState() { return game_state; }
	glm::mat4 getCameraView() { return view; }
	glm::mat4 getCameraProjection() { return projection; }
	void setWindow(Window* w);
	friend Entity::Entity(unsigned);
	friend void Entity::destroy();
protected:
	StorageComponent* storage;
	int data_reference;
	GraphicsSystem sys_graphics;
	PhysicsSystem sys_physics;
	TimerSystem sys_timer;
	AssetManager asset_library;
	
	ALCdevice* _audio_device;
	ALCcontext* _audio_context;
	
	std::map<unsigned, Entity*> _entities;
	std::vector<Tween<float>> _tweens;
	float _time_passed = 0;
	int _frames = 0;
	int _fps = 0;
	Vec2d _mouse_position;
	Vec2d _view_size;
	Vec3d cam_translate;
	Vec3d cam_rotate;
	Vec3d cam_scale = Vec3d(1, 1, 1);
	Vec3d bg_color;
	unsigned char _mouse_buttons[3]; //0 release, 1 press, 2 up, 3 down
	unsigned char keyboard_keys[350];
	unsigned _next_id = 0;
	std::stack<unsigned> _free_ids;
	
	lua_State* game_state;
	glm::mat4 view, projection;
	Window* main_window = NULL;
	std::string title = "DFEngine";
};

extern Game* game;
}
#endif
