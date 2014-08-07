#ifndef DF_ASSET_MANAGER
#define DF_ASSET_MANAGER
#include "Util.h"
#include <inttypes.h>

namespace df {
struct Model;
struct Font;

class AssetManager {
public:
	AssetManager();
	~AssetManager();
	
	Font*  getFont(std::string id);
	ALuint getSound(std::string id);
	GLuint getTexture(std::string id);
	GLuint getShader(std::string id);
	GLuint getProgram(std::string id);
	GLuint getDefaultUV();
	Model  getModel(std::string id);

	void loadFont(std::string path, Vec2d size);
	void loadSound(std::string path);
	void loadTexture(std::string path);
	void loadShader(std::string path, bool frag = true);
	void compileProgram(std::string id, std::vector<std::string> shader_ids);
	void loadModel(std::string path);
	
	void cleanup();
protected:
	std::map<std::string, Font>   fonts;
	std::map<std::string, ALuint> sounds;
	std::map<std::string, GLuint> textures;
	std::map<std::string, GLuint> shaders;
	std::map<std::string, GLuint> programs;
	std::map<std::string, Model> models;
	
	FT_Library freetype_library;
	GLuint default_uv;
};
}
#endif
