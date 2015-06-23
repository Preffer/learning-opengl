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

GLuint readImage(const char* path, int* w, int* h) {
	printf("Reading image %s\n", path);

	unsigned char header[54];
	unsigned int dataPos;
	unsigned int imageSize;
	unsigned int width, height;
	unsigned char * data;

	FILE * file = fopen(path,"rb");
	if (!file) {
		printf("%s could not be opened. Are you in the right directory ? Don't forget to read the FAQ !\n", path); getchar();
		return 0;
	}

	if ( fread(header, 1, 54, file)!=54 ){ 
		printf("Not a correct BMP file\n");
		return 0;
	}
	// A BMP files always begins with "BM"
	if ( header[0]!='B' || header[1]!='M' ){
		printf("Not a correct BMP file\n");
		return 0;
	}
	// Make sure this is a 24bpp file
	if ( *(int*)&(header[0x1E])!=0  )         {printf("Not a correct BMP file\n");    return 0;}
	if ( *(int*)&(header[0x1C])!=24 )         {printf("Not a correct BMP file\n");    return 0;}

	// Read the information about the image
	dataPos    = *(int*)&(header[0x0A]);
	imageSize  = *(int*)&(header[0x22]);
	width      = *(int*)&(header[0x12]);
	height     = *(int*)&(header[0x16]);

	// Some BMP files are misformatted, guess missing information
	if (imageSize==0)    imageSize=width*height*3; // 3 : one byte for each Red, Green and Blue component
	if (dataPos==0)      dataPos=54; // The BMP header is done that way

	// Create a buffer
	data = new unsigned char [imageSize];

	// Read the actual data from the file into the buffer
	fread(data,1,imageSize,file);

	// Everything is in memory now, the file wan be closed
	fclose (file);

	// Create one OpenGL texture
	GLuint textureID;
	glGenTextures(1, &textureID);
	
	// "Bind" the newly created texture : all future texture functions will modify this texture
	glBindTexture(GL_TEXTURE_2D, textureID);

	// Give the image to OpenGL
	glTexImage2D(GL_TEXTURE_2D, 0,GL_RGB, width, height, 0, GL_BGR, GL_UNSIGNED_BYTE, data);

	// OpenGL has now copied the data. Free our own version
	delete [] data;

	// Poor filtering, or ...
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); 

	// ... nice trilinear filtering.
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); 
	glGenerateMipmap(GL_TEXTURE_2D);

	// Return the ID of the texture we just created
	*w = width;
	*h = height;
	return textureID;
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
