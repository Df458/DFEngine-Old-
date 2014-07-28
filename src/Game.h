#ifndef DF_GAME
#define DF_GAME
#include "Util.h"
#include "System.h"
#include "Tween.h"
#include "Font.h"
#include "AssetManager.h"
#include "Entity.h"

namespace df{

class Game {
public:
	Game() {}
	~Game() {}
	
	void init();
	void cleanup();
	void run(float delta_time);
	void draw(float delta_time);
	void addTween(Tween<float> t) { _tweens.push_back(t); }
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
	AssetManager* getAssetManager() { return &asset_library; }
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
};

extern Game* game;

static int lua_getGameFPS(lua_State* ls) {
	lua_pushinteger(ls, game->getFPS());
	return 1;
}

static int lua_loadSound(lua_State* ls) {
	game->getAssetManager()->loadSound(lua_tostring(ls, 1));
	lua_pushinteger(ls, game->getAssetManager()->getSound(lua_tostring(ls, 1)));
	return 1;
}

static int lua_loadFont(lua_State* ls) {
	game->getAssetManager()->loadFont(lua_tostring(ls, 1), Vec2d(lua_tonumber(ls, 2), lua_tonumber(ls, 3)));
	return 0;
}

static int lua_loadTexture(lua_State* ls) {
	game->getAssetManager()->loadTexture(lua_tostring(ls, 1));
	lua_pushinteger(ls, game->getAssetManager()->getTexture(lua_tostring(ls, 1)));
	return 1;
}

static int lua_loadShader(lua_State* ls) {
	game->getAssetManager()->loadShader(lua_tostring(ls, 1));
	return 0;
}

static int lua_createProgram(lua_State* ls) {
	int arg_count = lua_gettop(ls);
	std::string program_id = lua_tostring(ls, 1);
	std::vector<std::string>vec;
	for(int i = 2; i <= arg_count; ++i) {
		vec.push_back(lua_tostring(ls, i));
	}
	
	game->getAssetManager()->compileProgram(program_id, vec);	
	return 0;
}

static int lua_clearTweens(lua_State* ls) {
	game->clearTweens();
	return 0;
}

static int lua_drawText(lua_State* ls) {
	int arg_count = lua_gettop(ls);
	std::string font_name = lua_tostring(ls, 1);
	std::string text = lua_tostring(ls, 2);
	Vec3d position, rotation, scale(1,1,1), color;
	float alpha = 1;
	
	if(arg_count >= 5) {
		position = Vec3d(lua_tonumber(ls, 3), lua_tonumber(ls, 4), lua_tonumber(ls, 5));
	}
	if(arg_count >= 8) {
		rotation = Vec3d(lua_tonumber(ls, 6), lua_tonumber(ls, 7), lua_tonumber(ls, 8));
	}
	if(arg_count >= 11) {
		scale = Vec3d(lua_tonumber(ls, 9), lua_tonumber(ls, 10), lua_tonumber(ls, 11));
	}
	if(arg_count >= 15) {
		color = Vec3d(lua_tonumber(ls, 12), lua_tonumber(ls, 13), lua_tonumber(ls, 14));
		alpha = lua_tonumber(ls, 15);
	}
	
	game->getAssetManager()->getFont(font_name)->draw(text, position, rotation, scale, color, alpha);
	return 0;
}

static int lua_drawRect(lua_State* ls) {
	glPushMatrix();
	int arg_count = lua_gettop(ls);
	if(arg_count < 10)
		glDisable(GL_TEXTURE_2D);
	else {
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, lua_tointeger(ls, 10));
	}
	glTranslatef(lua_tonumber(ls, 1), lua_tonumber(ls, 2), lua_tonumber(ls, 3));
	if(arg_count >= 9)
		glColor4f(lua_tonumber(ls, 6), lua_tonumber(ls, 7), lua_tonumber(ls, 8), lua_tonumber(ls, 9));
	drawGlRect(lua_tonumber(ls, 4), lua_tonumber(ls, 5));
	glPopMatrix();
	return 0;
}

static int lua_generateID(lua_State* ls) {
	lua_pushinteger(ls, game->generateId());
	return 1;
}

