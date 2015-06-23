#ifndef UTIL_HPP
#define UTIL_HPP

const char* readFileBytes(const char* path);
GLuint readImage(const char* path, int* w, int* h);
GLuint buildShader(const char* shaderFile, GLenum shaderType);
GLuint buildProgram(const char* vertexShaderFile, const char* fragmentShaderFile);

#endif
