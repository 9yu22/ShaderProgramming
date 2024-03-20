#version 330

in vec3 a_Position; //attribute: vs의 입력값

void main()
{
	vec4 newPosition = vec4(a_Position, 1);
	
	gl_Position = newPosition;
}
