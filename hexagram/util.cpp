#include <GL/glew.h>
#include <boost/format.hpp>
#include "util.hpp"

using namespace std;
using namespace boost;

const char* readFileBytes(const char* path) {
	FILE* fp = fopen(path, "r");
	if (!fp) {
		throw runtime_error((format("File not found: %1%") % path).str());
	}

	fseek(fp, 0, SEEK_END);
	long size = ftell(fp);
	rewind(fp);

	char* buffer = new char[size + 1];
	fread(buffer, size, 1, fp);
	fclose(fp);
	buffer[size] = 0;

	return buffer;
}

GLuint buildShader(const char* shaderFile, GLenum shaderType) {
	GLuint shaderID = glCreateShader(shaderType);
	const GLchar* shaderCode = readFileBytes(shaderFile);

	glShaderSource(shaderID, 1, &shaderCode, NULL);
	glCompileShader(shaderID);

	GLint result;
	glGetShaderiv(shaderID, GL_COMPILE_STATUS, &result);
	if (result != GL_TRUE) {
		glGetShaderiv(shaderID, GL_INFO_LOG_LENGTH, &result);
		char* info = new char[result];
		glGetShaderInfoLog(shaderID, result, NULL, info);
		throw runtime_error((format("Build shader %1% failed: %2%") % shaderFile % info).str());
	}

	return shaderID;
}

GLuint buildProgram(const char* vertexShaderFile, const char* fragmentShaderFile) {
	GLuint vertexShaderID = buildShader(vertexShaderFile, GL_VERTEX_SHADER);
	GLuint fragmentShaderID = buildShader(fragmentShaderFile, GL_FRAGMENT_SHADER);

	GLuint programID = glCreateProgram();
	glAttachShader(programID, vertexShaderID);
	glAttachShader(programID, fragmentShaderID);
	glLinkProgram(programID);

	GLint result;
	glGetProgramiv(programID, GL_LINK_STATUS, &result);
	if (result != GL_TRUE) {
		glGetProgramiv(programID, GL_INFO_LOG_LENGTH, &result);
		char* info = new char[result];
		glGetProgramInfoLog(programID, result, NULL, info);
		throw runtime_error((format("Build program failed: %1%") % info).str());
	}

	return programID;
}
