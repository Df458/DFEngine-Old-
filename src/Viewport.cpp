#include "Viewport.h"
#include "Model.h"
#include "Game.h"

using namespace df;

Viewport::Viewport() {
	preferred_size = Vec2d(800, 600);
	scale = Vec3d(1, 1, 1);
	fill = false;
}

void Viewport::run(float delta_time) {
	Vec2d self_position, self_size;
	
	self_position = requestPosition();
	self_size = requestSize();

	glClear(GL_DEPTH_BUFFER_BIT);
	glViewport (self_position.x, self_position.y-self_size.y, self_size.x, self_size.y);
	glLoadIdentity();
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	float ratio = self_size.x / self_size.y;
	//float viewscale = self_size.x / game->getViewSize().x < self_size.y / game->getViewSize().y ? self_size.x / game->getViewSize().x : self_size.y / game->getViewSize().y;
	Vec2d fill = game->getViewSize();
	ratio /= fill.x / fill.y;
	if(ratio >= 1)
		fill.x *= ratio;
	if(ratio <= 1)
		fill.y /= ratio;
	
	Vec2d fillsize = Vec2d(game->getViewSize().x, game->getViewSize().y);
	
	translation = game->getCameraTranslate();
	rotation = game->getCameraRotate();
	scale = game->getCameraScale();
	
	glm::mat4 projection = glm::ortho(0.0f, fill.x, fill.y, 0.0f, -100.0f, 100.0f);
	glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(translation.x + (fill.x - fillsize.x) / 2, translation.y + (fill.y - fillsize.y) / 2, translation.z)) * 
	glm::rotate(glm::mat4(1.0f), rotation.x, glm::vec3(1.0f, 0.0f, 0.0f)) * 
	glm::rotate(glm::mat4(1.0f), rotation.y, glm::vec3(0.0f, 1.0f, 0.0f)) * 
	glm::rotate(glm::mat4(1.0f), rotation.z, glm::vec3(0.0f, 0.0f, 1.0f)) * 
	glm::scale(glm::mat4(1.0f), glm::vec3(scale.x, scale.y, scale.z));
	
	game->draw(delta_time, view, projection);

	glDisable(GL_DEPTH_TEST);

	Model m = game->getAssetManager()->getModel("default");
	GLuint program = game->getAssetManager()->getProgram("default");
	GLuint vertex_buffer = m.vertex_buffer;
	GLuint vertex_indices = m.index_buffer;
	glm::vec4 blend_color = {0.0f, 0.0f, 0.0f, 1.0f};
	GLuint texture = game->getAssetManager()->getTexture("default");

	GLuint vertex_attr_pos = glGetAttribLocation(program, "vertex_pos");
	GLuint mvp_uniform_pos = glGetUniformLocation(program, "model_view_projection");
	GLuint texture_uniform_pos  = glGetUniformLocation(program, "tex");

	glUseProgram(program);
	
	glm::mat4 border_transforms[4] = {
		projection * glm::scale(glm::mat4(1.0f), glm::vec3((fill.x - fillsize.x) / 2, fill.y, 1)),
		projection * glm::translate(glm::mat4(1.0f), glm::vec3(fill.x, 0.0f, 0.0f)) * glm::scale(glm::mat4(1.0f), glm::vec3((fill.x - fillsize.x) / 2, fill.y, 1)),
		projection * glm::scale(glm::mat4(1.0f), glm::vec3(fill.x, (fill.y - fillsize.y) / 2, 1)),
		projection * glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, fill.y, 0.0f)) * glm::scale(glm::mat4(1.0f), glm::vec3(fill.x, (fill.y - fillsize.y) / 2, 1)),
	};
	
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);
	glUniform1i(texture_uniform_pos, 0);
	glUniform4fv(glGetUniformLocation(program, "color"), 1, (GLfloat*)&blend_color);
	for(int i = 0; i < 4; ++i) {
		glUniformMatrix4fv(mvp_uniform_pos, 1, GL_FALSE, &border_transforms[i][0][0]);

		glEnableVertexAttribArray(vertex_attr_pos);
		glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vertex_indices);
		glVertexAttribPointer(vertex_attr_pos, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
		glDrawElements(GL_TRIANGLES, m.index_count, GL_UNSIGNED_INT, 0);
	}
	
}

void Viewport::keyEvent(int key, int scancode, int action, int modifiers) {
	Widget::keyEvent(key, scancode, action, modifiers);
	
	game->keyEvent(key, scancode, action, modifiers);
}

void Viewport::mouseEvent(Vec2d self_position, int button, int action, int modifiers) {
	Widget::mouseEvent(self_position, button, action, modifiers);
	if(!is_active)
		return;
	
	Vec2d self_size = requestSize();
	float viewscale = self_size.x / game->getViewSize().x < self_size.y / game->getViewSize().y ? self_size.x / game->getViewSize().x : self_size.y / game->getViewSize().y;
	Vec2d fill = Vec2d(game->getViewSize().x * viewscale, game->getViewSize().y * viewscale);

	self_position.x = clamp((self_position.x - (self_size.x - fill.x) / 2) / fill.x, 0, 1);
	self_position.y = clamp((self_position.y - (self_size.y - fill.y) / 2) / fill.y, 0, 1);
	game->mouseEvent(self_position, button, action, modifiers);
}

void Viewport::mouseMoveEvent(Vec2d self_position) {
	Widget::mouseMoveEvent(self_position);
	if(!is_active)
		return;
	
	//Size of the viewport in the window
	Vec2d self_size = requestSize();
	
	//Scale factor of the game view drawn in the viewport
	float viewscale = self_size.x / game->getViewSize().x < self_size.y / game->getViewSize().y ? self_size.x / game->getViewSize().x : self_size.y / game->getViewSize().y;
	
	//Size of the scaled-down game view in the viewport
	Vec2d fill = Vec2d(game->getViewSize().x * viewscale, game->getViewSize().y * viewscale);

	self_position.x = clamp((self_position.x - (self_size.x - fill.x) / 2) / fill.x, 0, 1);
	self_position.y = clamp((self_position.y - (self_size.y - fill.y) / 2) / fill.y, 0, 1);
	game->setMousePosition(self_position);
}
