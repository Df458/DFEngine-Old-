#ifndef DF_SINGLE_CONTAINER
#define DF_SINGLE_CONTAINER
#include "Util.h"
#include "Container.h"

namespace df {

class SingleContainer : public Container{
public:
	~SingleContainer() { if(child) { removeChild(child); } }
	virtual Widget* getChild() = 0;
	virtual void addChild(Widget* new_child) { child = new_child; child->owner = this; if(is_active) new_child->setActive(true); }
	virtual void removeChild(Widget* new_child) {
		if(new_child != child || child == NULL)
			return;
		child->owner = NULL;
		child->setActive(false);
		child = NULL;
	}
	virtual Vec2d calculatePosition(Widget*) { return Vec2d(0, requestSize().y); }
	virtual Vec2d calculateSize(Widget*) { return requestSize(); }
	virtual void run(float delta_time) { if(child) child->run(delta_time); }
	virtual void keyEvent(int key, int scancode, int action, int modifiers) {
		if(child) child->keyEvent(key, scancode, action, modifiers);
	}
	virtual void mouseEvent(Vec2d position, int button, int action, int	modifiers) {
		if(child) child->mouseEvent(position, button, action, modifiers);
	}
	virtual void mouseMoveEvent(Vec2d position) {
		if(child) child->mouseMoveEvent(position);
	}
	void setActive(bool active) { if(child) child->setActive(active); }
protected:
	Widget* child = NULL;
};

}
#endif
