#version 330

in vec3 a_Position;

void main()
{
	vec4 newPosition = vec4(a_Position, 1);
	//newPosition.x+=0.3; //위치조정

	gl_Position = newPosition;
}
