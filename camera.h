#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace ME {
	class Camera {
	private:
		static const float MIN_V_FOV;
		static const float MAX_V_FOV;
		static const float DEFAULT_MOVEMENT_SPEED;
		static const float DEFAULT_MOUSE_SENSITIVITY;
		static const float MOUSE_SENSITIVITY_SCALING;
		static const float DEFAULT_ZOOM_SENSITIVITY;
	public:
		enum class Direction
		{
			UP, DOWN, FORWARD, BACKWARD, LEFT, RIGHT
		};
	public:
		Camera();
		glm::mat4 getViewMatrix() const;
		glm::mat4 getProjectionMatrix(const float& screenWidth, const float& screenHeight) const;
		void processMouseMovement(double xOffset, double yOffset);
		void processCameraMovement(Direction direction, float deltaTime);
		void processZooming(double yOffset);
		void setMovementSpeed(float speed);
		glm::vec3 pos;
	private:
		float yaw;
		float pitch;
		glm::vec3 up;
		glm::vec3 front;
		float v_fov;
		float movementSpeed;
		float mouseSensitivity;
		float zoomSensitivity;
	};
}