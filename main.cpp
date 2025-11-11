#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <memory>
#include <cmath>
#include <string>
#include <algorithm>

#include "camera.h"
#include "shader.h"
#include "stb_image.h"
#include "texture.h"
#include "vertices.h"

const int WIDTH = 1920;
const int HEIGHT = 1080;

glm::vec3 lightPos(1.2f, 1.0f, 2.0f);
float specularStrength = 0.5;

ME::Camera camera = ME::Camera();

void framebufferSizeCallback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
}
void mouseCallback(GLFWwindow* window, double xPos, double yPos)
{
	static float lastX, lastY;
	static bool firstMouse;
	if (firstMouse)
	{
		lastX = xPos;
		lastY = yPos;
		firstMouse = false;
	}

	float xOffset = xPos - lastX;
	float yOffset = lastY - yPos;
	lastX = xPos;
	lastY = yPos;
	camera.processMouseMovement(xOffset, yOffset);
}
void scrollCallback(GLFWwindow* window, double xOffset, double yOffset)
{
	camera.processZooming(yOffset);
}

void processInput(GLFWwindow* window) {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
	}

	// Handle camera movemont

	float currentTime = glfwGetTime();
	static float lastTime = -1;
	if (lastTime < 0) {
		lastTime = currentTime;
	}
	float deltaTime = currentTime - lastTime;
	lastTime = currentTime;

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
		camera.processCameraMovement(ME::Camera::Direction::FORWARD, deltaTime);
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		camera.processCameraMovement(ME::Camera::Direction::BACKWARD, deltaTime);
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
		camera.processCameraMovement(ME::Camera::Direction::LEFT, deltaTime);
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
		camera.processCameraMovement(ME::Camera::Direction::RIGHT, deltaTime);
	}
	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
		camera.processCameraMovement(ME::Camera::Direction::DOWN, deltaTime);
	}
	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
		camera.processCameraMovement(ME::Camera::Direction::UP, deltaTime);
	}
}

int main() {

	// Initialization
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Create a window
	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "LearnOpenGL", NULL, NULL);
	if (window == NULL) {
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	// Set the viewport
	glViewport(0, 0, WIDTH, HEIGHT);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
	glfwSetCursorPosCallback(window, mouseCallback);
	glfwSetScrollCallback(window, scrollCallback);
	// Set the rendering mode
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	// Enable depth testing
	glEnable(GL_DEPTH_TEST);

	// Setting up VBO
	GLuint VBO;
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	// Setting up light VAO
	GLuint lightVAO;
	glGenVertexArrays(1, &lightVAO);
	glBindVertexArray(lightVAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	// Setting up cube VAO
	GLuint cubeVAO;
	glGenVertexArrays(1, &cubeVAO);
	glBindVertexArray(cubeVAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// Create and compile shaders
	std::unique_ptr<ME::Shader> lightingShader;
	try {
		lightingShader = std::make_unique<ME::Shader>("lighting.vert", "lighting.frag");
	}
	catch (const ME::ShaderException &e) {
		std::cerr << "Error on creating shader:\n" << e.what() << '\n';
		return -1;
	}
	std::unique_ptr<ME::Shader> lightCubeShader;
	try {
		lightCubeShader = std::make_unique<ME::Shader>("lightCube.vert", "lightCube.frag");
	}
	catch (const ME::ShaderException& e) {
		std::cerr << "Error on creating shader:\n" << e.what() << '\n';
		return -1;
	}

	lightingShader->use();
	lightingShader->setVec3("objectColor", glm::vec3(1.0f, 0.5f, 0.31f));
	lightingShader->setVec3("lightColor", glm::vec3(1.0f, 1.0f, 1.0f));
	lightingShader->setVec3("lightPos", lightPos);

	// fps
	int frameCount = 0;
	double startTime = glfwGetTime();
	while (!glfwWindowShouldClose(window)) {
		// Calculate fps
		frameCount++;
		double fps = frameCount / (glfwGetTime() - startTime);
		std::string title = std::string("fps: ") + std::to_string(fps);
		glfwSetWindowTitle(window, title.c_str());
		// Process input
		processInput(window);

		// Clear the screen	
		glClearColor(0, 0, 0, 1);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Draw the triangles
		
		// Set view, projection and model matrix uniforms
		glm::mat4 view = camera.getViewMatrix();
		glm::mat4 projection = camera.getProjectionMatrix(WIDTH, HEIGHT);
		
		// Passing MVP matrices
		lightingShader->use();
		lightingShader->setVec3("viewPos", camera.pos); 
		lightingShader->setMatrix4f("view", view);
		lightingShader->setMatrix4f("projection", projection);
		glm::mat4 model = glm::mat4(1.0f);
		lightingShader->setMatrix4f("model", model);
		glBindVertexArray(lightVAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		lightCubeShader->use();
		lightCubeShader->setMatrix4f("view", view);
		lightCubeShader->setMatrix4f("projection", projection);
		model = glm::mat4(1.0f);
		model = glm::translate(model, lightPos);
		model = glm::scale(model, glm::vec3(0.2f));
		lightCubeShader->setMatrix4f("model", model);
		glBindVertexArray(cubeVAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	glDeleteVertexArrays(1, &lightVAO);
	glDeleteVertexArrays(1, &cubeVAO);
	glDeleteBuffers(1, &VBO);
	std::cout << "terminated.";

	glfwTerminate();
	return 0;
}