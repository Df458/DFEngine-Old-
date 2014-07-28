#ifndef DF_WIDGET
#define DF_WIDGET
#include "Util.h"

namespace df {

class Container;

class Widget{
public:
	virtual ~Widget();
	virtual void run(float delta_time) = 0;
	void setFill(bool new_fill) { fill = new_fill; }
	bool getFill() { return fill; }
	Vec2d getSize() { return preferred_size; }
	void setSize(Vec2d size) { preferred_size = size; }
	virtual void keyEvent(int key, int scancode, int action, int modifiers) { 
		is_active = true;
		if(key == GLFW_KEY_TAB && action == GLFW_PRESS)
			should_switch = true;
		}
	virtual void mouseEvent(Vec2d position, int button, int action, int	modifiers) {
		Vec2d pos = requestPosition();
		Vec2d siz = requestSize();
		if(position.x > pos.x && pos.y > position.y && position.x - siz.x < pos.x && pos.y - siz.y < position.y)
			is_active = true;
		else
			is_active = false;
	}
	virtual void mouseMoveEvent(Vec2d position) {}
	Container* owner = NULL; //DO NOT USE
	bool getShouldSwitch() { return should_switch; }
	void setShouldSwitch(bool sswitch) { should_switch = sswitch; }
	bool getActive() { return is_active; }
	virtual void setActive(bool active) { is_active = active; }

protected:
	bool fill = true;
	Vec2d preferred_size;
	bool is_active = false;
	bool should_switch = false;
	
	virtual Vec2d requestPosition();
	virtual Vec2d requestSize();
};

}
#endif
