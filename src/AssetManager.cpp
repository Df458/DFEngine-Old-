#include "AssetManager.h"
#include "Font.h"
#include "Model.h"

using namespace df;

AssetManager::AssetManager() {
	FT_Init_FreeType(&freetype_library);
	
//Initialize model data for the default model, a rectangular plane
	static const GLfloat rect_vertex_data[] = { 
		-1.0f, -1.0f, 0.0f,
		 1.0f, -1.0f, 0.0f,
		-1.0f,  1.0f, 0.0f,
		 1.0f,  1.0f, 0.0f
	};
	static const GLfloat rect_uv_data[] = { 
		 0.0f,  1.0f,
		 1.0f,  1.0f,
		 0.0f,  0.0f,
		 1.0f,  0.0f
	};
	static const GLuint rect_index_data[] = { 
		0, 1, 2,
		1, 2, 3
	};

	Model default_model;
	default_model.index_count = 6;
	glGenBuffers(1, &default_model.vertex_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, default_model.vertex_buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(rect_vertex_data), rect_vertex_data, GL_STATIC_DRAW);
	
	glGenBuffers(1, &default_model.index_buffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, default_model.index_buffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(rect_index_data), rect_index_data, GL_STATIC_DRAW);
	
	glGenBuffers(1, &default_uv);
	glBindBuffer(GL_ARRAY_BUFFER, default_uv);
	glBufferData(GL_ARRAY_BUFFER, sizeof(rect_uv_data), rect_uv_data, GL_STATIC_DRAW);
	
	default_model.uv_buffer = default_uv;

//Initalize RGB data for the default txture, which is plain white
	static const GLfloat color_tex_data[] = {
		1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f
	} ;
	
	GLuint default_texture;
	glGenTextures(1, &default_texture);
	glBindTexture(GL_TEXTURE_2D, default_texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 2, 2, 0, GL_RGB, GL_UNSIGNED_BYTE, color_tex_data);
	
	textures["default"] = default_texture;
	models["default"] = default_model;

//Load the default shaders	
	loadShader("default.vert", false);
	loadShader("default.frag");
	std::vector<std::string>vec;
	vec.push_back("default.vert");
	vec.push_back("default.frag");
	compileProgram("default", vec);
}

AssetManager::~AssetManager() {
	cleanup();
}

Font*  AssetManager::getFont(std::string id) {
	return &fonts[id];
}

ALuint AssetManager::getSound(std::string id) {
	return sounds[id];
}

GLuint AssetManager::getTexture(std::string id) {
	return textures[id];
}

GLuint AssetManager::getShader(std::string id) {
	return shaders[id];
}

GLuint AssetManager::getProgram(std::string id) {
	return programs[id];
}

GLuint AssetManager::getDefaultUV()	{
	return default_uv;
}

Model  AssetManager::getModel(std::string id) {
	return models[id];
}

void AssetManager::cleanup() {
	for(auto i : textures) {
		glDeleteTextures(1, &i.second);
	}
	for(auto i : programs) {
		glDeleteProgram(i.second);
	}
	for(auto i : shaders) {
		glDeleteShader(i.second);
	}
	for(auto i : models) {
	    glDeleteBuffers(1, &i.second.vertex_buffer);
	    glDeleteBuffers(1, &i.second.index_buffer);
	    glDeleteBuffers(1, &i.second.uv_buffer);
	}
	for(auto i : sounds) {
		alDeleteSources(1, &i.second);
	}
	for(auto i : fonts) {
		i.second.cleanup();
	}
	
	FT_Done_FreeType(freetype_library);
}

//:TODO: 30.08.14 10:09:38, Hugues Ross
// This is old code, and needs to be rewritten
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

//:TODO: 30.08.14 10:10:23, Hugues Ross
// This needs support for multiple sound types, and streaming
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
	
	free(input_buffer);
}

void AssetManager::loadShader(std::string path, bool frag) {
	std::string fname = path;
	path = getPath().append("/data/shaders/" + path);
	FILE* infile = fopen(path.c_str(), "r");
	fseek(infile, 0, SEEK_END);
	int length = ftell(infile);
	fseek(infile, 0, SEEK_SET);
	char* input_data = new char[length];
	fread((void*) input_data, 1, (size_t)length, infile);
	input_data[length-1] = '\0';
	fclose(infile);
	const char* shader_data[2] = {input_data, "\0"};

	GLuint shader;
	if(frag)
		shader = glCreateShader(GL_FRAGMENT_SHADER);
	else
		shader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(shader, 1, shader_data, NULL);
	glCompileShader(shader);
	shaders[fname] = shader;
	delete input_data;
}

