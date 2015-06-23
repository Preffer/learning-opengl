#version 330 core

out vec3 color;

uniform sampler2D textureSampler;
uniform ivec2 texelSize;

void main() {
	color = texture2D(textureSampler, gl_FragCoord.xy / texelSize).rgb;
}
