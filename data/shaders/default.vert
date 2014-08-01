#version 120
attribute vec3 vertex_pos;
attribute vec2 uv_out;
varying   vec2 uv;
uniform mat4 model_view_projection;

void main() {
	gl_Position = model_view_projection * vec4(vertex_pos, 1.0);
	uv = uv_out;
}
