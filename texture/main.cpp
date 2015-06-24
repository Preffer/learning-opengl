#include <cstring>
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

	GLuint programID = buildProgram("vertex.shader", "fragment.shader");
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
	GLuint texture = readImage("uvtemplate.bmp", &width, &height);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);

	GLuint textureID  = glGetUniformLocation(programID, "textureSampler");
	glUniform1i(textureID, 0);

	GLuint texelSizeID = glGetUniformLocation(programID, "texelSize");
	glUniform2i(texelSizeID, width, height);
	
	char* readback = new char[512 * 512 * 3];
	for (int i = 0; i < 512 * 512 * 3; i++) {
		readback[i] = rand();
	}

	GLuint textureBuffer;
	glGenTextures(1, &textureBuffer);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, textureBuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, readback);
	glGenerateMipmap(GL_TEXTURE_2D);

	GLuint bufferID  = glGetUniformLocation(programID, "bufferSampler");
	glUniform1i(bufferID, 1);

	enum {Color, Depth, NumRenderbuffers};

	GLuint framebuffer, renderbuffer[NumRenderbuffers];

	glGenRenderbuffers(NumRenderbuffers, renderbuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, renderbuffer[Color]);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_RGBA, 512, 512);

	glBindRenderbuffer(GL_RENDERBUFFER, renderbuffer[Depth]);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 512, 512);

	glGenFramebuffers(1, &framebuffer);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, framebuffer);
	glFramebufferRenderbuffer(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, renderbuffer[Color]);
	glFramebufferRenderbuffer(GL_DRAW_FRAMEBUFFER, GL_DEPTH_COMPONENT, GL_RENDERBUFFER, renderbuffer[Depth]);

	glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureBuffer, 0);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	cout << "before: " << (glGetError() == GL_INVALID_OPERATION) << endl;

	do {
		cout << "1: " << (glGetError() == GL_INVALID_OPERATION) << endl;
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, framebuffer);
		cout << "2: " << (glGetError() == GL_INVALID_OPERATION) << endl;
		glViewport(0, 0, 512, 512);
		cout << "3: " << (glGetError() == GL_INVALID_OPERATION) << endl;
		glDrawArrays(GL_TRIANGLE_STRIP, 0, sizeof(vertexData));
		cout << "4: " << (glGetError() == GL_INVALID_OPERATION) << endl;
		glGenerateMipmap(GL_TEXTURE_2D);
		cout << "5: " << (glGetError() == GL_INVALID_OPERATION) << endl;
		glBindFramebuffer(GL_READ_FRAMEBUFFER, framebuffer);
		cout << "6: " << (glGetError() == GL_INVALID_OPERATION) << endl;
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
		cout << "7: " << (glGetError() == GL_INVALID_OPERATION) << endl;

		//glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		//glClear(GL_COLOR_BUFFER_BIT);

		glViewport(0, 0, 512, 512);
		cout << "8: " << (glGetError() == GL_INVALID_OPERATION) << endl;
		glBlitFramebuffer(0, 0, 512, 512, 0, 0, 512, 512, GL_COLOR_BUFFER_BIT, GL_NEAREST);
		cout << "9: " << (glGetError() == GL_INVALID_OPERATION) << endl;

		//glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
		//glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

		//glGenerateMipmap(GL_TEXTURE_2D);

		glfwSwapBuffers(window);
		cout << "10: " << (glGetError() == GL_INVALID_OPERATION) << endl;
		glfwPollEvents();
		cout << "11: " << (glGetError() == GL_INVALID_OPERATION) << endl;
	} while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS && glfwWindowShouldClose(window) == 0);

	return EXIT_SUCCESS;
}
