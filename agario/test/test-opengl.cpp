#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <string.h> 
#include <math.h>
#include <assert.h>

// todo: remove this
#define GL_SILENCE_DEPRECATION

#include <GLFW/glfw3.h>

#include "shader.hpp"

#define CIRCLE_SIDES 50
#define CIRCLE_VERTS (CIRCLE_SIDES + 2)
#define COLOR_LEN 3
#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600

// Defines circle to be rendered.
struct circle_obj {
	float verts[3 * CIRCLE_VERTS];
	float color[COLOR_LEN];
	unsigned int vao; // vertex attribute object
	unsigned int vbo; // vertex buffer object (gpu memory)
	circle_obj *next;
};

// Forward declarations
void render_loop(GLFWwindow *window);
void update_verts(circle_obj *circle);
void render_circle(circle_obj *circle, Shader *shader);
circle_obj *generate_circle(GLfloat x, GLfloat y, GLfloat z, GLfloat radius, GLfloat *color);
void free_circle(circle_obj *circle);
void process_input(GLFWwindow *window);
void position_circle(circle_obj *circle, GLfloat x, GLfloat y, GLfloat z, GLfloat radius);

circle_obj *circles;

int main(int argc, char *argv[]) {
	GLFWwindow *window;

	// Initialize the library
	if (!glfwInit()) return -1;
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // uncomment this statement to fix compilation on OS X
#endif

	// Create a windowed mode window and its OpenGL context
	window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Draw Circles Test", nullptr, nullptr);

	if (!window) {
		glfwTerminate();
		std::cerr << "window create failed" << std::endl;
		return -1;
	}

	glfwMakeContextCurrent(window);

	render_loop(window);

	return 0;
}

// Rendering update loop.
void render_loop(GLFWwindow *window) {
	// Load shader (for positioning and color)
	Shader shader("../client/vertex.shader", "../client/fragment.shader");
	
	circles = nullptr;
	GLfloat color[3] = { 1.0f, 0.0f, 0.0f };
	circle_obj *circle = generate_circle(0, 0, 0, 100.0f, color);
	circle_obj *circle2 = generate_circle(0, 0, 0, 100.f, color);

	int i = 0;
	// Loop until the user closes the window
	while (!glfwWindowShouldClose(window)) {
		process_input(window);

		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		// Render
		shader.use();
		//update verts
		render_circle(circle, &shader);
		render_circle(circle2, &shader);

		position_circle(circle2, i, 0, 0, 100.f);

		glfwSwapBuffers(window);
		glfwPollEvents();

		i++;
	}

	std::cout << "Total iterations: " << i << std::endl;

	free_circle(circle);
	free_circle(circle2);
	glfwTerminate();
}

// Shell update for vertices
void update_verts(circle_obj *circle) {
	glBindBuffer(GL_ARRAY_BUFFER, circle->vbo);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(circle->verts), circle->verts);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

// Renders the provided circle.
void render_circle(circle_obj *circle, Shader *shader) {
	shader->setVec3("col", circle->color[0], circle->color[1], circle->color[2]);
	glBindVertexArray(circle->vao);
	glDrawArrays(GL_TRIANGLE_FAN, 0, CIRCLE_VERTS);
	glBindVertexArray(0);
}

void position_circle(circle_obj *circle, GLfloat x, GLfloat y, GLfloat z, GLfloat radius) {
	circle->verts[0] = x / SCREEN_WIDTH;
	circle->verts[1] = y / SCREEN_HEIGHT;
	circle->verts[2] = z;

	for (int i = 1; i < CIRCLE_VERTS; i++) {
		circle->verts[i * 3] = static_cast<float> (x + (radius * cos(i *  2 * M_PI / CIRCLE_SIDES))) / SCREEN_WIDTH;
		circle->verts[i * 3 + 1] = static_cast<float> (y + (radius * sin(i * 2 * M_PI / CIRCLE_SIDES))) / SCREEN_HEIGHT;
		circle->verts[i * 3 + 2] = z;
	}

	glGenVertexArrays(1, &circle->vao);
	glGenBuffers(1, &circle->vbo);

	glBindVertexArray(circle->vao);
	glBindBuffer(GL_ARRAY_BUFFER, circle->vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(circle->verts), circle->verts, GL_STREAM_DRAW);

	// Position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);
	glEnableVertexAttribArray(0);
}

// Generates a circle with desired pos/radius/color.
circle_obj *generate_circle(GLfloat x, GLfloat y, GLfloat z, GLfloat radius, GLfloat *color) {
	auto circle = new circle_obj;
	assert(circle != nullptr);

	position_circle(circle, x, y, z, radius);

	// Set color
	memcpy(circle->color, color, COLOR_LEN * sizeof(float));

	glBindVertexArray(0);

	if (circles == nullptr) circles = circle;
	else circles->next = circle;

	circle->next = nullptr;
	return circle;
}

// Frees GPU and host memory.
void free_circle(circle_obj *circle) {
	glDeleteVertexArrays(1, &circle->vao);
	glDeleteBuffers(1, &circle->vbo);
	delete circle;
}

// Handle key input
void process_input(GLFWwindow *window) {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
    std::cout << "Space bar pressed!" << std::endl;
}
