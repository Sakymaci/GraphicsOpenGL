#version 430

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;

uniform float offsetX;
uniform bool isLine;
uniform float symmetryLocation;
uniform vec2 centerPoint;
out vec3 myPos;
out vec3 myColor;

void main(void)
{
	if(isLine)
	{
		gl_Position = vec4(aPos.x , aPos.y+symmetryLocation , aPos.z, 1.0);
		myPos = aPos;
		myColor = aColor;
	}
	else 
	{
		gl_Position = vec4(aPos.x + offsetX, aPos.y, aPos.z, 1.0);
		myPos = aPos;
		myColor = aColor;
	}
}