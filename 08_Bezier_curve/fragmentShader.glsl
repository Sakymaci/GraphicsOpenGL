#version 430

out vec4 color;
uniform bool isLine;
uniform bool isPoint;
uniform bool isCurve;

void main(void)
{
	if (isLine)
		color = vec4(1.0, 0.0, 0.0, 1.0);
	if (isCurve)
		color = vec4(0.0, 1.0, 0.0, 1.0);
	if (isPoint)
	{
	color = vec4(0.0, 0.0, 1.0, 1.0);
	vec2 circCoord = 2.0 * gl_PointCoord - 1.0;
        if (dot(circCoord, circCoord) > 1.0) {
            discard;
        }
	}
}