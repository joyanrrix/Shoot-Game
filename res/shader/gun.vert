#version 330 core

layout (location = 0) in vec3 aPosition;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;

out vec3 Normal;
out vec2 TexCoord;
out vec3 Position;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

void main() {
	Position = vec3(model * vec4(aPosition, 1.0));
	Normal = mat3(transpose(inverse(model))) * aNormal;
	TexCoord = aTexCoord;

	gl_Position = projection * view * vec4(Position, 1.0);
}