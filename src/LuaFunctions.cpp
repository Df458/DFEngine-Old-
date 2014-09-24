#include "LuaFunctions.h"
#include "Game.h"
#include "AssetManager.h"
#include "Font.h"
#include "Tween.h"
#include "Component.h"
#include "EntityMotionState.h"

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
	//game->clearTweens();
	fprintf(stderr, "ERROR: Tween system is no longer supported!\n");
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

int lua_drawModel(lua_State* ls) {
    glm::mat4 model_transform = 
    glm::translate(glm::mat4(1.0f), glm::vec3(lua_tonumber(ls, 4), lua_tonumber(ls, 5), lua_tonumber(ls, 6))) *
    //glm::rotate(glm::mat4(1.0f), (float)lua_tonumber(ls, 7), glm::vec3(lua_tonumber(ls, 8), lua_tonumber(ls, 9), lua_tonumber(ls, 10))) *
    glm::scale(glm::mat4(1.0f), glm::vec3(lua_tonumber(ls, 11), lua_tonumber(ls, 12), lua_tonumber(ls, 13)));
    glm::mat4 view_transform = game->getCameraView();
    glm::mat4 projection_transform = game->getCameraProjection();
    glm::vec4 blend_color = {1, 1, 1, 1};

    df::Model model = game->getAssetManager()->getModel(lua_tostring(ls, 1));
    drawModel(&model, game->getAssetManager()->getTexture(lua_tostring(ls, 2)), game->getAssetManager()->getProgram(lua_tostring(ls, 3)), blend_color, model_transform, view_transform, projection_transform);
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
	switch(component_type) {
	    case COMPONENT_PHYSICS: {
		btCollisionShape* shape = NULL;
		btVector3 translation(lua_tonumber(ls, 3), lua_tonumber(ls, 4), lua_tonumber(ls, 5));
		btQuaternion rotation(lua_tonumber(ls, 6), lua_tonumber(ls, 7), lua_tonumber(ls, 8), lua_tonumber(ls, 9));

		std::string shape_str = lua_tostring(ls, 10);
		unsigned char start = 0;
		if(shape_str == "Box") {
		    shape = new btBoxShape(btVector3(lua_tonumber(ls, 11), lua_tonumber(ls, 12), lua_tonumber(ls, 13)));
		    start = 14;
		} else if(shape_str == "Sphere") {
		    shape = new btSphereShape(lua_tonumber(ls, 11));
		    start = 12;
		} else if(shape_str == "Capsule") {
		    shape = new btCapsuleShape(lua_tonumber(ls, 11), lua_tonumber(ls, 12));
		    start = 13;
		} else if(shape_str == "Convex Mesh") {
		    Model input_model = game->getAssetManager()->getModel(lua_tostring(ls, 11));
		    btConvexHullShape* shape_build = new btConvexHullShape();
		    for(unsigned i = 0; i * 3 < input_model.vertex_count; ++i) {
			shape_build->addPoint(btVector3(input_model.vertex_buffer_raw[i * 3], input_model.vertex_buffer_raw[i * 3 + 1], input_model.vertex_buffer_raw[i * 3 + 2]));
		    }
		    btShapeHull* hull = new btShapeHull(shape_build);
		    btScalar margin = shape_build->getMargin();
		    hull->buildHull(margin);
		    shape = new btConvexHullShape((btScalar*)hull->getVertexPointer(), hull->numVertices());
		    delete shape_build;
		    start = 12;
		} else if(shape_str == "Tilemap") {
//:TODO: 24.09.14 11:12:40, Hugues Ross
// Implement this to take in a tilemap and generate solids from it
		}
		if(!shape)
		    break;

		btScalar mass = 1;
		if(arg_count >= start)
		    mass = lua_tonumber(ls, start);
		btVector3 inertia(0, 0, 0);
		shape->calculateLocalInertia(mass, inertia);

		EntityMotionState* motion_state = new EntityMotionState(btTransform(rotation, translation));

		btRigidBody::btRigidBodyConstructionInfo c_info(mass, motion_state, shape, inertia);
		btRigidBody* body = new btRigidBody(c_info);
		PhysicsComponent* cmp_dat = new PhysicsComponent(body);
		cmp_dat->collision_mask = 1;
		cmp_dat->collision_hit  = 1;
		cmp_dat->motion_state = motion_state;
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
	else if(script_type == "enter")
		en->setEnterScr(script_value);
	else if(script_type == "collide")
		en->setColScr(script_value);
	else if(script_type == "leave")
		en->setLeaveScr(script_value);
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

int lua_getRaycast(lua_State* ls) {
    short mask = 0;
    short hits = 0;
    if(lua_gettop(ls) >= 7)
	mask = lua_tointeger(ls, 7);
    if(lua_gettop(ls) >= 8)
	hits = lua_tointeger(ls, 8);
    lua_pushinteger(ls, game->getRaycast(Vec3d(lua_tointeger(ls, 1), lua_tointeger(ls, 2), lua_tointeger(ls, 3)), Vec3d(lua_tointeger(ls, 4), lua_tointeger(ls, 5), lua_tointeger(ls, 6)), mask, hits));
    return 1;
}

//:FIXME: 12.09.14 18:23:20, Hugues Ross
// This returns nothing
int lua_getMouseOver(lua_State* ls) {
    glm::mat4 inverse_matrix = glm::inverse(game->getCameraProjection() * game->getCameraView());
    Vec2d pos = game->getMousePosition();
    glm::vec4 ray_start_pos((pos.x - 0.5f) * 2, (pos.y - 0.5f) * 2, -1.0f, 1.0f);
    ray_start_pos = inverse_matrix * ray_start_pos;
    ray_start_pos /= ray_start_pos.w;
    glm::vec4 ray_end_pos((pos.x - 0.5f) * 2, (pos.y - 0.5f) * 2, 0.0f, 1.0f);
    ray_end_pos = inverse_matrix * ray_end_pos;
    ray_end_pos /= ray_end_pos.w;

    glm::vec3 final_ray(ray_end_pos - ray_start_pos);
    final_ray = glm::normalize(final_ray);

    short mask = 0;
    short hits = 0;
    Vec3d from(0, 0, 0); 
    Vec3d to(final_ray.x, final_ray.y, final_ray.z);
    if(lua_gettop(ls) >= 7)
	mask = lua_tointeger(ls, 7);
    if(lua_gettop(ls) >= 8)
	hits = lua_tointeger(ls, 8);
    lua_pushinteger(ls, game->getRaycast(from, to, mask, hits));
    return 1;
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
