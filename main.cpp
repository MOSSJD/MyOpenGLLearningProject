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

#include "shader.h"
#include "stb_image.h"
#include "vertices.h"

float mixRatio = .6;

const int WIDTH = 1920;
const int HEIGHT = 1080;
const float V_FOV = 78;

glm::vec3 cameraPos = glm::vec3(0, 0, 3.0f);
glm::vec3 cameraFront = glm::vec3(0, 0, -1.0f);
glm::vec3 cameraUp = glm::vec3(0, 1.0f, 0);

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
}

void processInput(GLFWwindow* window) {
	float currentTime = glfwGetTime();
	static float lastTime = -1;
	static float lastTimePressed = -1;
	if (lastTimePressed < 0) {
		lastTimePressed = glfwGetTime();
	}
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
	}
	else if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS && currentTime - lastTimePressed > .2) {
		lastTimePressed = currentTime;
		mixRatio += .1;
		if (mixRatio > 1) {
			mixRatio = 1;
		}
	}
	else if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS && currentTime - lastTimePressed > .2) {
		lastTimePressed = currentTime;
		mixRatio -= .1;
		if (mixRatio < 0) {
			mixRatio = 0;
		}
	}

	// Handle the camera movemont
	if (lastTime < 0) {
		lastTime = currentTime;
	}
	float deltaTime = currentTime - lastTime;
	const float cameraSpeed = 3.0f;
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
		cameraPos += cameraFront * cameraSpeed * deltaTime;
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		cameraPos -= cameraFront * cameraSpeed * deltaTime;
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
		cameraPos += glm::normalize(glm::cross(cameraUp, cameraFront)) * cameraSpeed * deltaTime;
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
		cameraPos -= glm::normalize(glm::cross(cameraUp, cameraFront)) * cameraSpeed * deltaTime;
	}
	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
		cameraPos += cameraUp * cameraSpeed * deltaTime;
	}
	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
		cameraPos -= cameraUp * cameraSpeed * deltaTime;
	}
	lastTime = currentTime;
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
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	unsigned int indices[] = {
		0, 1, 2,
		2, 3, 0,
	};

	// Bind vertices to a vertex array object
	unsigned int VAO;
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	// Bind vertices to a buffer
	unsigned int VBO;
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	//unsigned int EBO;
	//glGenBuffers(1, &EBO);
	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	//glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
	// Specify the layout of the vertex data
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	//glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	//glEnableVertexAttribArray(2);

	// Setting up textures
	
	// Texture1
	// Generating a texture
	unsigned int texture1;
	glGenTextures(1, &texture1);
	glBindTexture(GL_TEXTURE_2D, texture1);
	// Set Texture wraping configurations
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// Loading a texture image
	int width, height, nrChannels;
	stbi_set_flip_vertically_on_load(true);
	unsigned char* data = stbi_load("container.jpg", &width, &height, &nrChannels, 0);
	try {
		if (data == nullptr) {
			throw ME::MyError("Fail to load texture image");
		}
	}
	catch (const ME::MyError& e) {
		std::cerr << "Error on loading texture:\n" << e.what() << '\n';
		return -1;
	}
	// Loading textures and generating mipmaps
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
	glGenerateMipmap(GL_TEXTURE_2D);
	stbi_image_free(data);

	// Texture2
	// Generating a texture
	unsigned int texture2;
	glGenTextures(1, &texture2);
	glBindTexture(GL_TEXTURE_2D, texture2);
	// Set Texture wraping configurations
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	data = stbi_load("awesomeface.png", &width, &height, &nrChannels, 0);
	try {
		if (data == nullptr) {
			throw ME::MyError("Fail to load texture image");
		}
	}
	catch (const ME::MyError& e) {
		std::cerr << "Error on loading texture:\n" << e.what() << '\n';
		return -1;
	}
	// Loading textures and generating mipmaps
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
	glGenerateMipmap(GL_TEXTURE_2D);
	stbi_image_free(data);

	// Create and compile shaders
	std::unique_ptr<ME::Shader> shader;
	try {
		shader = std::make_unique<ME::Shader>("vs.vert", "fs.frag");
	}
	catch (const ME::ShaderException &e) {
		std::cerr << "Error on creating shader:\n" << e.what() << '\n';
		return -1;
	}

	// Set the rendering mode
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	// Compute fps
	int frameCount = 0;
	double startTime = glfwGetTime();

	// Setup texture sampler ID
	shader->use();
	shader->setInt("texture1", 0);
	shader->setInt("texture2", 1);


	// Enable depth testing
	glEnable(GL_DEPTH_TEST);

	while (!glfwWindowShouldClose(window)) {
		// Calculate fps
		frameCount++;
		double fps = frameCount / (glfwGetTime() - startTime);
		std::string title = std::string("fps: ") + std::to_string(fps);
		glfwSetWindowTitle(window, title.c_str());
		// Process input
		processInput(window);

		// Clear the screen	
		glClearColor(.2f, .3f, .3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Bind Texture
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture1);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, texture2);

		// Draw the triangles
		shader->use();
		shader->setFloat("mixRatio", mixRatio);


		// Setup the model, view and projection matrices
		glm::mat4 view(1.0f);
		//glm::vec3 camPos(0);
		//float currentTime = glfwGetTime();
		//float radius = 6.0f;
		//camPos.x = radius * std::cos(currentTime);
		//camPos.z = -radius * std::sin(currentTime);
		view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
		glm::mat4 projection = glm::perspective(glm::radians(V_FOV), static_cast<float>(WIDTH) / HEIGHT, .01f, 100.0f);

		// Set the projection matrices
		shader->setMatrix4f("view", view);
		shader->setMatrix4f("projection", projection);

		glBindVertexArray(VAO);
		for (int i = 0; i < 10; i++) {

			glm::mat4 model(1.0f);
			model = glm::translate(model, cubePositions[i]);
			model = glm::rotate(model, glm::radians(50.0f + 20.0f * i), glm::vec3(1.0f, .3f, .5f));
			shader->setMatrix4f("model", model);
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}
		//glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
		
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}