void AssetManager::compileProgram(std::string program_id, std::vector<std::string> shader_ids) {
	GLuint new_program = glCreateProgram();
	for(auto s : shader_ids)
		glAttachShader(new_program, getShader(s));
	glLinkProgram(new_program);
	programs[program_id] = new_program;
}

void AssetManager::loadModel(std::string path) {
	std::string fname = path;
	path = getPath().append("/data/models/" + path);
	Model model;
	std::vector<GLfloat> verts;
	std::vector<GLuint>  indices;
	std::vector<GLuint>  uv_indices;
	std::vector<GLfloat>  uv_pos;
	
	FILE* infile = fopen(path.c_str(), "r");
	while(!feof(infile)) {
		char ch = fgetc(infile);
		switch(ch) {
			case 'v': {
				if(fgetc(infile) == 't') {
					GLfloat pos[2] = {0, 0};
					if(fscanf(infile, "%f %f\n", &pos[0], &pos[1]) != 2)
						std::cerr << "Read Error\n";
					uv_pos.push_back(pos[0]);
					uv_pos.push_back(pos[1]);
				} else {
					fseek(infile, -1, SEEK_CUR);
					GLfloat vert[3] = {0, 0, 0};
					if(fscanf(infile, "%f %f %f\n", &vert[0], &vert[1], &vert[2]) != 3)
						std::cerr << "Read Error\n";
					verts.push_back(vert[0]);
					verts.push_back(vert[1]);
					verts.push_back(vert[2]);
					//std::cerr << "vert: " << vert[0] << ", " << vert[1] << ", " << vert[2] << "\n";
				}
			} break;
			
			case 'f': {
				GLuint index[3] = {0, 0, 0};
				GLuint uv_index[3] = {0, 0, 0};
				if(fscanf(infile, "%u/%u %u/%u %u/%u\n", &index[0], &uv_index[0], &index[1], &uv_index[1], &index[2], &uv_index[2]) != 6)
					std::cerr << "Read Error\n";
				indices.push_back(index[0] - 1);
				indices.push_back(index[1] - 1);
				indices.push_back(index[2] - 1);
				uv_indices.push_back(uv_index[0] - 1);
				uv_indices.push_back(uv_index[1] - 1);
				uv_indices.push_back(uv_index[2] - 1);
				//std::cerr << "index: " << index[0] << ", " << index[1] << ", " << index[2] << "\n";
			} break;
			
			default:
				char testchar;
				while((testchar = fgetc(infile)) && testchar != '\n' && testchar != '\0');
				//std::cerr << "N/A\n";
		}
	}
	
	GLfloat* final_verts = new GLfloat[uv_indices.size() * 3];
	for(unsigned i = 0; i < uv_indices.size(); ++i) {
		final_verts[uv_indices[i] * 3] = verts[indices[i] * 3];
		final_verts[uv_indices[i] * 3 + 1] = verts[indices[i] * 3 + 1];
		final_verts[uv_indices[i] * 3 + 2] = verts[indices[i] * 3 + 2];
	}
	
	fclose(infile);
	model.index_count = indices.size();
	glGenBuffers(1, &model.vertex_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, model.vertex_buffer);
	glBufferData(GL_ARRAY_BUFFER, uv_indices.size() * 3 *sizeof(GLfloat), &final_verts[0], GL_STATIC_DRAW);
	
	glGenBuffers(1, &model.index_buffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, model.index_buffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, uv_indices.size() * sizeof(GLuint), &uv_indices[0], GL_STATIC_DRAW);
	
	glGenBuffers(1, &model.uv_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, model.uv_buffer);
	glBufferData(GL_ARRAY_BUFFER, uv_pos.size() * sizeof(GLfloat), &uv_pos[0], GL_STATIC_DRAW);
	//model.uv_buffer = default_uv;
	
	models[fname] = model;

	delete final_verts;
}
