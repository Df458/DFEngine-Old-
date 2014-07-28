#include "AssetManager.h"

using namespace df;

AssetManager::AssetManager() {
	FT_Init_FreeType(&freetype_library);
}

AssetManager::~AssetManager() {
	cleanup();
}

void AssetManager::cleanup() {
	for(auto i : textures) {
		glDeleteTextures(1, &i.second);
	}
	for(auto i : sounds) {
		alDeleteSources(1, &i.second);
	}
	for(auto i : fonts) {
		i.second.cleanup();
	}
	
	FT_Done_FreeType(freetype_library);
}

// TODO Rewrite this
void AssetManager::loadTexture(std::string path) {
	std::string fname = path;
	path = getPath().append("/data/textures/" + path);
	
	GLuint texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	
	FILE* infile = fopen(path.c_str(), "rb");
	if(!infile) {
		std::cerr << "IO ERROR: Could not open file " + fname + ".\n";
		return;
	}
	
	uint8_t header[8];
	png_structp pstruct;
	png_infop info_struct;
	uint16_t width, height;
	png_byte* image_data;
	png_bytep* row_ptrs;
	
	fread(header, sizeof(uint8_t), 8, infile);
	if(png_sig_cmp(header, 0, 8)) {
		std::cerr << "IO ERROR: " + fname + " has an invalid header.\n";
		return;
	}
	pstruct = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	if(!pstruct) {
		std::cerr << "IO ERROR: Could not read structure of " + fname + ".\n";
		return;
	}
	info_struct = png_create_info_struct(pstruct);
	if(!info_struct) {
		png_destroy_read_struct(&pstruct, NULL, NULL);
		std::cerr << "IO ERROR: Could not create info_struct for " + fname + ".\n";
		return;
	}
	if(setjmp(png_jmpbuf(pstruct))) {
		return;
	}
	
	png_init_io(pstruct, infile);
	png_set_sig_bytes(pstruct, 8);
	png_read_info(pstruct, info_struct);
	
	width = png_get_image_width(pstruct, info_struct);
	height = png_get_image_height(pstruct, info_struct);
    png_byte color_type = png_get_color_type(pstruct, info_struct);
    png_byte bit_depth = png_get_bit_depth(pstruct, info_struct);
    int number_of_passes = png_set_interlace_handling(pstruct);
    
	if(color_type == PNG_COLOR_TYPE_RGB) {
		png_set_filler(pstruct, 0xff, PNG_FILLER_AFTER);
	}
	
	png_read_update_info(pstruct, info_struct);
    
    if(setjmp(png_jmpbuf(pstruct))){
		return;
	}
	
	int rowbytes = png_get_rowbytes(pstruct, info_struct);
	//rowbytes += 3 - ((rowbytes-1) % 4);
	
	image_data = (png_byte*)malloc(rowbytes * height /** sizeof(png_byte)+15*/);
	row_ptrs = (png_bytep*)malloc(sizeof(png_bytep) * height);
	for(int i = 0; i < height; i++){
		row_ptrs[height - 1 - i] = image_data + i * rowbytes;
	}
	
	png_read_image(pstruct, row_ptrs);
	
	/*if(png_get_color_type(pstruct, info_struct) != PNG_COLOR_TYPE_RGBA) {
		png_set_add_alpha(pstruct, 0xff, PNG_FILLER_AFTER);
		std::cerr << "Added Alpha channel\n";
	}*/
	
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image_data);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	float color[] = { 0.0f, 0.0f, 0.0f, 0.0f };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, color);
	
	textures[fname] = texture;
	
	png_destroy_read_struct(&pstruct, &info_struct, NULL);
	free(image_data);
	free(row_ptrs);
	fclose(infile);
}

void AssetManager::loadFont(std::string path, Vec2d size) {
	std::string fname = path;
	path = getPath().append("/data/fonts/" + path);

	FT_Face face;
	FT_New_Face(freetype_library, path.c_str(), 0, &face);
	
	fonts[fname] = Font(fname, size, face);
	FT_Done_Face(face);
}

