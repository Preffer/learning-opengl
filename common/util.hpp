#ifndef UTIL_HPP
#define UTIL_HPP

char* readFileBytes(const char* path);
unsigned char* readBMP(const char* path, int* w, int* h);
GLuint buildShader(const char* shaderFile, GLenum shaderType);
GLuint buildProgram(const char* vertexShaderFile, const char* fragmentShaderFile);

#endif
