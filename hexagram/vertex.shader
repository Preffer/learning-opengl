#version 330 core

layout(location = 0) in vec4 vPosition;
layout(location = 1) in vec3 vColor;

out vec3 fColor;

void main() {
	gl_Position = vPosition;
	fColor = vColor;
}
