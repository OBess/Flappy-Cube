#version 330

layout (location = 0) in vec3 position;
uniform vec3 newPos;

void main()
{
	gl_Position = vec4(position + newPos, 1.0f);
}