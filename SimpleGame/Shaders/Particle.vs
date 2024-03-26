#version 330

in vec3 a_Position;

const vec3 c_StartPos = vec3(-1, 0, 0);
const vec3 c_Velocity = vec3(2.0, 0, 0);
const vec3 c_ParaVelocity = vec3(2.0, 2.0, 0); //45도 각도로 오른쪽 위로 쏘아지는 형태
const vec2 c_2DGravity = vec2(0.0, -4.9);
uniform float u_Time = 0; //시간
uniform float u_Period = 2.0;
const float c_PI=3.141592;

void Line(){
	float newTime = abs(fract(u_Time/u_Period) - 0.5)*2.0; //다시 0으로 돌아와서 왼쪽에서 오른쪽으로 가는 운동을 반복함.
	vec4 newPosition;
	newPosition.xyz = (a_Position + c_StartPos) + c_Velocity * newTime;
	newPosition.w = 1;

	gl_Position = newPosition;
}

void Circle(){
	//float newTime = fract(u_Time/u_Period)*c_PI*2.0;//반시계
	float newTime=-fract(u_Time/u_Period)*c_PI*2.0;//시계 - 시험
	vec2 trans = vec2(cos(newTime), sin(newTime));
	vec4 newPosition;
	newPosition.xy=a_Position.xy+trans;
	newPosition.zw=vec2(0,1);
	gl_Position=newPosition;
}

void Parabola(){ //포물선
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

//삼각형그리기 - 시험: 1번꼭짓점, 2번꼭짓점, 3번꼭짓점을 const로 주고, 입력된 시간에 따라 왔다갔다하게
//0~1사이를 반복적으로 증가했다가 감소했다가를 이용
//if문을 이용하면 쉽게 할 수 있음. 
//어려운ver. if없이, 외부C++코드 수정없이, vs에서만으로 가능! 시험에 나옴.

//3주차온강 50분 여러개의 파티클
//랜덤으로 만드는건 효율적이지 않음 - 시간 등 많이 잡아먹음.