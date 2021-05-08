#include <iostream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#define STB_IMAGE_IMPLEMENTATION
#include  <stb_image.h>

#include  "Shader.h"

#include  "Chip8.h"

#define WIN_WIDTH 640
#define WIN_HEIGHT 480

int main()
{
	glfwInit();

	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
	glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);

	GLFWwindow* win = glfwCreateWindow(WIN_WIDTH, WIN_HEIGHT, "Chip8 Emulator 2", nullptr, nullptr);
	glfwMakeContextCurrent(win);

	//compile time cast, no extra cpu instructions usually added
	gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress));

	glViewport(0, 0, WIN_WIDTH, WIN_HEIGHT);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	///////////////////////////////////////////////////////////////////

	//vertex data and tex coords
	const float VERTEX_DATA[] =
	{	  //pos		   //tex coords
		 -1.0f,  1.0f, 0.0f, 1.0f, // top left
		  1.0f,  1.0f, 1.0f, 1.0f, // top right
		  1.0f, -1.0f, 1.0f, 0.0f, // bottom right
    	 -1.0f, -1.0f, 0.0f, 0.0f  // bottom left 
	};

	const int INDICIES[] =
	{
		0, 1, 2,
		0,2, 3
	};

	GLuint vbo, ibo, vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(VERTEX_DATA), VERTEX_DATA, GL_STATIC_DRAW);

	glGenBuffers(1, &ibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(INDICIES), INDICIES, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), static_cast<void*>(nullptr));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), reinterpret_cast<void*>(2 * sizeof(float)));

	Shader f("shader", "Shader");
	Chip8 chip8;
	chip8.load("rom");

	GLuint tex;
	glGenTextures(1, &tex);
	glBindTexture(GL_TEXTURE_2D, tex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, GPU_BUFFER_WIDTH, GPU_BUFFER_HEIGHT, 0, GL_RED, GL_UNSIGNED_BYTE, NULL);
	glfwShowWindow(win);

	f.use();
	while(!glfwWindowShouldClose(win))
	{
		glfwPollEvents();

		chip8.cycle();

		if(chip8.canDraw())
		{
			//update texture
			auto data = chip8.getDrawData();

			glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, GPU_BUFFER_WIDTH, GPU_BUFFER_HEIGHT, GL_RED, GL_UNSIGNED_BYTE, static_cast<void*>(data.data()));
		}

		//draw
		glClear(GL_COLOR_BUFFER_BIT);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

		glfwSwapBuffers(win);
	}

	//glDeleteTextures(1, &tex);
	glDeleteBuffers(1, &vbo);
	glDeleteBuffers(1, &ibo);
	glDeleteVertexArrays(1, &vao);

	glfwDestroyWindow(win);
	glfwTerminate();
		
	return 0;
}