#include "Component.h"
#include "Game.h"

using namespace df;

void RectGraphicsComponent::_runSub(float delta_time) {
	glUseProgram(program);
	Component* cmp = game->getComponent(_owner_id, component_type_str[COMPONENT_PHYSICS]);
	if(!cmp)
		return;
	PhysicsComponent* transforms = dynamic_cast<PhysicsComponent*>(cmp);
	btMotionState* state = transforms->body->getMotionState();
	btTransform trans;
	float gltransform[16];
	state->getWorldTransform(trans);
	trans.getOpenGLMatrix(gltransform);
	for(int i = 12; i < 15; ++i)
		gltransform[i] *= 10;
	
	if(!has_texture)
		glDisable(GL_TEXTURE_2D);
	else {
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, texture_id);
	}

	glPushMatrix();
	//glTranslatef(trans.getOrigin().getX() * 10.0f, trans.getOrigin().getY() * 10.0f, trans.getOrigin().getZ() * 10.0f);
	//glRotatef(trans.getRotation().x(), trans.getRotation().y(), trans.getRotation().z(), trans.getRotation().w());
	//glRotatef(_transforms->rotation.x, 1, 0, 0);
	//glRotatef(_transforms->rotation.y, 0, 1, 0);
	//glRotatef(_transforms->rotation.z, 0, 0, 1);
	//glScalef(_transforms->scale.x, _transforms->scale.y, _transforms->scale.z);
	glMultMatrixf(gltransform);
	
	glColor4f(color.x, color.y, color.z, alpha);
	drawGlRect(dimensions.x, dimensions.y);
	/*glBegin(GL_QUADS);
		glTexCoord2f(0, 1); glVertex3f(-dimensions.x / 2,-dimensions.y / 2,0);
		glTexCoord2f(1, 1); glVertex3f(dimensions.x / 2,-dimensions.y / 2,0);
		glTexCoord2f(1, 0); glVertex3f(dimensions.x / 2,dimensions.y / 2,0);
		glTexCoord2f(0, 0); glVertex3f(-dimensions.x / 2,dimensions.y / 2,0);
	glEnd();*/
	glPopMatrix();
}

void TimerComponent::_runSub(float delta_time) {
	for(auto i = _timers.begin(); i != _timers.end(); ++i) {
		if(i->second.time < 0 || i->second.func.empty())
			continue;
		
		i->second.time -= delta_time / FPS;
		if(i->second.time <= 0) {
			i->second.time = -1;
			lua_State* ls = game->getState();
			//ls = luaL_newstate();
			//luaL_openlibs(ls);
			//luaL_openlib(ls, "game", lua_game_functions, 0);
				
			Entity* owner = game->getEntity(_owner_id);
				
			lua_newtable(ls);
			owner->insert(ls);
			lua_setglobal(ls, "this");
			game->insertData(ls);
			lua_insertpath(ls);
			luaL_loadfile(ls,(getPath() + "/data/scripts/" + i->second.func).c_str());
			if(lua_pcall(ls, 0, LUA_MULTRET, 0))
				std::cerr << lua_tostring(ls, -1) << "\n";
			lua_getglobal(ls, "this");
			owner->retrieve(ls);
			game->retrieveData(ls);
		}
	}
}

void TimerComponent::_insertDataSub(lua_State* ls) {
		lua_newtable(ls);
		for(auto i : _timers) {
			lua_pushinteger(ls, i.first);
			lua_newtable(ls);
			lua_pushnumber(ls, i.second.time);
			lua_setfield(ls, -2, "time");
			lua_pushstring(ls, i.second.func.c_str());
			lua_setfield(ls, -2, "script");
			lua_settable(ls, -3);
		}
		lua_setfield(ls, -2, "timers");
	}
void TimerComponent::_retrieveDataSub(lua_State* ls) {
		lua_getfield(ls, -1, "timers");
		lua_pushnil(ls);
		while(lua_next(ls, -2)) {
			lua_getfield(ls, -1, "time");
			lua_getfield(ls, -2, "script");
			_timers[lua_tointeger(ls, -4)].time = lua_tonumber(ls, -2);
			_timers[lua_tointeger(ls, -4)].func = lua_tostring(ls, -1);
			lua_pop(ls, 3);
		}
		lua_pop(ls, 1);
	}
