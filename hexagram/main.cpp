#include <cmath>
#include <cstring>
#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <boost/format.hpp>
#include "../common/util.hpp"

using namespace std;
using namespace boost;

int main(int argc, char* argv[]) {
	if (argc < 2) {
		cout << format("Usage: %1% <star>") % argv[0]<< endl;
		return EXIT_FAILURE;
	}

	const int star = atoi(argv[1]);

	if (!glfwInit()) {
		throw runtime_error("Failed to initialize GLFW");
	}

	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(600, 600, "Hexagram", NULL, NULL);
	if (!window) {
		throw runtime_error("Failed to open GLFW window");
	}
	glfwMakeContextCurrent(window);
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK) {
		throw runtime_error("Failed to initialize GLEW");
	}

	GLuint programID = buildProgram("vertex.shader", "fragment.shader");
	glUseProgram(programID);
	glClearColor(25 / 255.0f, 25 / 255.0, 25 / 255.0, 0.0f);

	GLuint vertexArrayID;
	glGenVertexArrays(1, &vertexArrayID);
	glBindVertexArray(vertexArrayID);

	GLfloat vertexData[star * 12];

	// center vertex
	GLfloat centerVertex[2];
	centerVertex[0] = 0;
	centerVertex[1] = 0;

	for (int i = 0; i < star; i++) {
		vertexData[12 * i] = cos(2 * i * M_PI / star) / 2;
		vertexData[12 * i + 1] = sin(2 * i * M_PI / star) / 2;

		vertexData[12 * i + 2] = cos(2 * (i + 1) * M_PI / star) / 2;
		vertexData[12 * i + 3] = sin(2 * (i + 1) * M_PI / star) / 2;

		memcpy(vertexData + (12 * i + 4), centerVertex, sizeof(GLfloat) * 2);
		memcpy(vertexData + (12 * i + 6), vertexData + (12 * i), sizeof(GLfloat) * 4);

		vertexData[12 * i + 10] = cos((2 * i + 1) * M_PI / star);
		vertexData[12 * i + 11] = sin((2 * i + 1) * M_PI / star);
	}

	GLfloat colorData[star * 18];

	// center color
	GLfloat centerColor[3];
	centerColor[0] = float(rand()) / RAND_MAX;
	centerColor[1] = float(rand()) / RAND_MAX;
	centerColor[2] = float(rand()) / RAND_MAX;

	/*
	 * if: most case first
	 *
	 * first star => generate 3 random color
	 * other star => generate 2 random color, copy 1 color
	 * last  star => generate 1 random color, copy 2 color
	 *
	 */
	for (int i = 0; i < star; i++) {
		if (i != 0) {
			memcpy(colorData + (18 * i), colorData + (18 * i - 6), sizeof(GLfloat) * 3);
		} else {
			colorData[0] = float(rand()) / RAND_MAX;
			colorData[1] = float(rand()) / RAND_MAX;
			colorData[2] = float(rand()) / RAND_MAX;
		}

		if (i != (star - 1)) {
			colorData[18 * i + 3] = float(rand()) / RAND_MAX;
			colorData[18 * i + 4] = float(rand()) / RAND_MAX;
			colorData[18 * i + 5] = float(rand()) / RAND_MAX;
		} else {
			memcpy(colorData + (18 * i + 3), colorData, sizeof(GLfloat) * 3);
		}

		memcpy(colorData + (18 * i + 6), centerColor, sizeof(centerColor));
		memcpy(colorData + (18 * i + 9), colorData + (18 * i), sizeof(GLfloat) * 6);

		colorData[18 * i + 15] = float(rand()) / RAND_MAX;
		colorData[18 * i + 16] = float(rand()) / RAND_MAX;
		colorData[18 * i + 17] = float(rand()) / RAND_MAX;
	}

	GLuint vertexBuffer;
	glGenBuffers(1, &vertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertexData), vertexData, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glEnableVertexAttribArray(0);

	GLuint colorBuffer;
	glGenBuffers(1, &colorBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, colorBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(colorData), colorData, GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glEnableVertexAttribArray(1);

	do {
		glClear(GL_COLOR_BUFFER_BIT);
		glDrawArrays(GL_TRIANGLES, 0, star * 12);
		glfwSwapBuffers(window);
		glfwPollEvents();
	} while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS && glfwWindowShouldClose(window) == 0);

	return EXIT_SUCCESS;
}
