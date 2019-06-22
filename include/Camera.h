#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>

class Camera {
private:
	glm::vec3 position;
	glm::vec3 front;
	glm::vec3 up;
	glm::vec3 right;
	glm::vec3 worldUp;

	bool hasMouseMoved = false;
	float lastXPos;
	float lastYPos;
	bool WPressed = false;

	GLFWwindow* window;
	float movementSpeed = 0.05f;
	float mouseSpeed = 0.05f;

	static void cursorCallback(GLFWwindow* window, double xPos, double yPos) {
		// Get pointer to current camera and initialize the last position values
		Camera* cam = (Camera*)glfwGetWindowUserPointer(window);
		if (!cam->hasMouseMoved) {
			cam->lastXPos = xPos;
			cam->lastYPos = yPos;
			cam->hasMouseMoved = true;
		}

		// Pitch calculation
		glm::vec3 temp = glm::vec4(cam->front, 1.0f) * glm::rotate(
			glm::mat4(1.0f),
			glm::radians((float)(yPos - cam->lastYPos) * cam->mouseSpeed),
			cam->right
		);
		cam->front = glm::vec3(temp);
		cam->up = glm::normalize(glm::cross(cam->right, cam->front));
		cam->lastYPos = yPos;

		// Yaw calculation
		temp = glm::vec4(cam->front, 1.0f) * glm::rotate(
			glm::mat4(1.0f),
			glm::radians((float)(xPos - cam->lastXPos) * cam->mouseSpeed),
			cam->up
		);
		cam->front = glm::vec3(temp);
		cam->right = glm::normalize(glm::cross(cam->front, cam->worldUp));
		cam->lastXPos = xPos;
	}

	static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
		//std::cout << action << std::endl;
		Camera* cam = (Camera*)glfwGetWindowUserPointer(window);
		if (key == GLFW_KEY_W && action == GLFW_REPEAT) {
			cam->position += cam->movementSpeed * cam->front;
		}
		if (key == GLFW_KEY_S && action == GLFW_REPEAT) {
			cam->position -= cam->movementSpeed * cam->front;
		}
		if (key == GLFW_KEY_A && action == GLFW_REPEAT) {
			cam->position -= cam->movementSpeed * glm::cross(cam->front, cam->up);
		}
		if (key == GLFW_KEY_D && action == GLFW_REPEAT) {
			cam->position += cam->movementSpeed * glm::cross(cam->front, cam->up);
		}
	}


public:
	Camera(glm::vec3 position, glm::vec3 front, glm::vec3 worldUp, GLFWwindow* window) {
		this->position = position;
		this->front = glm::normalize(front);
		this->right = glm::normalize(glm::cross(front, worldUp));
		this->up = glm::normalize(glm::cross(this->right, front));
		this->window = window;
		this->worldUp = worldUp;

		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		glfwSetWindowUserPointer(window, this);
		glfwSetCursorPosCallback(window, cursorCallback);
		glfwSetKeyCallback(window, keyCallback);
	}

	glm::vec3 getPosition() { return this->position; }
	glm::vec3 getFront() { return this->front; }

	glm::mat4 getViewMat() {
		return glm::lookAt(this->position, this->position + this->front, this->up);
	}
};