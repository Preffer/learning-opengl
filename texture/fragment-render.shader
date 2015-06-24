#version 330 core

out vec3 color;

uniform sampler2D motion;
uniform ivec2 size;

void main() {
	color = texture2D(motion, gl_FragCoord.xy / size).rgb;
	//color = vec3(0.3, 0.3, 0.3);
}
