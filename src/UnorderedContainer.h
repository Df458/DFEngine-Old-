#ifndef DF_UNORDERED_CONTAINER
#define DF_UNORDERED_CONTAINER
#include "Container.h"

namespace df {
    class UnorderedContainer : public Widget {
    public:
	UnorderedContainer(){}
	void addChild(Widget*);
	void removeChild(Widget*);
	Vec2d calculatePosition(Widget*);
	Vec2d calculateSize(Widget*);
    protected:

    };
}

#endif
