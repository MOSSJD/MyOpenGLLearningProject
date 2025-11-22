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
	camera.setMovementSpeed(2);
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
	GLuint sceneVAO;
	glGenVertexArrays(1, &sceneVAO);
	glBindVertexArray(sceneVAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);
	// Setting up cube VAO
	GLuint lightCubeVAO;
	glGenVertexArrays(1, &lightCubeVAO);
	glBindVertexArray(lightCubeVAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// Loading textures
	std::unique_ptr<ME::Texture> diffuseTexture;
	try {
		std::unique_ptr<ME::Texture> tempTexure = std::make_unique<ME::Texture>("container2.png");
		diffuseTexture = std::move(tempTexure);
	}
	catch (ME::MyError e) {
		std::cerr << "Error on loading diffuse texture:" << e.what() << std::endl;
	}
	std::unique_ptr<ME::Texture> specularTexture;
	try {
		std::unique_ptr<ME::Texture> tempTexure = std::make_unique<ME::Texture>("container2_specular.png");
		specularTexture = std::move(tempTexure);
	}
	catch (ME::MyError e) {
		std::cerr << "Error on loading specular texture:" << e.what() << std::endl;
	}

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
	// Setting block materials
	lightingShader->setFloat("material.shininess", 16.f);
	// Setting light colors
	lightingShader->setVec3("light.specular", glm::vec3(1.f, 1.f, 1.f)); 
	// fps
	int frameCount = 0;
	double startTime = glfwGetTime();
	// calculating light rotation
	glm::vec3 rotateCenter(1.f, 2, .5f);
	float rotateRadius = 2.0f;
	float lastTime = glfwGetTime();
	float rotateSpeed = 2;
	float theta = 0;
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
		// Drawing triangles
		// Setting view, and projection matrix
		glm::mat4 view = camera.getViewMatrix();
		glm::mat4 projection = camera.getProjectionMatrix(WIDTH, HEIGHT);
		// 渲染场景模型
		// Passing MVP matrices
		lightingShader->use();
		// Setting light properties
		lightingShader->setVec3("light.position",  camera.pos);
		lightingShader->setVec3("light.direction", camera.front);
		lightingShader->setFloat("light.cutOff",   glm::cos(glm::radians(12.5f)));
		glm::vec3 lightColor = glm::vec3(1.0f);
		glm::vec3 diffuseColor = lightColor; 
		glm::vec3 ambientColor = diffuseColor * .3f; 
		lightingShader->setVec3("light.ambient", ambientColor);
		lightingShader->setVec3("light.diffuse", diffuseColor);
		// Passing camera position and view and projection matrices
		lightingShader->setVec3("viewPos", camera.pos); 
		lightingShader->setMatrix4f("view", view);
		lightingShader->setMatrix4f("projection", projection);
		// Using texture
		glBindVertexArray(sceneVAO);
		// 为diffuse指定所使用的纹理单元（GL_TEXTURE0）
		lightingShader->setInt("material.diffuse", 0);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, diffuseTexture->getGlID());
		// 为specular指定所使用的纹理单元（GL_TEXTURE1）
		lightingShader->setInt("material.specular", 1);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, specularTexture->getGlID());
		// Rendering
		// 传递模型矩阵
		for(unsigned int i = 0; i < 10; i++)
		{
			glm::mat4 model = glm::mat4(1.0f);
			model = glm::translate(model, cubePositions[i]);
			float angle = 20.0f * i;
			model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
			lightingShader->setMatrix4f("model", model);

			glDrawArrays(GL_TRIANGLES, 0, 36);
		}

		// Backprocessing
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	glDeleteVertexArrays(1, &sceneVAO);
	glDeleteVertexArrays(1, &lightCubeVAO);
	glDeleteBuffers(1, &VBO);
	glfwTerminate();

	std::cout << "terminated.";
	return 0;
}