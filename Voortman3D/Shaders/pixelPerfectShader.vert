#version 450

layout (location = 0) in vec3 inPos;
layout (location = 1) in vec3 inNormal;
layout (location = 2) in vec3 inColor;

layout (set = 0, binding = 0) uniform UBO {
	mat4 projection;
	mat4 view;
	mat4 model;
} ubo;

layout (set = 1, binding = 0) uniform Node {
	mat4 matrix;
} node;

void main() {
	gl_Position = ubo.projection * ubo.view * ubo.model * vec4(inPos, 1.0);
}