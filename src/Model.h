#ifndef MODEL_H
#define MODEL_H
#include "Util.h"

namespace df {

struct Model {
    GLuint vertex_buffer = 0;
    GLuint index_buffer = 0;
    GLfloat* vertex_buffer_raw = NULL;
    GLuint uv_buffer = 0;
    unsigned index_count = 0;
    unsigned vertex_count = 0;
};
}

#endif