// TODO Add support for new types
void AssetManager::loadSound(std::string path) {
	std::string fname = path;
	path = getPath().append("/data/se/" + path);
	
	ALuint sound_source, sound_buffer;
	alGenSources(1, &sound_source);
	alGenBuffers(1, &sound_buffer);
	alSourcef(sound_source, AL_PITCH, 1);
	alSourcef(sound_source, AL_GAIN, 1);
	alSource3f(sound_source, AL_POSITION, 0, 0, 0);
	alSource3f(sound_source, AL_VELOCITY, 0, 0, 0);
	alSourcei(sound_source, AL_LOOPING, AL_FALSE);
	
	FILE* infile = fopen(path.c_str(), "rb");
	if(!infile) {
		std::cerr << "IO ERROR: Could not open file " + fname + ".\n";
		return;
	}
	
	char id[5];
	id[4] = 0;
	uint8_t* input_buffer;
	uint32_t file_size, format_size, sample_rate, uint8_ts_per_sec, data_size;
	short tag, channel_count, block_alignment, bits_per_sample;
	ALenum format = AL_FORMAT_MONO8;
	
	fread(&id, sizeof(uint8_t), 4, infile);
	if(strcmp(id, "RIFF")) {
		std::cerr << "IO ERROR: " + fname + " is not a RIFF file. (RIFF not found, got " << id << ")\n";
		fclose(infile);
		return;
	}
	fread(&file_size, sizeof(uint32_t), 1, infile);
	fread(&id, sizeof(uint8_t), 4, infile);
	if(strcmp(id, "WAVE")) {
		std::cerr << "IO ERROR: " + fname + " is not a WAV file. (WAVE not found, got " << id << ")\n";
		fclose(infile);
		return;
	}
	
	fread(&id, sizeof(uint8_t), 4, infile);
	if(strcmp(id, "fmt ")) {
		std::cerr << "IO ERROR: " + fname + " has unrecognizable formatting data. (fmt not found, got " << id << ")\n";
		fclose(infile);
		return;
	}
	fread(&format_size, sizeof(uint32_t), 1, infile);
	if(format_size != 16) {
		std::cerr << "IO ERROR: " + fname + " has unrecognizable formatting data. (nonstandard formatting length)\n";
		fclose(infile);
		return;
	}
	fread(&tag, sizeof(short), 1, infile);
	fread(&channel_count, sizeof(short), 1, infile);
	fread(&sample_rate, sizeof(uint32_t), 1, infile);
	fread(&uint8_ts_per_sec, sizeof(uint32_t), 1, infile);
	fread(&block_alignment, sizeof(short), 1, infile);
	fread(&bits_per_sample, sizeof(short), 1, infile);
	
	fread(&id, sizeof(uint8_t), 4, infile);
	if(strcmp(id, "data")) {
		fclose(infile);
		return;
	}
	fread(&data_size, sizeof(uint32_t), 1, infile);
	input_buffer = (uint8_t*) malloc(sizeof(uint8_t) * data_size);
	fread(input_buffer, sizeof(uint8_t), data_size, infile);
	fclose(infile);
	
	if(channel_count == 1) {
		if(bits_per_sample == 8) {
			format = AL_FORMAT_MONO8;
		} else if(bits_per_sample == 16) {
			format = AL_FORMAT_MONO16;
		}
	} else if(channel_count == 2) {
		if(bits_per_sample == 8) {
			format = AL_FORMAT_STEREO8;
		} else if(bits_per_sample == 16) {
			format = AL_FORMAT_STEREO16;
		}
	}
	
	alBufferData(sound_buffer, format, input_buffer, data_size, sample_rate);
	alSourcei(sound_source, AL_BUFFER, sound_buffer);
	
	sounds[fname] = sound_source;
}

void AssetManager::loadShader(std::string path) {
	std::string fname = path;
	path = getPath().append("/data/shaders/" + path);
	FILE* infile = fopen(path.c_str(), "r");
	std::vector<const char*> command_vector;
	char c;
	std::string command;
	do {
		c = fgetc(infile);
		if(c == '\n' || c == '\0') {
			command += '\0';
			char* cpy = new char[command.length()];
			std::copy(command.c_str(), command.c_str() + command.length(), cpy);
			command_vector.push_back(cpy);
			command.clear();
		} else
			command += c;
	} while(c != EOF);
	
	const char** shaderdat = new const char*[command_vector.size()];
	for(unsigned i = 0; i < command_vector.size(); ++i)
		shaderdat[i] = command_vector[i];
	
	GLuint shader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(shader, command_vector.size(), shaderdat, NULL);
	glCompileShader(shader);
	shaders[fname] = shader;
}

void AssetManager::compileProgram(std::string program_id, std::vector<std::string> shader_ids) {
	GLuint new_program = glCreateProgram();
	for(auto s : shader_ids)
		glAttachShader(new_program, getShader(s));
	glLinkProgram(new_program);
	programs[program_id] = new_program;
}
