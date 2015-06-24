#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <boost/format.hpp>
#include "../common/util.hpp"

using namespace std;
using namespace boost;
using namespace glm;

int main(int argc, char* argv[]) {
	if (!glfwInit()) {
		throw runtime_error("Failed to initialize GLFW");
	}

	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(512, 512, "Texture", NULL, NULL);
	if (!window) {
		throw runtime_error("Failed to open GLFW window");
	}
	glfwMakeContextCurrent(window);
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK) {
		throw runtime_error("Failed to initialize GLEW");
	}

	GLuint programID = buildProgram("vertex.shader", "fragment-render.shader");
	glUseProgram(programID);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

	GLuint vertexArrayID;
	glGenVertexArrays(1, &vertexArrayID);
	glBindVertexArray(vertexArrayID);

	static const GLfloat vertexData[] = {
		-1.0f, -1.0f,
		-1.0f,  1.0f,
		 1.0f, -1.0f,
		 1.0f,  1.0f
	};

	GLuint vertexBuffer;
	glGenBuffers(1, &vertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertexData), vertexData, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glEnableVertexAttribArray(0);

	int width, height;
	unsigned char* motionData = readBMP("motion.bmp", &width, &height);

	GLuint motionID;
	glGenTextures(1, &motionID);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, motionID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, motionData);
	glGenerateMipmap(GL_TEXTURE_2D);

	glUniform1i(glGetUniformLocation(programID, "motion"), 0);
	glUniform2i(glGetUniformLocation(programID, "size"), width, height);

	char* frameData = new char[width * height * 3];
	for(int i = 0; i < width * height * 3; i++) {
		frameData[i] = rand();
	}

	GLuint frameTexture;
	glGenTextures(1, &frameTexture);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, frameTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, frameData);
	glGenerateMipmap(GL_TEXTURE_2D);

	GLuint frameBuffer;
	glGenFramebuffers(1, &frameBuffer);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, frameBuffer);

	// Set "renderedTexture" as our colour attachement #0
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, frameTexture, 0);

	cout << (glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE) << endl;

	GLuint showID = buildProgram("vertex.shader", "fragment-show.shader");
	glUseProgram(showID);
	glUniform1i(glGetUniformLocation(showID, "frame"), 1);
	glUniform2i(glGetUniformLocation(showID, "size"), width, height);

	GLuint timeID = glGetUniformLocation(showID, "time");

	int frameCount = 0;
	do {
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, frameBuffer);

		glClear(GL_COLOR_BUFFER_BIT);
		glUseProgram(programID);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, sizeof(vertexData));

		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

		glClear(GL_COLOR_BUFFER_BIT);
		glUseProgram(showID);
		glUniform1f(timeID, glfwGetTime());
		glDrawArrays(GL_TRIANGLE_STRIP, 0, sizeof(vertexData));

		cout << format("Time: %1%, Count: %2%, FPS: %3%") % glfwGetTime() % (++frameCount) % (frameCount / glfwGetTime()) << endl;
		glfwSwapBuffers(window);
		glfwPollEvents();
	} while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS && glfwWindowShouldClose(window) == 0);

	return EXIT_SUCCESS;
}
