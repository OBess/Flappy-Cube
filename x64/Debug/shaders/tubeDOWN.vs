#version 330

layout (location = 0) in vec3 pos;

void main()
{
	vec2 oldPos = vec2(pos.x, pos.y);
	gl_Position = vec4(mat2(1, 0, 0, -1) * oldPos, 0.0f, 1.0f);
}