#version 430 core

in vec4 out_color;
in vec2 textureUV;

uniform sampler2D textureData;

out vec4 final_color;

void main()
{
	final_color = texture(textureData, textureUV) * out_color;
}