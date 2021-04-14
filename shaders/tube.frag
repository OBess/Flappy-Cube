#version 330

in vec2 txtrCoord;

out vec4 color;

uniform sampler2D txtr;

void main()
{
	color = texture(txtr, txtrCoord);
}