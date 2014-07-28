#ifndef DF_BOX
#define DF_BOX
#include "Util.h"
#include "SetContainer.h"

namespace df{

enum orientation { ORIENTATION_HORIZONTAL, ORIENTATION_VERTICAL };

class Box : public SetContainer {
public:
	~Box();
	Widget* getChild(int position) { if(position >= 0 && position < size) return children[position]; return NULL; }
	void addChild(Widget* child);
	void removeChild(Widget* child);
	orientation getOrientation() { return widget_orient; }
	void setOrientation(orientation orient) { widget_orient = orient; }
	virtual void run(float delta_time);
	virtual Vec2d calculatePosition(Widget*);
	virtual Vec2d calculateSize(Widget*);
	virtual void keyEvent(int key, int scancode, int action, int modifiers);
	virtual void mouseEvent(Vec2d position, int button, int action, int	modifiers);
	virtual void mouseMoveEvent(Vec2d position);
	void setActive(bool active) { if(!is_active && active && size > 0) children[0]->setActive(true); else for(int i = 0; i < size; ++i) children[i]->setActive(false); is_active = active; }
private:
	Widget** children;
	int size = 0;
	orientation widget_orient = ORIENTATION_VERTICAL;
};

}
#endif
