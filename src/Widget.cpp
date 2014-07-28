#include "Widget.h"
#include "Container.h"

using namespace df;

Widget::~Widget() {
	if(owner)
		owner->removeChild(this);
}

Vec2d Widget::requestPosition() {
	if(!owner)
		return Vec2d();
	
	return owner->calculatePosition(this);
}

Vec2d Widget::requestSize() {
	if(!owner)
		return preferred_size;
	
	Vec2d final_size = owner->calculateSize(this);
	final_size.x = final_size.x < preferred_size.x || fill ? final_size.x : preferred_size.x;
	final_size.y = final_size.y < preferred_size.y || fill ? final_size.y : preferred_size.y;
	
	return final_size;
}
