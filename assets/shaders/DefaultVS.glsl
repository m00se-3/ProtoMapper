#version 430 core

layout (location = 0) in vec2 position;
layout (location = 1) in vec2 texCoords;
layout (location = 2) in vec4 color;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec4 out_color;
out vec2 textureUV;

void main()
{
	out_color = color;
	textureUV = texCoords;
	gl_Position = projection * view * model * vec4(position.x, position.y, 1.0, 1.0);
}