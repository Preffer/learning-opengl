#version 330 core

out vec3 color;

uniform sampler2D textureSampler;
uniform ivec2 texelSize;
uniform sampler2D bufferSampler;

void main() {
	color = vec3(0.1, 0, 0) + texture2D(bufferSampler, gl_FragCoord.xy / texelSize).rgb;
}
