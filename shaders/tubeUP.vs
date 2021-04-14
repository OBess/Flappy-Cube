#version 330

layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 textureCoord;

out vec2 txtrCoord;

uniform vec2 offset;

void main()
{
	gl_Position = vec4(pos.x + offset.x, pos.y + offset.y, pos.z, 1.0f);

	txtrCoord = textureCoord;
}