#version 330

in vec3 a_Position;

const vec3 c_StartPos = vec3(-1, 0, 0);
const vec3 c_Velocity = vec3(2.0, 0, 0);
const vec3 c_ParaVelocity = vec3(2.0, 2.0, 0); //45�� ������ ������ ���� ������� ����
const vec2 c_2DGravity = vec2(0.0, -4.9);
uniform float u_Time = 0; //�ð�
uniform float u_Period = 2.0;
const float c_PI=3.141592;

void Line(){
	float newTime = abs(fract(u_Time/u_Period) - 0.5)*2.0; //�ٽ� 0���� ���ƿͼ� ���ʿ��� ���������� ���� ��� �ݺ���.
	vec4 newPosition;
	newPosition.xyz = (a_Position + c_StartPos) + c_Velocity * newTime;
	newPosition.w = 1;

	gl_Position = newPosition;
}

void Circle(){
	//float newTime = fract(u_Time/u_Period)*c_PI*2.0;//�ݽð�
	float newTime=-fract(u_Time/u_Period)*c_PI*2.0;//�ð� - ����
	vec2 trans = vec2(cos(newTime), sin(newTime));
	vec4 newPosition;
	newPosition.xy=a_Position.xy+trans;
	newPosition.zw=vec2(0,1);
	gl_Position=newPosition;
}

void Parabola(){ //������
	float newTime = fract(u_Time/u_Period);
	float t = newTime;
	float tt = t*t;
	vec4 newPosition;
	float transX = (a_Position.x + c_StartPos.x) + c_ParaVelocity.x * newTime + 0.5 * c_2DGravity.x * tt;
	float transY = (a_Position.y + c_StartPos.y) + c_ParaVelocity.y * newTime + 0.5 * c_2DGravity.y * tt;
	newPosition.xy = vec2(transX, transY);
	newPosition.zw=vec2(0,1);
	gl_Position=newPosition;
}

void Basic(){
	vec4 newPosition = vec4(a_Position, 1);
	gl_Position=newPosition;
}

void main()
{
	//Line();
	//Circle();
	Parabola();
	//Basic();
}

//�ﰢ���׸��� - ����: 1��������, 2��������, 3���������� const�� �ְ�, �Էµ� �ð��� ���� �Դٰ����ϰ�
//0~1���̸� �ݺ������� �����ߴٰ� �����ߴٰ��� �̿�
//if���� �̿��ϸ� ���� �� �� ����. 
//�����ver. if����, �ܺ�C++�ڵ� ��������, vs���������� ����! ���迡 ����.

//3�����°� 50�� �������� ��ƼŬ
//�������� ����°� ȿ�������� ���� - �ð� �� ���� ��Ƹ���.