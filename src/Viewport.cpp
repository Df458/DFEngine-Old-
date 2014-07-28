#include "Viewport.h"

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

	glClear(GL_DEPTH_BUFFER_BIT); //Setup the viewport for drawing
	glViewport (self_position.x, self_position.y-self_size.y, self_size.x, self_size.y);
	glLoadIdentity();
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	float ratio = self_size.x / self_size.y;
	//float viewscale = self_size.x / game->getViewSize().x < self_size.y / game->getViewSize().y ? self_size.x / game->getViewSize().x : self_size.y / game->getViewSize().y;
	Vec2d fill = game->getViewSize();
	ratio /= fill.x / fill.y;
	if(ratio >= 1)
		fill.x *= ratio;
	if(ratio <= 1)
		fill.y /= ratio;
	glOrtho(0, fill.x, fill.y, 0, -1, 1);
	
	Vec2d fillsize = Vec2d(game->getViewSize().x, game->getViewSize().y);
	
	 
	glPushMatrix(); //Apply view transforms
	translation = game->getCameraTranslate();
	rotation = game->getCameraRotate();
	scale = game->getCameraScale();
	glTranslatef(translation.x + (fill.x - fillsize.x) / 2, translation.y + (fill.y - fillsize.y) / 2, translation.z);
	glRotatef(rotation.x, 1, 0, 0);
	glRotatef(rotation.y, 0, 1, 0);
	glRotatef(rotation.z, 0, 0, 1);
	glScalef(scale.x, scale.y, scale.z);
	
	game->draw(delta_time);

	glPopMatrix();
	glUseProgram(0);
	glColor4f(0,0,0,1);
	glDisable(GL_TEXTURE_2D);
	glBegin(GL_QUADS);
		glVertex2f(0, 0);
		glVertex2f((fill.x - fillsize.x) / 2, 0);
		glVertex2f((fill.x - fillsize.x) / 2, fill.y);
		glVertex2f(0, fill.y);
		
		glVertex2f(fill.x - ((fill.x - fillsize.x) / 2), 0);
		glVertex2f(fill.x, 0);
		glVertex2f(fill.x, fill.y);
		glVertex2f(fill.x - ((fill.x - fillsize.x) / 2), fill.y);
		
		glVertex2f(0, 0);
		glVertex2f(fill.x, 0);
		glVertex2f(fill.x, (fill.y - fillsize.y) / 2);
		glVertex2f(0, (fill.y - fillsize.y) / 2);
		
		glVertex2f(0, fill.y - (fill.y - fillsize.y) / 2);
		glVertex2f(fill.x, fill.y - (fill.y - fillsize.y) / 2);
		glVertex2f(fill.x, fill.y);
		glVertex2f(0, fill.y);
	glEnd();
	
	/*if(_is_active) {
		glLineWidth(3);
		glColor4f(0.7f, 0.7f, 0.7f, 1);
		glBegin(GL_LINE_LOOP);
			glVertex2f(0, 0);
			glVertex2f(size.x / (_preferred_size.x), 0);
			glVertex2f(size.x / (_preferred_size.x), size.y / (_preferred_size.y));
			glVertex2f(0, size.y / (_preferred_size.y));
		glEnd();
	}*/
}

void Viewport::keyEvent(int key, int scancode, int action, int modifiers) {
	Widget::keyEvent(key, scancode, action, modifiers);
	if(key == GLFW_KEY_KP_ADD && action == GLFW_PRESS) {
		game->addTween(Tween<float>(scale.getXRef(), scale.x + 0.5f, 0.5f, TWEEN_INTERP_SMOOTH_BOTH));
		game->addTween(Tween<float>(scale.getYRef(), scale.y + 0.5f, 0.5f, TWEEN_INTERP_SMOOTH_BOTH));
	} else if(key == GLFW_KEY_KP_SUBTRACT && action == GLFW_PRESS) {
		game->addTween(Tween<float>(scale.getXRef(), scale.x - 0.5f, 0.5f, TWEEN_INTERP_SMOOTH_BOTH));
		game->addTween(Tween<float>(scale.getYRef(), scale.y - 0.5f, 0.5f, TWEEN_INTERP_SMOOTH_BOTH));
	} else if(key == GLFW_KEY_EQUAL && action == GLFW_PRESS) {
		game->addTween(Tween<float>(scale.getXRef(), 1, 1, TWEEN_INTERP_SMOOTH_BOTH));
		game->addTween(Tween<float>(scale.getYRef(), 1, 1, TWEEN_INTERP_SMOOTH_BOTH));
	}
	
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
	
	Vec2d self_size = requestSize();
	float viewscale = self_size.x / game->getViewSize().x < self_size.y / game->getViewSize().y ? self_size.x / game->getViewSize().x : self_size.y / game->getViewSize().y;
	Vec2d fill = Vec2d(game->getViewSize().x * viewscale, game->getViewSize().y * viewscale);

	self_position.x = clamp((self_position.x - (self_size.x - fill.x) / 2) / fill.x, 0, 1);
	self_position.y = clamp((self_position.y - (self_size.y - fill.y) / 2) / fill.y, 0, 1);
	game->setMousePosition(self_position);
}
