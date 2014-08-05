#include "Font.h"
#include "Game.h"

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
	GLuint program = game->getAssetManager()->getProgram("default");
	glUseProgram(program);
	
	glm::mat4 global_transform = glm::translate(glm::mat4(1.0f), glm::vec3(position.x, position.y, position.z)) *
	glm::rotate(glm::mat4(1.0f), rotation.x, glm::vec3(1, 0, 0)) * glm::rotate(glm::mat4(1.0f), rotation.y, glm::vec3(0, 1, 0)) * glm::rotate(glm::mat4(1.0f), rotation.z, glm::vec3(0, 0, 1)) *
	glm::scale(glm::mat4(1.0f), glm::vec3(scale.x, scale.y, scale.z));
	glm::mat4 view_transform = game->getCameraView();
	glm::mat4 projection_transform = game->getCameraProjection();
	glm::vec4 blend_color = {color.x, color.y, color.z, alpha};
	
	GLuint vertex_attr_pos = glGetAttribLocation(program, "vertex_pos");
	GLuint uv_attr_pos = glGetAttribLocation(program, "uv_out");
	GLuint mvp_uniform_pos = glGetUniformLocation(program, "model_view_projection");
	GLuint texture_uniform_pos  = glGetUniformLocation(program, "tex");
	GLuint color_uniform_pos = glGetUniformLocation(program, "color");
	
	glEnableVertexAttribArray(vertex_attr_pos);
	Model m = game->getAssetManager()->getModel("default");
	glBindBuffer(GL_ARRAY_BUFFER, m.vertex_buffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m.index_buffer);
	glVertexAttribPointer(vertex_attr_pos, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glEnableVertexAttribArray(uv_attr_pos);
	glBindBuffer(GL_ARRAY_BUFFER, game->getAssetManager()->getDefaultUV());
	glVertexAttribPointer(uv_attr_pos, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
	
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
		glm::mat4 model_transform = glm::translate(glm::mat4(1.0f), glm::vec3(-glyphs[text[i]].bearing.x + pos.x, pos.y - glyphs[text[i]].bearing.y, 0)) *
		glm::scale(glm::mat4(1.0f), glm::vec3(glyphs[text[i]].dimensions.x / 2, glyphs[text[i]].dimensions.y / -2, 1));
		glm::mat4 mvp_matrix = projection_transform * view_transform * global_transform * model_transform;
		
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, glyphs[text[i]].texture);
		glUniform1i(texture_uniform_pos, 0);
		glUniformMatrix4fv(mvp_uniform_pos, 1, GL_FALSE, &mvp_matrix[0][0]);
		glUniform4fv(color_uniform_pos, 1, (GLfloat*)&blend_color);
		
		glDrawElements(GL_TRIANGLES, m.index_count, GL_UNSIGNED_INT, 0);
		
		pos.x += glyphs[text[i]].advance;
	}
	
	glDisableVertexAttribArray(vertex_attr_pos);
	glDisableVertexAttribArray(uv_attr_pos);
}
