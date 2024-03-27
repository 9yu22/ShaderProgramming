#include "stdafx.h"
#include "Renderer.h"

Renderer::Renderer(int windowSizeX, int windowSizeY)
{
	Initialize(windowSizeX, windowSizeY);
}


Renderer::~Renderer()
{
}

void Renderer::Initialize(int windowSizeX, int windowSizeY)
{
	//Set window size
	m_WindowSizeX = windowSizeX;
	m_WindowSizeY = windowSizeY;

	//Load shaders
	m_SolidRectShader = CompileShaders("./Shaders/SolidRect.vs", "./Shaders/SolidRect.fs");

	m_ParticleShader = CompileShaders("./Shaders/Particle.vs", "./Shaders/Particle.fs");

	m_ParticleCloudShader = CompileShaders("./Shaders/ParticleCloud.vs", "./Shaders/ParticleCloud.fs");
	
	//Create VBOs
	CreateVertexBufferObjects();

	//Create Particle Cloud
	CreateParticleCloud(1000);

	if (m_SolidRectShader > 0 && m_VBORect > 0)
	{
		m_Initialized = true;
	}
}

bool Renderer::IsInitialized()
{
	return m_Initialized;
}

void Renderer::CreateVertexBufferObjects()
{
	float rect[]
		=
	{
		-1.f / m_WindowSizeX, -1.f / m_WindowSizeY, 0.f, -1.f / m_WindowSizeX, 1.f / m_WindowSizeY, 0.f, 1.f / m_WindowSizeX, 1.f / m_WindowSizeY, 0.f, //Triangle1
		-1.f / m_WindowSizeX, -1.f / m_WindowSizeY, 0.f,  1.f / m_WindowSizeX, 1.f / m_WindowSizeY, 0.f, 1.f / m_WindowSizeX, -1.f / m_WindowSizeY, 0.f, //Triangle2
	};

	glGenBuffers(1, &m_VBORect);
	glBindBuffer(GL_ARRAY_BUFFER, m_VBORect);
	glBufferData(GL_ARRAY_BUFFER, sizeof(rect), rect, GL_STATIC_DRAW);

	float vertices[] = { 0.0f,0.0f,0.0f, 1.0f,0.0f,0.0f, 1.0f,1.0f,0.0f, 0.0f,1.0f,0.0f };

	glGenBuffers(1, &m_TestVBO);// id생성(버퍼를 1개 생성, id=m_TestVBO)
	glBindBuffer(GL_ARRAY_BUFFER, m_TestVBO); // ARRAY_BUFFER를 사용하는 작업대를 생성. 이 작업대에서 m_TestVBO를 다룰 예정. *Bind:용도를 구체화
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW); // GL_ARRAY_BUFFER작업대에 있는것을, vertices사이즈만큼, GL_STATIC_DRAW용도로 사용하겠다.
	//GPU memory에 넣을 데이터 만들기 끝 => 따라서, 이제 이 데이터를 사용하는 코드를 작성하면됨.

	float vertices_prac[] = { 0.0f,0.0f,0.0f, 1.0f,1.0f,0.0f, 0.0f,1.0f,0.0f };
	glGenBuffers(1, &VBO_prac);
	glBindBuffer(GL_ARRAY_BUFFER, VBO_prac);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices_prac), vertices_prac, GL_STATIC_DRAW);



	float size = 0.05;
	float ParticleVerts[] = { -size, -size, 0,
							   size,  size, 0, 
							  -size,  size, 0,

							  -size, -size, 0, 
							   size, -size, 0,
							   size,  size, 0};
	glGenBuffers(1, &m_ParticleVBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_ParticleVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(ParticleVerts), ParticleVerts, GL_STATIC_DRAW);

}

