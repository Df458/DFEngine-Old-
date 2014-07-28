#include "Font.h"

using namespace df;

Font::Font(std::string id, Vec2d dimensions, FT_Face font_face) {
	//Set the font face parameters
	FT_Set_Pixel_Sizes(font_face, dimensions.x, dimensions.y);
	
	for(int i = 0; i < 256; ++i) {
		//Prepare the Freetype glyph
		FT_Glyph extract_glyph;
		FT_UInt index = FT_Get_Char_Index(font_face, i);
		if(FT_Load_Glyph(font_face, index, FT_LOAD_DEFAULT) || FT_Get_Glyph(font_face->glyph, &extract_glyph))
			continue;
		FT_Vector glyph_origin;
		glyph_origin.x = 0;
		glyph_origin.y = 0;
		FT_Glyph_Metrics metrics = font_face->glyph->metrics;
		
		//Extract the glyph's bitmap data
		if(FT_Glyph_To_Bitmap(&extract_glyph, FT_RENDER_MODE_NORMAL, &glyph_origin, 1))
			continue;
		FT_BitmapGlyph bitmap_glyph = (FT_BitmapGlyph)extract_glyph;
		FT_Bitmap& bitmap = bitmap_glyph->bitmap;
		int width = bitmap.width;
		int height = bitmap.rows;
		
		//Create a new glyph
		Glyph g;
		g.id = i;
		g.dimensions.x = width;
		g.dimensions.y = height;
		g.bearing.x = metrics.horiBearingX / 64;
		g.bearing.y = metrics.horiBearingY / 64;
		g.advance = metrics.horiAdvance / 64;
		glGenTextures(1, &g.texture);
		
		//Create the texture
		GLubyte* tex_data = new GLubyte[2 * width * height];
		for(int y = 0; y < height; ++y) {
			for(int x = 0; x < width; x++) {
				tex_data[2 * (x + y * width)]     = bitmap.buffer[x + y * bitmap.pitch];
				tex_data[2 * (x + y * width) + 1] = bitmap.buffer[x + y * bitmap.pitch];
			}
		}
		glBindTexture( GL_TEXTURE_2D, g.texture);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
		glPixelStorei(GL_UNPACK_ALIGNMENT,1);
		glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_LUMINANCE_ALPHA, GL_UNSIGNED_BYTE, tex_data);
		
		//Add the new glyph to the list, and clean up
		glyphs[i] = g;
		_dimensions.x = width;
		_dimensions.y = height;
		delete [] tex_data;
	}
}

void Font::cleanup() {
	for(auto i : glyphs) {
		glDeleteTextures(1, &i.second.texture);
	}
}

void Font::draw(std::string text, Vec3d position, Vec3d rotation, Vec3d scale, Vec3d color, float alpha) {
	glEnable(GL_TEXTURE_2D);
	
	glPushMatrix();
	glTranslatef(position.x, position.y, position.z);
	glRotatef(rotation.x, 1, 0, 0);
	glRotatef(rotation.y, 0, 1, 0);
	glRotatef(rotation.z, 0, 0, 1);
	glScalef(scale.x, scale.y, scale.z);
	glColor4f(color.x, color.y, color.z, alpha);
	
	Vec2d pos;
	pos.y = _dimensions.y;
	for(unsigned i = 0; i < text.size(); ++i) {
		if(text[i] == '\n') {
			pos.x = 0;
			pos.y += _dimensions.y;
			continue;
		} else if(text[i] == ' ' || glyphs.find(text[i]) == glyphs.end()) {
			pos.x += _dimensions.x;
			continue;
		}
		
		glBindTexture(GL_TEXTURE_2D, glyphs[text[i]].texture);
		glBegin(GL_QUADS);
			glTexCoord2f(0, 0); glVertex2f(glyphs[text[i]].bearing.x + pos.x, pos.y - glyphs[text[i]].bearing.y);
			glTexCoord2f(1, 0); glVertex2f(glyphs[text[i]].bearing.x + pos.x + glyphs[text[i]].dimensions.x, pos.y - glyphs[text[i]].bearing.y);
			glTexCoord2f(1, 1); glVertex2f(glyphs[text[i]].bearing.x + pos.x + glyphs[text[i]].dimensions.x, pos.y - glyphs[text[i]].bearing.y + glyphs[text[i]].dimensions.y);
			glTexCoord2f(0, 1); glVertex2f(glyphs[text[i]].bearing.x + pos.x, pos.y - glyphs[text[i]].bearing.y + glyphs[text[i]].dimensions.y);
		glEnd();

		pos.x += glyphs[text[i]].advance;
	}
	glPopMatrix();
}
