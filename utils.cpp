#include "utils.h"

void GLAPIENTRY utils::gl_message_callback(GLenum source, GLenum type, GLuint id, GLenum severity,
	GLsizei length, const GLchar* message, const void* userParam)
{
	auto const type_str = [type]() {
		switch (type)
		{
		case GL_DEBUG_TYPE_ERROR: return "ERROR";
		case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: return "DEPRECATED_BEHAVIOR";
		case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR: return "UNDEFINED_BEHAVIOR";
		case GL_DEBUG_TYPE_PORTABILITY: return "PORTABILITY";
		case GL_DEBUG_TYPE_PERFORMANCE: return "PERFORMANCE";
		case GL_DEBUG_TYPE_MARKER: return "MARKER";
		case GL_DEBUG_TYPE_OTHER: return "OTHER";
		default: return "";
		}
	}();

	auto const severity_str = [severity]() {
		switch (severity) {
		case GL_DEBUG_SEVERITY_NOTIFICATION: return "NOTIFICATION";
		case GL_DEBUG_SEVERITY_LOW: return "LOW";
		case GL_DEBUG_SEVERITY_MEDIUM: return "MEDIUM";
		case GL_DEBUG_SEVERITY_HIGH: return "HIGH";
		default: return "";
		}
	}();

	auto const src_str = [source]() {
		switch (source)
		{
		case GL_DEBUG_SOURCE_API: return "API";
		case GL_DEBUG_SOURCE_WINDOW_SYSTEM: return "WINDOW SYSTEM";
		case GL_DEBUG_SOURCE_SHADER_COMPILER: return "SHADER COMPILER";
		case GL_DEBUG_SOURCE_THIRD_PARTY: return "THIRD PARTY";
		case GL_DEBUG_SOURCE_APPLICATION: return "APPLICATION";
		case GL_DEBUG_SOURCE_OTHER: return "OTHER";
		default: return "";
		}
	}();

	(void)length;
	(void)userParam;
	std::cerr << "GL CALLBACK: " << id << ", " << type_str << ", " << severity_str << ", " << src_str << ": " << message << '\n';
}

void utils::error_callback(int code, const char* description)
{
	std::cerr << "Error: " << code << " | " << description << '\n';
}

void utils::framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	(void)window;
	glViewport(0, 0, width, height);
}

GLuint utils::loadShader(const char* path, const GLenum type)
{
	std::ifstream file(path, std::ios::binary | std::ios::in);
	if (!file.is_open())
	{
		std::cerr << "Niepoprawny odczyt pliku shadera" << std::endl;
		std::exit(1);
	}
	file.seekg(0, std::ios::end);
	const unsigned int length = file.tellg();
	if (length <= 0)
	{
		std::cerr << "Niepoprawny odczyt pliku shadera" << std::endl;
		std::exit(1);
	}
	file.seekg(0, std::ios::beg);			//ustaw odczyt na początek
	GLchar* buffer = new GLchar[length + 1];		//bufor na kod żródłowy i znak null
	file.read(buffer, length);				//wczytaj cały plik
	buffer[length] = '\0';					//dodaj znak null

	const GLuint shader = glCreateShader(type);	//utworzenie idendyfikatora shadera
	glShaderSource(shader, 1, &buffer, nullptr);	//przekaż kod źródłowy shadera
	delete[] buffer;
	glCompileShader(shader);				//skompiluj shader
	return shader;							//zwróć idendyfikator
}

GLuint utils::createProgram(std::initializer_list<GLuint> shaders)
{
	GLuint program = glCreateProgram();
	for(const GLuint shader : shaders)
	{
		glAttachShader(program, shader);
		glDeleteShader(shader);
	}
	glLinkProgram(program);

	GLint status;					//zmienna do przechowywania stanu konsolidacji i walidacji
	glGetProgramiv(program, GL_LINK_STATUS, &status);		//pobierz stan linkowania
	if (status == GL_FALSE)
	{
		//pobranie i wyświetlenie logu informacyjnego
		GLint loglength;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &loglength);
		char* log = new char[loglength];
		glGetProgramInfoLog(program, loglength, nullptr, log);
		std::cout << log << std::endl;
		delete[] log;
		std::exit(1);
	}
	glValidateProgram(program);		//walidacja programu(opcjonalna)
	glGetProgramiv(program, GL_VALIDATE_STATUS, &status);		//pobierz stan walidacji
	if (status == GL_FALSE)
	{
		//pobranie i wyświetlenie logu informacyjnego
		GLint loglength;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &loglength);
		char* log = new char[loglength];
		glGetProgramInfoLog(program, loglength, nullptr, log);
		std::cout << log << std::endl;
		delete[] log;
		std::exit(1);
	}
	return program;
}