void Renderer::AddShader(GLuint ShaderProgram, const char* pShaderText, GLenum ShaderType)
{
	//쉐이더 오브젝트 생성
	GLuint ShaderObj = glCreateShader(ShaderType);

	if (ShaderObj == 0) {
		fprintf(stderr, "Error creating shader type %d\n", ShaderType);
	}

	const GLchar* p[1];
	p[0] = pShaderText;
	GLint Lengths[1];
	Lengths[0] = strlen(pShaderText);
	//쉐이더 코드를 쉐이더 오브젝트에 할당
	glShaderSource(ShaderObj, 1, p, Lengths);

	//할당된 쉐이더 코드를 컴파일
	glCompileShader(ShaderObj);

	GLint success;
	// ShaderObj 가 성공적으로 컴파일 되었는지 확인
	glGetShaderiv(ShaderObj, GL_COMPILE_STATUS, &success);
	if (!success) {
		GLchar InfoLog[1024];

		//OpenGL 의 shader log 데이터를 가져옴
		glGetShaderInfoLog(ShaderObj, 1024, NULL, InfoLog);
		fprintf(stderr, "Error compiling shader type %d: '%s'\n", ShaderType, InfoLog);
		printf("%s \n", pShaderText);
	}

	// ShaderProgram 에 attach!!
	glAttachShader(ShaderProgram, ShaderObj);
}

bool Renderer::ReadFile(char* filename, std::string *target)
{
	std::ifstream file(filename);
	if (file.fail())
	{
		std::cout << filename << " file loading failed.. \n";
		file.close();
		return false;
	}
	std::string line;
	while (getline(file, line)) {
		target->append(line.c_str());
		target->append("\n");
	}
	return true;
}

GLuint Renderer::CompileShaders(char* filenameVS, char* filenameFS)
{
	GLuint ShaderProgram = glCreateProgram(); //빈 쉐이더 프로그램 생성

	if (ShaderProgram == 0) { //쉐이더 프로그램이 만들어졌는지 확인
		fprintf(stderr, "Error creating shader program\n");
	}

	std::string vs, fs;

	//shader.vs 가 vs 안으로 로딩됨
	if (!ReadFile(filenameVS, &vs)) {
		printf("Error compiling vertex shader\n");
		return -1;
	};

	//shader.fs 가 fs 안으로 로딩됨
	if (!ReadFile(filenameFS, &fs)) {
		printf("Error compiling fragment shader\n");
		return -1;
	};

	// ShaderProgram 에 vs.c_str() 버텍스 쉐이더를 컴파일한 결과를 attach함
	AddShader(ShaderProgram, vs.c_str(), GL_VERTEX_SHADER);

	// ShaderProgram 에 fs.c_str() 프레그먼트 쉐이더를 컴파일한 결과를 attach함
	AddShader(ShaderProgram, fs.c_str(), GL_FRAGMENT_SHADER);

	GLint Success = 0;
	GLchar ErrorLog[1024] = { 0 };

	//Attach 완료된 shaderProgram 을 링킹함
	glLinkProgram(ShaderProgram);

	//링크가 성공했는지 확인
	glGetProgramiv(ShaderProgram, GL_LINK_STATUS, &Success);

	if (Success == 0) {
		// shader program 로그를 받아옴
		glGetProgramInfoLog(ShaderProgram, sizeof(ErrorLog), NULL, ErrorLog);
		std::cout << filenameVS << ", " << filenameFS << " Error linking shader program\n" << ErrorLog;
		return -1;
	}

	glValidateProgram(ShaderProgram);
	glGetProgramiv(ShaderProgram, GL_VALIDATE_STATUS, &Success);
	if (!Success) {
		glGetProgramInfoLog(ShaderProgram, sizeof(ErrorLog), NULL, ErrorLog);
		std::cout << filenameVS << ", " << filenameFS << " Error validating shader program\n" << ErrorLog;
		return -1;
	}

	glUseProgram(ShaderProgram);
	std::cout << filenameVS << ", " << filenameFS << " Shader compiling is done." << std::endl;

	return ShaderProgram;
}

