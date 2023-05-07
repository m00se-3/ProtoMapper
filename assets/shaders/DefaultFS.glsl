#version 320 core

in vec4 out_color;

out vec4 final_color;

void main(void)
{
	final_color = out_color;
}