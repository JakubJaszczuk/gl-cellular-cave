#ifndef UTILS_H
#define UTILS_H

#define GL_GLEXT_PROTOTYPES
//#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <fstream>
#include <initializer_list>

namespace utils
{
void error_callback(int code, const char* description);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);

void GLAPIENTRY gl_message_callback(GLenum source, GLenum type, GLuint id, GLenum severity,
	GLsizei length, const GLchar* message, const void* userParam);

GLuint loadShader(const char* path, const GLenum type);

GLuint createProgram(std::initializer_list<GLuint> shaders);
} // namespace utils

#endif // UTILS_H
