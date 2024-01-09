#version 430

out vec4 color;
uniform bool isLine;
in vec3 myPos;
in vec3 myColor;
uniform vec2 centerPoint;

void main(void)
{
	if(isLine)
	{
		color = vec4(centerPoint,1.0 , 1.0);
	}
	else
	{
		color = vec4(myColor , 1.0);
	}
}