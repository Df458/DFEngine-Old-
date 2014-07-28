#ifndef VEC2D_H
#define VEC2D_H
class Vec2d {
public:
	Vec2d() {}
	Vec2d(float n_x, float n_y) { x = n_x; y = n_y; }
	float x = 0;
	float y = 0;
	
	float* getXRef() { return &x; }
	float* getYRef() { return &y; }
	
	Vec2d operator+(Vec2d rvalue)  { return Vec2d(x + rvalue.x, y + rvalue.y); }
	void  operator+=(Vec2d rvalue) { x += rvalue.x; y += rvalue.y; }
	Vec2d operator-(Vec2d rvalue)  { return Vec2d(x - rvalue.x, y - rvalue.y); }
	void  operator-=(Vec2d rvalue) { x -= rvalue.x; y -= rvalue.y; }
};
#endif
