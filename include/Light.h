#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

struct PointLight {
	glm::vec3 position;
	glm::vec3 color;
	float attConstant = 1.0f;
	float attLinear = 0.09f;
	float attQuadratic = 0.032f;
};