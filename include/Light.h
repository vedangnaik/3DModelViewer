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
bool operator== (PointLight a1, PointLight a2) {
	if (a1.position == a2.position &&
		a1.color == a2.color && 
		a1.attConstant == a2.attConstant && 
		a1.attLinear == a2.attLinear &&
		a1.attQuadratic == a2.attQuadratic) {
		return true;
	}
	else {
		return false;
	}
}

struct DirectionalLight {
	glm::vec3 direction;
	glm::vec3 color;
};

struct SpotLight {
	glm::vec3 position;
	glm::vec3 direction;
	float cosineInnerCutoff;
	float cosineOuterCutoff;
	glm::vec3 color;
};
bool operator== (SpotLight a1, SpotLight a2) {
	if (a1.position == a2.position &&
		a1.direction == a2.direction &&
		a1.color == a2.color &&
		a1.cosineInnerCutoff == a2.cosineInnerCutoff &&
		a1.cosineOuterCutoff == a2.cosineOuterCutoff) {
		return true;
	}
	else {
		return false;
	}
}