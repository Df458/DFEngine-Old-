#ifndef DF_VIEWPORT
#define DF_VIEWPORT
#include "Util.h"
#include "Widget.h"
#include "Game.h"

namespace df {

class Viewport : public Widget {
public:
	Viewport();
	~Viewport() {}
	void run(float delta_time);
	void keyEvent(int key, int scancode, int action, int modifiers);
	void mouseEvent(Vec2d position, int button, int action, int	modifiers);
	void mouseMoveEvent(Vec2d position);
	
protected:
	Vec3d translation;
	Vec3d rotation;
	Vec3d scale;
};

}
#endif
