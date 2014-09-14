#include "Util.h"
#include "Model.h"

std::string getPath(){
	char buf[1024];
#ifdef WINDOWS
    GetModuleFileName(NULL, buf, 1024);
    std::string::size_type pos = std::string(buf).find_last_of("\\/");
    return std::string(buf).substr(0, pos);
#elif __GNUC__
    ssize_t len = ::readlink("/proc/self/exe", buf, sizeof(buf)-1);
    if (len != -1) {
	buf[len] = '\0';
    std::string s = std::string(buf);
    return s.erase(s.rfind("/"));
    }else{
		std::cerr << "Error: Could not determine working directory\n";
		return "";
    }
#endif
}

void lua_insertpath(lua_State* ls) {
	lua_getglobal(ls, "package");
    lua_getfield(ls, -1, "path");
    std::string path = lua_tostring(ls, -1);
    path = getPath() + "/data/scripts/?.lua;" + path;
    lua_pop(ls, 1);
    lua_pushstring(ls, path.c_str());
    lua_setfield(ls, -2, "path");
    lua_pop(ls, 1);
}

void insertVec2d(lua_State* ls, std::string key, Vec2d v) {
	lua_newtable(ls);
	lua_pushnumber(ls, v.x);
	lua_setfield(ls, -2, "x");
	lua_pushnumber(ls, v.y);
	lua_setfield(ls, -2, "y");
	
	lua_setfield(ls, -2, key.c_str());
}

void insertVec3d(lua_State* ls, std::string key, Vec3d v) {
	lua_newtable(ls);
	lua_pushnumber(ls, v.x);
	lua_setfield(ls, -2, "x");
	lua_pushnumber(ls, v.y);
	lua_setfield(ls, -2, "y");
	lua_pushnumber(ls, v.z);
	lua_setfield(ls, -2, "z");
	
	lua_setfield(ls, -2, key.c_str());
}

void retrieveVec2d(lua_State* ls, std::string key, Vec2d& v) {
	lua_getfield(ls, -1, key.c_str());
	lua_getfield(ls, -1, "x");
	v.x = lua_tonumber(ls, -1);
	lua_getfield(ls, -2, "y");
	v.y = lua_tonumber(ls, -1);
	lua_pop(ls, 3);
}

void retrieveVec3d(lua_State* ls, std::string key, Vec3d& v) {
	lua_getfield(ls, -1, key.c_str());
	lua_getfield(ls, -1, "x");
	v.x = lua_tonumber(ls, -1);
	lua_getfield(ls, -2, "y");
	v.y = lua_tonumber(ls, -1);
	lua_getfield(ls, -3, "z");
	v.z = lua_tonumber(ls, -1);
	lua_pop(ls, 4);
}

float clamp(float val, float low, float high) {
	if(val < low)
		val = low;
	else if(val > high)
		val = high;
	return val;
}

void drawModel(df::Model* model, GLuint texture, GLuint program, glm::vec4 blend_color, glm::mat4 model_matrix, glm::mat4 view_matrix, glm::mat4 proj_matrix) {
	glm::mat4 mvp_matrix = proj_matrix * view_matrix * model_matrix;
	glEnable(GL_DEPTH_TEST);
	
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
	glBindBuffer(GL_ARRAY_BUFFER, model->vertex_buffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, model->index_buffer);
	glVertexAttribPointer(vertex_attr_pos, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glEnableVertexAttribArray(uv_attr_pos);
	glBindBuffer(GL_ARRAY_BUFFER, model->uv_buffer);
	glVertexAttribPointer(uv_attr_pos, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glDrawElements(GL_TRIANGLES, model->index_count, GL_UNSIGNED_INT, 0);

	glDisableVertexAttribArray(vertex_attr_pos);
	glDisableVertexAttribArray(uv_attr_pos);
}
