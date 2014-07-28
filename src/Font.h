#ifndef DF_FONT
#define DF_FONT
#include "Util.h"

namespace df {

struct Glyph {
	char id;
	Vec2d dimensions;
	Vec2d bearing;
	float advance;
	GLuint texture;
};

class Font {
public:
	Font() { }
	void cleanup();
	Font(std::string id, Vec2d dimensions, FT_Face font_face);
	void draw(std::string text, Vec3d position = Vec3d(), Vec3d rotation = Vec3d(), Vec3d scale = Vec3d(1, 1, 1), Vec3d color = Vec3d(), float alpha = 1);
protected:
	std::string _id;
	Vec2d _dimensions;
	std::map<char, Glyph> glyphs;
};

}

#endif
