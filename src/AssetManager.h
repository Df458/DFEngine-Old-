#ifndef DF_ASSET_MANAGER
#define DF_ASSET_MANAGER
#include "Util.h"
#include "Font.h"
#include <inttypes.h>

namespace df {
class AssetManager {
public:
	AssetManager();
	~AssetManager();
	
	Font* getFont(std::string id) 	  { return &fonts[id];   }
	ALuint getSound(std::string id)   { return sounds[id];   }
	GLuint getTexture(std::string id) { return textures[id]; }
	GLuint getShader(std::string id)  { return shaders[id];  }
	GLuint getProgram(std::string id) { return programs[id]; }

	void loadFont(std::string path, Vec2d size);
	void loadSound(std::string path);
	void loadTexture(std::string path);
	void loadShader(std::string path);
	void compileProgram(std::string id, std::vector<std::string> shader_ids);
	
	void cleanup();
protected:
	std::map<std::string, Font>   fonts;
	std::map<std::string, ALuint> sounds;
	std::map<std::string, GLuint> textures;
	std::map<std::string, GLuint> shaders;
	std::map<std::string, GLuint> programs;
	
	FT_Library freetype_library;
};
}
#endif
