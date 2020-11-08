#include <random>
#include <algorithm>
#include <iostream>
#include <vector>
#include <thread>
#include <glm/glm.hpp>
#include "utils.h"

using namespace std::chrono_literals;

int main(int argc, char const *argv[])
{
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_real_distribution dis_normalized(0.0f, 1.0f);
	auto prob = 0.5;
	auto period = 250ms;
	if(argc > 1)
	{
		prob = std::atof(argv[1]);
	}
	if(argc > 2)
	{
		period = std::chrono::milliseconds(std::atoi(argv[2]));
	}
	std::bernoulli_distribution dis(prob);

	if(!glfwInit())
	{
		return 1;
	}

	int32_t res_width = 512;
	int32_t res_height = 512;

	glfwSetErrorCallback(utils::error_callback);

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);
	GLFWwindow* window = glfwCreateWindow(res_width, res_height, "Cellular automata", NULL, NULL);

	if(!window)
	{
		glfwTerminate();
	}

	glfwSetFramebufferSizeCallback(window, utils::framebuffer_size_callback);
	glfwMakeContextCurrent(window);
	glDebugMessageCallback(utils::gl_message_callback, 0);  // Super fajna rzecz
	glfwSwapInterval(1);

	// Shader
	const auto vs = utils::loadShader("vert.vert", GL_VERTEX_SHADER);
	const auto fs = utils::loadShader("frag.frag", GL_FRAGMENT_SHADER);
	const auto program = utils::createProgram({vs, fs});
	glUseProgram(program);

	// Automata data
	std::vector<uint8_t> automata_data(128 * 128);
	std::generate(std::begin(automata_data), std::end(automata_data), [&]() { return dis(gen);} );

	GLuint texture;
	glCreateTextures(GL_TEXTURE_2D, 1, &texture);
	glTextureParameteri(texture, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTextureParameteri(texture, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTextureStorage2D(texture, 1, GL_R8UI, 128, 128);
	glBindTextureUnit(0, texture);
	glTextureSubImage2D(texture, 0, 0, 0, 128, 128, GL_RED_INTEGER, GL_UNSIGNED_BYTE, automata_data.data());
	//glTextureSubImage2D(texture, 0, 0, 0, 64, 64, GL_RED, GL_FLOAT, automata_data.data());

	// VAO
	GLuint vao;
	glCreateVertexArrays(1, &vao);
	glBindVertexArray(vao);

	// Images
	glBindImageTexture(0, texture, 0, false, 0, GL_READ_ONLY, GL_R8UI);
	glBindImageTexture(1, texture, 0, false, 0, GL_WRITE_ONLY, GL_R8UI);

	// Compute shader
	const auto cs = utils::loadShader("comp.comp", GL_COMPUTE_SHADER);
	const auto compute = utils::createProgram({cs});

	auto last_simulation_time = std::chrono::steady_clock::now();
	while(!glfwWindowShouldClose(window))
	{
		// Simulation
		auto time_now = std::chrono::steady_clock::now();
		std::chrono::duration<double, std::milli> time_diff = time_now - last_simulation_time;
		if(time_diff > period)
		{
			last_simulation_time = time_now;
			glUseProgram(compute);
			glDispatchCompute(4, 4, 1);
			glUseProgram(program);
		}

		// Draw
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

		// Events
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glDeleteProgram(program);
	glDeleteVertexArrays(1, &vao);

	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}
