#version 320 core

layout (location = 0) in vec2 position;
layout (location = 1) in vec2 texCoords;
layout (location = 2) in vec4 color;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec4 out_color;

void main(void)
{
	out_color = color;
	gl_Position = projection * view * model * vec4(postion.x, position.y, 1.f, 1.f);
}