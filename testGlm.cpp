#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

int main() {
	glm::mat4 trans = glm::mat4(1);
	trans = glm::rotate(trans, glm::radians(90.0f), glm::vec3(0, 0, 1));
	trans = glm::scale(trans, glm::vec3(.5f, .5f, .5f));
	glm::normalize(glm::vec3(1, 2, 3));
	//std::cout << vec.x << ' ' << vec.y << ' ' << vec.z << '\n';
	return 0;
}