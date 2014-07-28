#ifndef DF_SET_CONTAINER
#define DF_SET_CONTAINER
#include "Util.h"
#include "Container.h"

namespace df{

class SetContainer : public Container{
public:
	virtual Widget* getChild(int position) = 0;
	virtual Vec2d calculatePosition(Widget*) = 0;
	virtual Vec2d calculateSize(Widget*) = 0;
};

}
#endif