static int lua_addComponent(lua_State* ls) {
	int arg_count = lua_gettop(ls);
	int id = lua_tonumber(ls, 1);
	int component_type = lua_tonumber(ls, 2);
	//Current: id, w, h, d, gravity, mask, collides, <x, y, z>, <rot1, rot2, rot3, rot4>, <restitution>
	//New: id, typestring, x, y, z, rotx, roty, rotz, rotw, mask, collides
	switch(component_type) {
		case COMPONENT_RECTPHYSICS: {
			btCollisionShape* shape = new btBoxShape(btVector3(lua_tonumber(ls, 3)/10.0f, lua_tonumber(ls, 4)/10.0f, 1));
			bool has_gravity = lua_toboolean(ls, 5);
			btVector3 translation;
			btQuaternion rotation(0,0,0,1);
			short mask = lua_tointeger(ls, 6);
			short collides = lua_tointeger(ls, 7);
			if(arg_count >= 10)
				translation = btVector3(lua_tonumber(ls, 8) / 10.0f, lua_tonumber(ls, 9) / 10.0f, lua_tonumber(ls, 10) / 10.0f);
			if(arg_count >= 14)
				rotation = btQuaternion(lua_tonumber(ls, 11), lua_tonumber(ls, 12), lua_tonumber(ls, 13), lua_tonumber(ls, 14));
			btDefaultMotionState* motion_state = new btDefaultMotionState(btTransform(rotation, translation));
			btScalar mass = (lua_tonumber(ls, 3) + lua_tonumber(ls, 4)) / 2;
			if(!has_gravity)
				mass = 0;
			btVector3 inertia(0, 0, 0);
    		shape->calculateLocalInertia(mass, inertia);
			btRigidBody::btRigidBodyConstructionInfo c_info(mass, motion_state, shape, inertia);
			btRigidBody* body = new btRigidBody(c_info);
			body->setLinearFactor(btVector3(1, 1, 0));
			body->setAngularFactor(btVector3(0, 0, 0));
			if(arg_count >= 15)	{
				body->setRestitution(lua_tonumber(ls, 15));
			} else
				body->setRestitution(1);
			body->setFriction(0);
			PhysicsComponent* cmp_dat = new PhysicsComponent(body);
			cmp_dat->collision_mask = mask;
			cmp_dat->collision_hit = collides;
			game->addComponent(id, cmp_dat, component_type_str[COMPONENT_PHYSICS]);
		} break;
		case COMPONENT_CIRCLEPHYSICS: {
			btCollisionShape* shape = new btSphereShape(lua_tonumber(ls, 3) / 10.0f);
			bool has_gravity = lua_toboolean(ls, 4);
			btVector3 translation;
			btQuaternion rotation(0,0,0,1);
			short mask = lua_tointeger(ls, 5);
			short collides = lua_tointeger(ls, 6);
			if(arg_count >= 9)
				translation = btVector3(lua_tonumber(ls, 7) / 10.0f, lua_tonumber(ls, 8) / 10.0f, lua_tonumber(ls, 9) / 10.0f);
			if(arg_count >= 13)
				rotation = btQuaternion(lua_tonumber(ls, 10), lua_tonumber(ls, 11), lua_tonumber(ls, 12), lua_tonumber(ls, 13));
			btDefaultMotionState* motion_state = new btDefaultMotionState(btTransform(rotation, translation));
			btScalar mass = lua_tonumber(ls, 3);
			if(!has_gravity)
				mass = 0;
			btVector3 inertia(0, 0, 0);
    		shape->calculateLocalInertia(mass, inertia);
			btRigidBody::btRigidBodyConstructionInfo c_info(mass, motion_state, shape, inertia);
			btRigidBody* body = new btRigidBody(c_info);
			body->setLinearFactor(btVector3(1, 1, 0));
			body->setAngularFactor(btVector3(0, 0, 1));
			if(arg_count >= 14)	{
				body->setRestitution(lua_tonumber(ls, 14));
			}else
				body->setRestitution(1);
			body->setFriction(0);
			PhysicsComponent* cmp_dat = new PhysicsComponent(body);
			cmp_dat->collision_mask = mask;
			cmp_dat->collision_hit = collides;
			game->addComponent(id, cmp_dat, component_type_str[COMPONENT_PHYSICS]);
		} break;
		
		case COMPONENT_RECTGRAPHICS: {
			RectGraphicsComponent* cmp_dat = new RectGraphicsComponent;
			cmp_dat->has_texture = lua_toboolean(ls, 3);
			cmp_dat->texture_id = lua_tointeger(ls, 4);
			if(arg_count >= 6)
				cmp_dat->dimensions = Vec2d(lua_tonumber(ls, 5), lua_tonumber(ls, 6));
			if(arg_count >= 10) {
				cmp_dat->color = Vec3d(lua_tonumber(ls, 7), lua_tonumber(ls, 8), lua_tonumber(ls, 9));
				cmp_dat->alpha = lua_tonumber(ls, 10);
			}
			game->addComponent(id, cmp_dat, component_type_str[component_type]);
		} break;
		
		case COMPONENT_STORAGE: {
			game->addComponent(id, new StorageComponent, component_type_str[component_type]);
		} break;
		
		case COMPONENT_TIMER: {
			game->addComponent(id, new TimerComponent, component_type_str[component_type]);
		} break;
	}
	
	return 0;
}

