#include "camera.h"

const float ME::Camera::MIN_V_FOV = 1;
const float ME::Camera::MAX_V_FOV = 78;
const float ME::Camera::DEFAULT_MOVEMENT_SPEED = 10;
const float ME::Camera::DEFAULT_MOUSE_SENSITIVITY = 1;
const float ME::Camera::MOUSE_SENSITIVITY_SCALING = 0.05;
const float ME::Camera::DEFAULT_ZOOM_SENSITIVITY = 5;

ME::Camera::Camera() {
	v_fov = MAX_V_FOV;
	pos = glm::vec3(0, 0, 3.0f);
	front = glm::vec3(0, 0, -1.0f);
	up = glm::vec3(0, 1.0f, 0);
	yaw = -90;
	pitch = 0;
	movementSpeed = DEFAULT_MOVEMENT_SPEED;
	mouseSensitivity = DEFAULT_MOUSE_SENSITIVITY * MOUSE_SENSITIVITY_SCALING;
	zoomSensitivity = DEFAULT_ZOOM_SENSITIVITY;
}
void ME::Camera::processMouseMovement(double xOffset, double yOffset) {


	xOffset *= mouseSensitivity;
	yOffset *= mouseSensitivity;

	yaw += xOffset;
	pitch += yOffset;

	if (pitch > 89.0f)
		pitch = 89.0f;
	if (pitch < -89.0f)
		pitch = -89.0f;

	glm::vec3 direction;
	direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	direction.y = sin(glm::radians(pitch));
	direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	front = glm::normalize(direction);
}

void ME::Camera::setMovementSpeed(float speed) {
	movementSpeed = speed;
}
void ME::Camera::processZooming(double yOffset) {
	v_fov -= (float)yOffset * zoomSensitivity;
	if (v_fov < MIN_V_FOV)
		v_fov = MIN_V_FOV;
	if (v_fov > MAX_V_FOV)
		v_fov = MAX_V_FOV;
}

void ME::Camera::processCameraMovement(Direction direction, float deltaTime) {
	switch (direction)
	{
	case ME::Camera::Direction::UP:
		pos += up * movementSpeed * deltaTime;
		break;
	case ME::Camera::Direction::DOWN:
		pos -= up * movementSpeed * deltaTime;
		break;
	case ME::Camera::Direction::FORWARD:
		pos += front * movementSpeed * deltaTime;
		break;
	case ME::Camera::Direction::BACKWARD:
		pos -= front * movementSpeed * deltaTime;
		break;
	case ME::Camera::Direction::LEFT:
		pos += glm::normalize(glm::cross(up, front)) * movementSpeed * deltaTime;
		break;
	case ME::Camera::Direction::RIGHT:
		pos -= glm::normalize(glm::cross(up, front)) * movementSpeed * deltaTime;
		break;
	default:
		break;
	}
}

glm::mat4 ME::Camera::getViewMatrix() const {
	return glm::lookAt(pos, pos + front, up);
}
glm::mat4 ME::Camera::getProjectionMatrix(const float& screenWidth, const float& screenHeight) const {
	return glm::perspective(glm::radians(v_fov), static_cast<float>(screenWidth) / screenHeight, .01f, 100.0f);
}