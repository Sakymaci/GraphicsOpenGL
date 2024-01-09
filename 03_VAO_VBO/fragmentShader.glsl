#version 430

in vec3 myPos;
in vec3 myColor;
out vec4 color;

void main(void)
{
//	color = vec4(myColor, (myPos.x - myPos.y)/0.5, 1.0);
//	color = vec4(myColor * (myPos.x + 1.0) / 2.0, 1.0);
	color = vec4(myColor * (1.0 - (myPos.x - myPos.y) / 2.0), 1.0);
}