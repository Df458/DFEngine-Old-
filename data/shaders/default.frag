#version 120
uniform vec4 color = vec4(1.0, 1.0, 1.0, 1.0);
varying vec2 uv;
uniform sampler2D tex;

void main() {
	gl_FragColor = texture2D(tex, uv) * color;
}
