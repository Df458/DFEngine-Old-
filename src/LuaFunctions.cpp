#include "LuaFunctions.h"
#include "Game.h"
#include "AssetManager.h"
#include "Font.h"
#include "Tween.h"
#include "Component.h"

using namespace df;

int lua_getGameFPS(lua_State* ls) {
	lua_pushinteger(ls, game->getFPS());
	return 1;
}

int lua_loadSound(lua_State* ls) {
	game->getAssetManager()->loadSound(lua_tostring(ls, 1));
	lua_pushinteger(ls, game->getAssetManager()->getSound(lua_tostring(ls, 1)));
	return 1;
}

int lua_loadFont(lua_State* ls) {
	game->getAssetManager()->loadFont(lua_tostring(ls, 1), Vec2d(lua_tonumber(ls, 2), lua_tonumber(ls, 3)));
	return 0;
}

int lua_loadTexture(lua_State* ls) {
	game->getAssetManager()->loadTexture(lua_tostring(ls, 1));
	lua_pushinteger(ls, game->getAssetManager()->getTexture(lua_tostring(ls, 1)));
	return 1;
}

int lua_loadShader(lua_State* ls) {
	game->getAssetManager()->loadShader(lua_tostring(ls, 1));
	return 0;
}

int lua_loadModel(lua_State* ls) {
	game->getAssetManager()->loadModel(lua_tostring(ls, 1));
	return 0;
}

int lua_createProgram(lua_State* ls) {
	int arg_count = lua_gettop(ls);
	std::string program_id = lua_tostring(ls, 1);
	std::vector<std::string>vec;
	for(int i = 2; i <= arg_count; ++i) {
		vec.push_back(lua_tostring(ls, i));
	}
	
	game->getAssetManager()->compileProgram(program_id, vec);	
	return 0;
}

int lua_clearTweens(lua_State* ls) {
	game->clearTweens();
	return 0;
}

int lua_drawText(lua_State* ls) {
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

int lua_generateID(lua_State* ls) {
	lua_pushinteger(ls, game->generateId());
	return 1;
}

int lua_addComponent(lua_State* ls) {
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
		
		case COMPONENT_GRAPHICS: {
			GraphicsComponent* cmp_dat = new GraphicsComponent;
			if(arg_count >= 3)
				cmp_dat->model = game->getAssetManager()->getModel(lua_tostring(ls, 3));
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

int lua_setScript(lua_State* ls) {
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

int lua_playSound(lua_State* ls) {
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

int lua_getTexture(lua_State* ls) {
	std::string tex = lua_tostring(ls, 1);
	lua_pushinteger(ls, game->getAssetManager()->getTexture(tex));
	return 1;
}

int lua_getSound(lua_State* ls) {
	std::string tex = lua_tostring(ls, 1);
	lua_pushinteger(ls, game->getAssetManager()->getSound(tex));
	return 1;
}

int lua_getShader(lua_State* ls) {
	std::string tex = lua_tostring(ls, 1);
	lua_pushinteger(ls, game->getAssetManager()->getShader(tex));
	return 1;
}

int lua_getProgram(lua_State* ls) {
	std::string tex = lua_tostring(ls, 1);
	lua_pushinteger(ls, game->getAssetManager()->getProgram(tex));
	return 1;
}

int lua_clearAll(lua_State* ls) {
	game->killAll();
	return 0;
}

int lua_addTween(lua_State* ls) {
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

int lua_getMouseData(lua_State* ls) {
	Vec2d pos = game->getMousePosition();
	unsigned char* mb = game->getMouseButtons();
	lua_pushnumber(ls, pos.x);
	lua_pushnumber(ls, pos.y);
	lua_pushinteger(ls, mb[0]);
	lua_pushinteger(ls, mb[1]);
	lua_pushinteger(ls, mb[2]);
	return 5;
}

int lua_getKeyboardData(lua_State* ls) {
	unsigned char* keys = game->getKeyboardKeys();
	lua_newtable(ls);
	for(unsigned i = 0; i < 350; ++i) {
		lua_pushinteger(ls, i);
		lua_pushinteger(ls, keys[i]);
		lua_settable(ls, -3);
	}
	return 1;
}

int lua_getObjectData(lua_State* ls) {
	if(lua_gettop(ls) < 1)
		return 0;
	int id = lua_tonumber(ls, 1);
	
	lua_newtable(ls);
	game->getEntity(id)->insert(ls);
	return 1;
}

int lua_updateObjectData(lua_State* ls) {
	int id = lua_tonumber(ls, 1);
	if( id == -1) {
		game->retrieveData(ls);
	}else {
		game->getEntity(id)->retrieve(ls);
	}
	return 0;
}

int lua_addForce(lua_State* ls) {
	if(lua_gettop(ls) < 4)
		return 0;
	int id = lua_tonumber(ls, 1);
	PhysicsComponent* phys = (PhysicsComponent*)game->getEntity(id)->getComponent("physics");
	phys->body->applyForce(btVector3(lua_tonumber(ls, 2), lua_tonumber(ls, 3), lua_tonumber(ls, 4)), btVector3(0, 0, 0));
	return 0;
}

int lua_setActiveProgram(lua_State* ls) {
	std::string tex = lua_tostring(ls, 1);
	glUseProgram(game->getAssetManager()->getProgram(tex));
	return 0;
}

int lua_foreach(lua_State* ls) {
	game->foreach(lua_tointeger(ls, 1), lua_tostring(ls, 2));
	return 0;
}

int lua_setUniformProgramData(lua_State* ls) {
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