void Renderer::DrawSolidRect(float x, float y, float z, float size, float r, float g, float b, float a)
{
	float newX, newY;

	GetGLPosition(x, y, &newX, &newY);

	//Program select
	glUseProgram(m_SolidRectShader);

	glUniform4f(glGetUniformLocation(m_SolidRectShader, "u_Trans"), newX, newY, 0, size);
	glUniform4f(glGetUniformLocation(m_SolidRectShader, "u_Color"), r, g, b, a);

	int attribPosition = glGetAttribLocation(m_SolidRectShader, "a_Position");
	glEnableVertexAttribArray(attribPosition);
	glBindBuffer(GL_ARRAY_BUFFER, m_VBORect);
	glVertexAttribPointer(attribPosition, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, 0);

	glDrawArrays(GL_TRIANGLES, 0, 6);

	glDisableVertexAttribArray(attribPosition);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Renderer::GetGLPosition(float x, float y, float *newX, float *newY)
{
	*newX = x * 2.f / m_WindowSizeX;
	*newY = y * 2.f / m_WindowSizeY;
}

void Renderer::CreateParticleCloud(int numParticles)
{
	float centerX, centerY;
	centerX = 0.f;
	centerY = 0.f;
	float size = 0.01f;
	int particleCount = numParticles;
	int vertexCount = particleCount * 6; //버텍스 갯수, 사각형을 만들어야 하니까 점이 6개.
	int floatCount = vertexCount * (3+1); //x, y, z니까 3개 + 각각의 startTime을 위해 추가

	float* vertices = NULL; //동적할당
	vertices = new float[floatCount];

	int index = 0;
	for (int i = 0; i < particleCount; i++) {

		float startTime = 8.f*((float)rand() / (float)RAND_MAX);

		centerX = ((float)rand() / (float)RAND_MAX) * 2.f - 1.f;
		centerY = ((float)rand() / (float)RAND_MAX) * 2.f - 1.f;
		
		vertices[index] = centerX - size; index++;
		vertices[index] = centerY - size; index++;
		vertices[index] = 0.f; index++;
		vertices[index] = startTime; index++;//vertex1

		vertices[index] = centerX + size; index++;
		vertices[index] = centerY + size; index++;
		vertices[index] = 0.f; index++;
		vertices[index] = startTime; index++;//vertex2

		vertices[index] = centerX - size; index++;
		vertices[index] = centerY + size; index++;
		vertices[index] = 0.f; index++; 
		vertices[index] = startTime; index++;//vertex3 
													//triangle1

		vertices[index] = centerX - size; index++;
		vertices[index] = centerY - size; index++;
		vertices[index] = 0.f; index++;
		vertices[index] = startTime; index++;
		vertices[index] = centerX + size; index++;
		vertices[index] = centerY - size; index++;
		vertices[index] = 0.f; index++;
		vertices[index] = startTime; index++;
		vertices[index] = centerX + size; index++;
		vertices[index] = centerY + size; index++;
		vertices[index] = 0.f; index++; 
		vertices[index] = startTime; index++;//triangle2
	}

	glGenBuffers(1, &m_ParticleCloudVBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_ParticleCloudVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * floatCount, vertices, GL_STATIC_DRAW);
	m_ParticleCloudVertexCount = vertexCount;
	delete[] vertices;

}

void Renderer::DrawTest()
{
	//Program select
	glUseProgram(m_SolidRectShader);

	int attribPosition = glGetAttribLocation(m_SolidRectShader, "a_Position");
	glEnableVertexAttribArray(attribPosition);
	
	glBindBuffer(GL_ARRAY_BUFFER, m_TestVBO);
	glVertexAttribPointer(attribPosition, 4, GL_FLOAT, GL_FALSE, sizeof(float) * 3, 0);//데이터를 사용해서 그릴때, 데이터들중 어떤데이터부터 몇개씩 사용할지 설정.
							//GL_FLOAT형태로 3개를 읽어온다. 0번지 부터 읽는다. 

	glDrawArrays(GL_TRIANGLES, 0, 3);//Primitive가 GL_TRIANGLES. 삼각형을 그린다. 0번째꺼 부터, 3개를 그린다. // 이 함수 호출 즉시 GPU가 동작한다.
	//glDrawArrays(GL_TRIANGLES, 1, 3);
	
	/*
	
	사각형 만들기
	//glBindBuffer(GL_ARRAY_BUFFER, VBO_prac);
	//glVertexAttribPointer(attribPosition, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, 0);
	//glDrawArrays(GL_TRIANGLES, 0, 3);

	*/

	glDisableVertexAttribArray(attribPosition);
}

void Renderer::DrawParticle()
{

	//Program select
	GLuint shader = m_ParticleShader;
	glUseProgram(shader);

	// 물리법칙 적용
	int ulTime = glGetUniformLocation(shader, "u_Time"); 
	glUniform1f(ulTime, m_ParticleTime);
	m_ParticleTime += 0.016;
	//
	int ulPeriod = glGetUniformLocation(shader, "u_Period");
	glUniform1f(ulPeriod, 2.0);

	int attribPosition = glGetAttribLocation(shader, "a_Position");
	glEnableVertexAttribArray(attribPosition);

	glBindBuffer(GL_ARRAY_BUFFER, m_ParticleVBO);
	glVertexAttribPointer(attribPosition, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, 0); //데이터를 사용해서 그릴때, 데이터들중 몇번지('0')부터 몇개씩 사용할지(3) 설정.

	glDrawArrays(GL_TRIANGLES, 0, 6);//Primitive가 GL_TRIANGLES. 삼각형을 그린다. 읽어들인 정보 중 0번째꺼 부터, 6개를 이용해서 삼각형을 그린다. // 이 함수 호출 즉시 GPU가 동작한다.
	glDisableVertexAttribArray(attribPosition);
}

void Renderer::DrawParticleCloud()
{

	//Program select
	GLuint shader = m_ParticleCloudShader;
	glUseProgram(shader);

	// 물리법칙 적용
	int ulTime = glGetUniformLocation(shader, "u_Time");
	glUniform1f(ulTime, m_ParticleTime);
	m_ParticleTime += 0.016;
	//
	int ulPeriod = glGetUniformLocation(shader, "u_Period");
	glUniform1f(ulPeriod, 5.0);

	int attribPosition = glGetAttribLocation(shader, "a_Position"); //안사용하면 -1이 들어감.
	glEnableVertexAttribArray(attribPosition);
	glBindBuffer(GL_ARRAY_BUFFER, m_ParticleCloudVBO);
	glVertexAttribPointer(attribPosition, 3, GL_FLOAT, GL_FALSE, sizeof(float) *4 , 0); //데이터를 사용해서 그릴때, 데이터들중 몇번지('0')부터 몇개씩 사용할지(3) 설정.
	                                                             //다음에 쓸 수 있는 정보가 4개 간격

	int attribStartTime = glGetAttribLocation(shader, "a_StartTime");
	glEnableVertexAttribArray(attribStartTime);
	glBindBuffer(GL_ARRAY_BUFFER, m_ParticleCloudVBO); //굳이 안해줘도되긴함. 근데 해주는게 좋음. 시간도 거의 안듦.
	glVertexAttribPointer(attribStartTime, 1, GL_FLOAT, GL_FALSE, sizeof(float) * 4, (GLvoid*)(sizeof(float)*3)); //데이터를 사용해서 그릴때, 데이터들중 몇번지('0')부터 몇개씩 사용할지(3) 설정.
	                                      //값(float 1개)         //다음에 쓸 수있는 정보는 4번째부터.. //3개를 건너띈 다음부터 읽어라.    

	glDrawArrays(GL_TRIANGLES, 0, m_ParticleCloudVertexCount);//Primitive가 GL_TRIANGLES. 삼각형을 그린다. 읽어들인 정보 중 0번째꺼 부터, 6개를 이용해서 삼각형을 그린다. // 이 함수 호출 즉시 GPU가 동작한다.
	glDisableVertexAttribArray(attribPosition);

}

