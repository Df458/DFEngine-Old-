#ifndef DF_VIEWPORT
#define DF_VIEWPORT
#include "Util.h"
#include "Widget.h"
#include "UnorderedContainer.h"

namespace df {

class Viewport : public UnorderedContainer {
public:
	Viewport();
	~Viewport() {}
	void run(float delta_time);
	void keyEvent(int key, int scancode, int action, int modifiers);
	void mouseEvent(Vec2d position, int button, int action, int	modifiers);
	void mouseMoveEvent(Vec2d position);
	Widget* getChild(int);
	Vec2d calculatePosition(Widget*);
	Vec2d calculateSize(Widget*);
	
protected:
	Vec3d translation;
	Vec3d rotation;
	Vec3d scale;
	std::map<Vec2d, Widget*> children;
};

}
#endif
