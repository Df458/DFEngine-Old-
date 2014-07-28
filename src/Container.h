#ifndef DF_CONTAINER
#define DF_CONTAINER
#include "Util.h"
#include "Widget.h"

namespace df {

class Container : public Widget{
public:
	virtual void addChild(Widget* child) = 0;
	virtual void removeChild(Widget* child) = 0;
	virtual Vec2d calculatePosition(Widget*) = 0;
	virtual Vec2d calculateSize(Widget*) = 0;
};

}
#endif
