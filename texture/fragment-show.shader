#version 330 core

out vec3 color;

uniform sampler2D frame;
uniform ivec2 size;
uniform float time;

void main() {
	color = texture(frame, (gl_FragCoord.xy + vec2(sin(time) * 10, cos(time))) / size).rgb;
}
