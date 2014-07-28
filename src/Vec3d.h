#ifndef VEC3D_H
#define VEC3D_H
class Vec3d {
public:
	Vec3d() {}
	Vec3d(float n_x, float n_y, float n_z) { x = n_x; y = n_y; z = n_z; }
	float x = 0;
	float y = 0;
	float z = 0;
	
	float* getXRef() { return &x; }
	float* getYRef() { return &y; }
	float* getZRef() { return &z; }
	
	Vec3d operator+(Vec3d rvalue)  { return Vec3d(x + rvalue.x, y + rvalue.y, z + rvalue.z); }
	void  operator+=(Vec3d rvalue) { x += rvalue.x; y += rvalue.y; z += rvalue.z; }
	Vec3d operator-(Vec3d rvalue)  { return Vec3d(x - rvalue.x, y - rvalue.y, z - rvalue.z); }
	void  operator-=(Vec3d rvalue) { x -= rvalue.x; y -= rvalue.y; z += rvalue.z; }
};
#endif
