#include "Box.h"

using namespace df;

Box::~Box() {
	for(int i = 0; i < size; ++i) {
		children[i]->setActive(false);
		children[i]->owner = NULL;
	}
}

void Box::addChild(Widget* child) {
	Widget** new_children = new Widget*[size + 1];
	
	for(int i = 0; i < size; ++i)
		new_children[i] = children[i];
	new_children[size] = child;
	
	children = new_children;
	child->owner = this;
	if(size == 0 && is_active)
		child->setActive(true);
	size++;
}

void Box::removeChild(Widget* child) {
	for(int i = 0; i < size; ++i) {
		if(children[i] != child)
			continue;
		children[i]->setActive(false);
		children[i]->owner = NULL;
		for(int j = i + 1; j < size; ++j)
			children[j - 1] = children[j];
		--i;
		--size;
	}
}

Vec2d Box::calculatePosition(Widget* widget) {
	if(size == 0)
		return Vec2d();
	
	Vec2d self_position = requestPosition();
	Vec2d self_size = requestSize();
	int wp = 0;
	
	for(int i = 0; i < size; ++i)
		if(children[i] == widget)
			wp = i;
	
	if(widget_orient == ORIENTATION_HORIZONTAL) {
		self_size.x /= size;
		self_position.x += self_size.x * wp;
	}else{
		self_size.y /= size;
		self_position.y -= self_size.y * wp;
	}
	
	return self_position;
}

Vec2d Box::calculateSize(Widget*) {
	if(size == 0)
		return Vec2d();
	
	Vec2d self_size = requestSize();
	
	if(widget_orient == ORIENTATION_HORIZONTAL)
		self_size.x /= size;
	else
		self_size.y /= size;
	
	return self_size;
}

void Box::run(float delta_time) {
	for(int i = 0; i < size; ++i) {
		children[i]->run(delta_time);
	}
}

void Box::keyEvent(int key, int scancode, int action, int modifiers) {
	for(int i = 0; i < size; ++i) {
		if(children[i]->getActive()) {
			children[i]->keyEvent(key, scancode, action, modifiers);
			if(children[i]->getShouldSwitch()) {
				children[i]->setShouldSwitch(false);
				children[i]->setActive(false);
				if(i + 1 < size) {
					children[i + 1]->setActive(true);
				}else {
					setShouldSwitch(true);
				}
			}
			return;
		}
	}
	if(size > 0) {
		should_switch = false;
		children[0]->setActive(true);
	}
}

void Box::mouseMoveEvent(Vec2d position) {
	if(is_active) {
		for(int i = 0; i < size; ++i)
			children[i]->mouseMoveEvent(position);
	}
}

void Box::mouseEvent(Vec2d position, int button, int action, int modifiers) {
	Widget::mouseEvent(position, button, action, modifiers);
	if(is_active) {
		for(int i = 0; i < size; ++i)
			children[i]->mouseEvent(position, button, action, modifiers);
	} else {
		for(int i = 0; i < size; ++i)
			children[i]->setActive(false);
	}
}