static int lua_setScript(lua_State* ls) {
	int id = lua_tonumber(ls, 1);
	std::string script_type = lua_tostring(ls, 2);
	std::string script_value = lua_tostring(ls, 3);
	Entity* en = game->getEntity(id);
	
	if(script_type == "update")
		en->setUpScr(script_value);
	else if(script_type == "collide")
		en->setColScr(script_value);
	else if(script_type == "create")
		en->setCreScr(script_value);
	else if(script_type == "draw")
		en->setDrawScr(script_value);
	return 0;
}

static int lua_playSound(lua_State* ls) {
	int se;
	if(lua_isnumber(ls, 1)) {
		se = lua_tointeger(ls, 1);
	} else {
		std::string snd = lua_tostring(ls, 1);
		se = game->getAssetManager()->getSound(snd);
	}
	alSourcePlay(se);
	return 0;
}

static int lua_getTexture(lua_State* ls) {
	std::string tex = lua_tostring(ls, 1);
	lua_pushinteger(ls, game->getAssetManager()->getTexture(tex));
	return 1;
}

static int lua_getSound(lua_State* ls) {
	std::string tex = lua_tostring(ls, 1);
	lua_pushinteger(ls, game->getAssetManager()->getSound(tex));
	return 1;
}

static int lua_getShader(lua_State* ls) {
	std::string tex = lua_tostring(ls, 1);
	lua_pushinteger(ls, game->getAssetManager()->getShader(tex));
	return 1;
}

static int lua_getProgram(lua_State* ls) {
	std::string tex = lua_tostring(ls, 1);
	lua_pushinteger(ls, game->getAssetManager()->getProgram(tex));
	return 1;
}

static int lua_clearAll(lua_State* ls) {
	game->killAll();
	return 0;
}

static int lua_addTween(lua_State* ls) {
	int arg_count = lua_gettop(ls);
	int id = lua_tonumber(ls, 1);
	std::string component_type = lua_tostring(ls, 2);
	std::string target_string = lua_tostring(ls, 3);
	float target_value = lua_tonumber(ls, 4);
	float target_time = lua_tonumber(ls, 5);
	int interp = TWEEN_INTERP_LINEAR;
	int ending = TWEEN_END;
	if(arg_count > 5)
		interp = lua_tonumber(ls, 6);
	if(arg_count > 6)
		ending = lua_tonumber(ls, 7);
		
	if(id == -1) {
		game->addTweenSelf(target_string, target_value, target_time, interp, ending);
		return 0;
	}
	
	Component* cmp = game->getComponent(id, component_type);
	if(!cmp)
		return 0;
	void* target = cmp->retrieve(target_string);
	
	if(cmp != nullptr && target) {
		game->addTween(Tween<float>(target, target_value, target_time, interp, ending));
	}
	return 0;
}

