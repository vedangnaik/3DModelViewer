#version 450 core

layout (location = 0) in vec3 inPosition;
layout (location = 1) in vec3 inNormal;
layout (location = 2) in vec3 inTextureCoord;

out vec3 v2fNormal;
out vec3 v2fTextureCoord;
out vec3 v2fWorldFragmentPosition;

uniform mat4 model;
uniform mat3 inverseModel;
uniform mat4 view;
uniform mat4 projection;

void main() {
	v2fNormal = mat3(model) * inNormal;
	v2fTextureCoord = inTextureCoord;
	v2fWorldFragmentPosition = vec3(model * vec4(inPosition, 1.0));

	gl_Position = projection * view * model * vec4(inPosition, 1.0);
}