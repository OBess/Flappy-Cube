#version 330

layout (location = 0) in vec3 pos;

uniform vec2 offset;

void main()
{
	vec2 oldPos = vec2(pos.x + offset.x, pos.y + offset.y);
	gl_Position = vec4(mat2(1, 0, 0, -1) * oldPos, pos.z, 1.0f);
}