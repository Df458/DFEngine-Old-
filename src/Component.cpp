#include "Component.h"
#include "Game.h"

using namespace df;

GraphicsComponent::GraphicsComponent() {
	color = Vec3d(1, 1, 1);
	program = game->getAssetManager()->getProgram("default");
	uv = game->getAssetManager()->getDefaultUV();
	model = game->getAssetManager()->getModel("default");
	texture = game->getAssetManager()->getTexture("default");
}

void GraphicsComponent::_runSub(float delta_time) {
	Component* cmp = game->getComponent(_owner_id, component_type_str[COMPONENT_PHYSICS]);
	if(!cmp)
		return;

	PhysicsComponent* transforms = dynamic_cast<PhysicsComponent*>(cmp);
	btMotionState* state = transforms->body->getMotionState();
	btTransform trans;
	float gltransform[16];
	state->getWorldTransform(trans);
	
	glm::mat4 model_transform = 
	glm::translate(glm::mat4(1.0f), glm::vec3(trans.getOrigin().getX() * 10, trans.getOrigin().getY() * 10, trans.getOrigin().getZ() * 10)) *
	glm::rotate(glm::mat4(1.0f), trans.getRotation().getAngle(), glm::vec3(trans.getRotation().getAxis().getX(), trans.getRotation().getAxis().getY(), trans.getRotation().getAxis().getZ())) *
	glm::scale(glm::mat4(1.0f), glm::vec3(scale.x, scale.y, scale.z));
	glm::mat4 view_transform = game->getCameraView();
	glm::mat4 projection_transform = game->getCameraProjection();
	glm::mat4 mvp_matrix = projection_transform * view_transform * model_transform;
	glm::vec4 blend_color = {color.x, color.y, color.z, alpha};
	
	glUseProgram(program);
	GLuint vertex_attr_pos = glGetAttribLocation(program, "vertex_pos");
	GLuint uv_attr_pos = glGetAttribLocation(program, "uv_out");
	GLuint mvp_uniform_pos = glGetUniformLocation(program, "model_view_projection");
	GLuint texture_uniform_pos  = glGetUniformLocation(program, "tex");
	GLuint color_uniform_pos = glGetUniformLocation(program, "color");
	
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);
	glUniform1i(texture_uniform_pos, 0);
	glUniformMatrix4fv(mvp_uniform_pos, 1, GL_FALSE, &mvp_matrix[0][0]);
	glUniform4fv(color_uniform_pos, 1, (GLfloat*)&blend_color);
	
	glEnableVertexAttribArray(vertex_attr_pos);
	glBindBuffer(GL_ARRAY_BUFFER, model.vertex_buffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, model.index_buffer);
	glVertexAttribPointer(vertex_attr_pos, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glEnableVertexAttribArray(uv_attr_pos);
	glBindBuffer(GL_ARRAY_BUFFER, uv);
	glVertexAttribPointer(uv_attr_pos, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glDrawElements(GL_TRIANGLES, model.index_count, GL_UNSIGNED_INT, 0);

	glDisableVertexAttribArray(vertex_attr_pos);
	glDisableVertexAttribArray(uv_attr_pos);
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