static int lua_getMouseData(lua_State* ls) {
	Vec2d pos = game->getMousePosition();
	unsigned char* mb = game->getMouseButtons();
	lua_pushnumber(ls, pos.x);
	lua_pushnumber(ls, pos.y);
	lua_pushinteger(ls, mb[0]);
	lua_pushinteger(ls, mb[1]);
	lua_pushinteger(ls, mb[2]);
	return 5;
}

static int lua_getKeyboardData(lua_State* ls) {
	unsigned char* keys = game->getKeyboardKeys();
	lua_newtable(ls);
	for(unsigned i = 0; i < 350; ++i) {
		lua_pushinteger(ls, i);
		lua_pushinteger(ls, keys[i]);
		lua_settable(ls, -3);
	}
	return 1;
}

static int lua_getObjectData(lua_State* ls) {
	if(lua_gettop(ls) < 1)
		return 0;
	int id = lua_tonumber(ls, 1);
	
	lua_newtable(ls);
	game->getEntity(id)->insert(ls);
	return 1;
}

static int lua_updateObjectData(lua_State* ls) {
	int id = lua_tonumber(ls, 1);
	if( id == -1) {
		game->retrieveData(ls);
	}else {
		game->getEntity(id)->retrieve(ls);
	}
	return 0;
}

static int lua_addForce(lua_State* ls) {
	if(lua_gettop(ls) < 4)
		return 0;
	int id = lua_tonumber(ls, 1);
	PhysicsComponent* phys = (PhysicsComponent*)game->getEntity(id)->getComponent("physics");
	phys->body->applyForce(btVector3(lua_tonumber(ls, 2), lua_tonumber(ls, 3), lua_tonumber(ls, 4)), btVector3(0, 0, 0));
	return 0;
}

static int lua_setActiveProgram(lua_State* ls) {
	std::string tex = lua_tostring(ls, 1);
	glUseProgram(game->getAssetManager()->getProgram(tex));
	return 0;
}

static int lua_foreach(lua_State* ls) {
	game->foreach(lua_tointeger(ls, 1), lua_tostring(ls, 2));
	return 0;
}

static int lua_setUniformProgramData(lua_State* ls) {
	lua_setActiveProgram(ls);
	int arg_count = lua_gettop(ls);
	std::string program = lua_tostring(ls, 1);
	const char* uniform = lua_tostring(ls, 2);
	GLint location = glGetUniformLocation(game->getAssetManager()->getProgram(program), uniform);
	switch(arg_count - 2) {
		case 1:
			glUniform1f(location, lua_tonumber(ls, 3));
		break;
		case 2:
			glUniform2f(location, lua_tonumber(ls, 3), lua_tonumber(ls, 4));
		break;
		case 3:
			glUniform3f(location, lua_tonumber(ls, 3), lua_tonumber(ls, 4), lua_tonumber(ls, 5));
		break;
		case 4:
			glUniform4f(location, lua_tonumber(ls, 3), lua_tonumber(ls, 4), lua_tonumber(ls, 5), lua_tonumber(ls, 6));
		break;
	}
	return 0;
}

static const luaL_reg lua_game_functions[] = {
	{"getFPS", lua_getGameFPS},
	{"generateID", lua_generateID},
	{"addComponent", lua_addComponent},
	{"addTween", lua_addTween},
	{"drawText", lua_drawText},
	{"drawRect", lua_drawRect},
	{"getMouseData", lua_getMouseData},
	{"setScript", lua_setScript},
	{"getObjectData", lua_getObjectData},
	{"updateObjectData", lua_updateObjectData},
	{"playSound", lua_playSound},
	{"getTexture", lua_getTexture},
	{"getSound", lua_getSound},
	{"getShader", lua_getShader},
	{"getProgram", lua_getProgram},
	{"loadTexture", lua_loadTexture},
	{"loadFont", lua_loadFont},
	{"loadSound", lua_loadSound},
	{"loadShader", lua_loadShader},
	{"createProgram", lua_createProgram},
	{"addForce", lua_addForce},
	{"setUniformProgramData", lua_setUniformProgramData},
	{"setActiveProgram", lua_setActiveProgram},
	{"getKeyboardData", lua_getKeyboardData},
	{"clearAll", lua_clearAll},
	{"clearTweens", lua_clearTweens},
	{"foreach", lua_foreach},
	{0, 0}
};

}
#endif
