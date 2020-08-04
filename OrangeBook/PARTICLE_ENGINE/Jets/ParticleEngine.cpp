#include<Windows.h>
#include<stdio.h>
#include<gl/glew.h>
#include<gl/GL.h>

#include"vmath.h"

using namespace vmath;

#define WIN_WIDTH 800
#define WIN_HEIGHT 600
#define VELOCITY_ARRAY 3
#define START_TIME_ARRAY 4

#pragma comment(lib,"opengl32.lib")
#pragma comment(lib,"glew32.lib")
#pragma comment(lib,"winmm.lib")

//Prototype of WndProc() declared gloabally
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

//Global variable declarations
HWND ghwnd = NULL;
HDC ghdc = NULL;
HGLRC ghrc = NULL;


static GLint arrayWidth, arrayHeight;
static GLfloat *verts = NULL;
static GLfloat *colors = NULL;
static GLfloat *velocities = NULL;
static GLfloat *statrtTimes = NULL;

GLuint uniform_background;


//5.2831853
GLfloat gAnglePyramid = 1800.0f;


GLfloat heightIncr = 0.0f;
GLuint heightUniform;

GLuint yPosUniform;
GLfloat yPos = 2.0;
GLfloat XRes = 100;
GLfloat YRes = 50;

ULONG lastTick = GetTickCount(), currTick = GetTickCount();
ULONG UPDATESPEED;
UINT frames = 0.0;
float fps = 60.0;
float g_speed=1.0;
GLuint gLocation;


DWORD dwStyle;
WINDOWPLACEMENT wpPrev = { sizeof(WINDOWPLACEMENT) };

bool gbActiveWindow = false;
bool gbEscapeKeyIsPressed = false;
bool gbFullScreen = false;

enum
{
	VDG_ATTRIBUTE_VERTEX = 0,
	VDG_ATTRIBUTE_COLOR,
	VDG_ATTRIBUTE_NORMAL,
	VDG_ATTRIBUTE_TEXTURE0,
	VDG_ATTRIBUTE_STARTTIME,
	VDG_ATTRUBUTE_VELOCITY

};

GLuint gVertexShaderObject;
GLuint gFragmentShaderObject;
GLuint gShaderProgramObject;

GLuint gVao_Cube;
GLuint gVbo_CubePosition;
GLuint gVbo_Cube_Normal;

GLuint gVaoParticle;
GLuint gVboParticlePosition;
GLuint gVboParticleColor;
GLuint gVboParticleVelocity;
GLuint gVboParticleStartTime;


GLuint gTexture_sampler_uniform;
GLuint gTexture_sampler_uniform1;
GLuint gTexture_sampler_uniform2;

GLuint mouseUniform;

GLfloat NoOfWaves = 5.2831853;
GLuint noWavesUniform;

GLuint timeUniform;
GLfloat time = 0.01f;
GLuint gMVPUniform;
GLuint updateXUniform;
GLuint updateX2Uniform;

GLfloat updateX = 0.0f;
GLfloat updateX2 = 0.0f;
GLfloat mouse;
//GLuint timeUniform;

GLuint gLKeyPressedUniform;
mat4 gPerspectiveProjectionMatrix;
GLuint gModelViewMatrixUniform, gProjectionMatrixUniform, gViewMatrixUniform;
GLuint gLdUniform, gKdUniform, gLightPositonUniform;
GLuint gUniformSkycolor, gUniformcloudColor, gUniformScale;

GLfloat ParticleTime = 0.0f;

int NumberOfPoint = 10000;
GLfloat skycolor[] = { 0.8f,0.8f,0.8f };
GLfloat cloudcolor[] = { 0.0f,0.0f,0.8f };
GLfloat scaleVal = 0.5f;

GLuint iResolutionUniform;

GLfloat gAngle = 0.0f;

bool gbAnimate;
bool gbLight;

FILE *gpFile = NULL;
void PlayWAV(HINSTANCE);
//main()
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdLine, int nCmdShow)
{
	void display();
	void initialize();
	void uninitialize();
	void spin();
	void update();
	void 	CalcFPS();
	void PlayWAV(HINSTANCE);

	WNDCLASSEX wndclass;
	HWND hwnd;
	MSG msg;
	TCHAR szClassName[] = TEXT("RTROGL");
	bool bDone = false;
	PlayWAV(hInstance);

	fopen_s(&gpFile, "LogFile.txt", "w");
	if (gpFile == NULL)
	{
		printf("Error opening file\n");
		exit(0);
	}

	//initializing members of wndclass
	wndclass.cbSize = sizeof(WNDCLASSEX);
	wndclass.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wndclass.cbClsExtra = 0;
	wndclass.cbWndExtra = 0;
	wndclass.hInstance = hInstance;
	wndclass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wndclass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndclass.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
	wndclass.lpfnWndProc = WndProc;
	wndclass.lpszClassName = szClassName;
	wndclass.lpszMenuName = NULL;


	//Registering class
	RegisterClassEx(&wndclass);

	hwnd = CreateWindowEx(WS_EX_APPWINDOW,
		szClassName,
		TEXT("OpenGL Programmable PipeLine Native Windowing"),
		WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_VISIBLE,
		0,
		0,
		WIN_WIDTH,
		WIN_HEIGHT,
		NULL,
		NULL,
		hInstance,
		NULL);

	ghwnd = hwnd;

	initialize();

	ShowWindow(hwnd, SW_SHOW);
	SetForegroundWindow(hwnd);
	SetFocus(hwnd);

	//Game Loop
	while (bDone == false)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
				bDone = true;
			else
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
		else
		{
			display();
			update();
			CalcFPS();

			if (gbAnimate == true)
				spin();

			if (gbActiveWindow == true)
			{
				if (gbEscapeKeyIsPressed == true)
					bDone = true;
			}
		}
	}

	uninitialize();
	fprintf(gpFile, "Exitting Main..\n");
	return((int)msg.wParam);
}


LRESULT CALLBACK WndProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	void display();
	void resize(int, int);
	void ToggleFullScreen(void);
	void uninitialize(void);

	static bool gbIsLKeyPressed = false;
	static bool gbIsAKeyPressed = false;
	//	static bool 

	fprintf(gpFile, "Entering WndProc..\n");

	switch (iMsg)
	{
	case WM_ACTIVATE:
		if (HIWORD(wParam) == 0)
			gbActiveWindow = true;
		else
			gbActiveWindow = false;
		break;

	case WM_SIZE:
		resize(LOWORD(lParam), HIWORD(lParam));
		break;

	case WM_KEYDOWN:
		switch (wParam)
		{
		case VK_ESCAPE:
			if (gbEscapeKeyIsPressed == false)
				gbEscapeKeyIsPressed = true;
			break;

		case 0x46:
			if (gbFullScreen == false)
			{
				ToggleFullScreen();
				gbFullScreen = true;
			}
			else
			{
				ToggleFullScreen();
				gbFullScreen = false;
			}
			break;
		case 0x41:	//'A' or 'a' pressed (Animate)
			if (gbIsAKeyPressed == false)
			{
				gbAnimate = true;
				gbIsAKeyPressed = true;
			}
			else
			{
				gbAnimate = false;
				gbIsAKeyPressed = false;
			}
			break;
		case 0x4c:		//for l or L key (Light)
			if (gbIsLKeyPressed == false)
			{
				gbLight = true;
				gbIsLKeyPressed = true;
			}
			else
			{
				gbLight = false;
				gbIsLKeyPressed = false;
			}
			break;
		}
		break;

	case WM_LBUTTONDOWN:
		break;

	case WM_CLOSE:
		uninitialize();
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	}
	return(DefWindowProc(hwnd, iMsg, wParam, lParam));
	fprintf(gpFile, "Exitting WndProc..\n");

}

void ToggleFullScreen(void)
{
	MONITORINFO mi;

	if (gbFullScreen == false)
	{
		dwStyle = GetWindowLong(ghwnd, GWL_STYLE);
		if (dwStyle & WS_OVERLAPPEDWINDOW)
		{
			mi = { sizeof(MONITORINFO) };
			if (GetWindowPlacement(ghwnd, &wpPrev) && GetMonitorInfo(MonitorFromWindow(ghwnd, MONITORINFOF_PRIMARY), &mi))
			{
				SetWindowLong(ghwnd, GWL_STYLE, dwStyle & ~WS_OVERLAPPEDWINDOW);
				SetWindowPos(ghwnd, HWND_TOP, mi.rcMonitor.left,
					mi.rcMonitor.top,
					mi.rcMonitor.right - mi.rcMonitor.left,
					mi.rcMonitor.bottom - mi.rcMonitor.top,
					SWP_NOZORDER | SWP_FRAMECHANGED);
			}
		}
		ShowCursor(FALSE);
	}
	else
	{
		SetWindowLong(ghwnd, GWL_STYLE, dwStyle | WS_OVERLAPPEDWINDOW);
		SetWindowPlacement(ghwnd, &wpPrev);
		SetWindowPos(ghwnd, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOOWNERZORDER | SWP_NOZORDER | SWP_FRAMECHANGED);
		ShowCursor(TRUE);
	}
}



void initialize(void)
{

	void resize(int, int);
	void uninitialize();
	void update();
	fprintf(gpFile, "Entering Initialize..\n");
	void createPoints(GLint, GLint);
	int LoadGLTextures(GLuint *, TCHAR[]);

	//Variable 
	PIXELFORMATDESCRIPTOR pfd;
	int iPixelFormatIndex;
	//GLint num;

	//code
	ZeroMemory(&pfd, sizeof(PIXELFORMATDESCRIPTOR));

	//initialize code
	pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
	pfd.nVersion = 1;
	pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.cColorBits = 8;
	pfd.cRedBits = 8;
	pfd.cGreenBits = 8;
	pfd.cBlueBits = 8;
	pfd.cAlphaBits = 8;

	ghdc = GetDC(ghwnd);

	iPixelFormatIndex = ChoosePixelFormat(ghdc, &pfd);
	if (iPixelFormatIndex == 0)
	{
		ReleaseDC(ghwnd, ghdc);
		ghdc = NULL;
	}
	if (SetPixelFormat(ghdc, iPixelFormatIndex, &pfd) == FALSE)
	{
		ReleaseDC(ghwnd, ghdc);
		ghdc = NULL;
	}

	ghrc = wglCreateContext(ghdc);
	if (ghrc == NULL)
	{
		ReleaseDC(ghwnd, ghdc);
		ghdc = NULL;
	}

	if (wglMakeCurrent(ghdc, ghrc) == NULL)
	{
		wglDeleteContext(ghrc);
		ghrc = NULL;
		ReleaseDC(ghwnd, ghdc);
		ghdc = NULL;
	}

	GLenum glew_error = glewInit();
	if (glew_error != GLEW_OK)
	{
		wglDeleteContext(ghrc);
		ghrc = NULL;
		ReleaseDC(ghwnd, ghdc);
		ghdc = NULL;
	}

	//*****VERTEX SHADER*******
	//create shader 
	gVertexShaderObject = glCreateShader(GL_VERTEX_SHADER);

	//provide source code to shader
	const GLchar * vertexShaderShaderCode =
		"#version 430 core" \
		"\n" \
		"in vec4 vPosition;" \
		
		"in vec4 vColor;" \
		"uniform mat4 u_mvp_matrix;" \
		"uniform float iTime;"\
		"in vec3 velocity;"\
		"in float startTime;"\
		"out vec4 Color;"\

		"void main(void)"\
		"{"\
		"vec4 vert;"\
		"float t = iTime - startTime;"\
		"if(t >= 0.0)" \
		"{"\
		"vert = vPosition + vec4(velocity * t,0.0);"\
		"vert.y -= 4.9 * t * t;"\
		"Color = vColor;"\
		"}"\
		"else"\
		"{"\
		"vert = vPosition;"\
		"}"\
		"gl_Position =u_mvp_matrix * vert;" \
		"}";



	glShaderSource(gVertexShaderObject, 1, (const GLchar**)&vertexShaderShaderCode, NULL);

	//compile shader
	glCompileShader(gVertexShaderObject);

	GLint iInfoLength = 0;
	GLint iShaderCompiledStatus = 0;
	char * szInfoLog = NULL;

	glGetShaderiv(gVertexShaderObject, GL_COMPILE_STATUS, &iShaderCompiledStatus);
	if (iShaderCompiledStatus == GL_FALSE)
	{
		glGetShaderiv(gVertexShaderObject, GL_INFO_LOG_LENGTH, &iInfoLength);
		if (iInfoLength > 0)
		{
			szInfoLog = (char *)malloc(iInfoLength);
			if (szInfoLog != NULL)
			{
				GLsizei written;
				glGetShaderInfoLog(gVertexShaderObject, iInfoLength, &written, szInfoLog);
				fprintf(gpFile, "Vertex Shader Compilation Log:%s\n", szInfoLog);
				free(szInfoLog);
				uninitialize();
				exit(0);
			}
		}
	}


												//*****FRAGMENT SHADER****
												gFragmentShaderObject = glCreateShader(GL_FRAGMENT_SHADER);

										//											//provide source code to shader
												const GLchar *fragmentShaderSourceCode =
													"#version 430 core" \
													"\n" \
													"in vec4 Color;"\
													"out vec4 Fragcolor;"\
													"void main(void)"\
													"{"\
													"Fragcolor = Color;"\
													"}";
													

	glShaderSource(gFragmentShaderObject, 1, (const GLchar**)&fragmentShaderSourceCode, NULL);

	//compile shader
	glCompileShader(gFragmentShaderObject);
	glGetShaderiv(gFragmentShaderObject, GL_COMPILE_STATUS, &iShaderCompiledStatus);
	if (iShaderCompiledStatus == GL_FALSE)
	{
		glGetShaderiv(gFragmentShaderObject, GL_INFO_LOG_LENGTH, &iShaderCompiledStatus);
		if (iInfoLength > 0)
		{
			szInfoLog = (char *)malloc(iInfoLength);
			if (szInfoLog != NULL)
			{
				GLsizei written;
				glGetShaderInfoLog(gFragmentShaderObject, iInfoLength, &written, szInfoLog);
				fprintf(gpFile, "Fragment shader compilation Log %s\n", szInfoLog);
				free(szInfoLog);
				uninitialize();
				exit(0);
			
			}
		}
	}


	//*****SHADER PROGRAM******
	//create shader program
	gShaderProgramObject = glCreateProgram();

	//attach vertex shader to shader program
	glAttachShader(gShaderProgramObject, gVertexShaderObject);

	//attach fragment shader to shader program
	glAttachShader(gShaderProgramObject, gFragmentShaderObject);

	glBindAttribLocation(gShaderProgramObject, VDG_ATTRIBUTE_VERTEX, "vPosition");
	glBindAttribLocation(gShaderProgramObject, VDG_ATTRUBUTE_VELOCITY, "velocity");
	glBindAttribLocation(gShaderProgramObject, VDG_ATTRIBUTE_STARTTIME, "startTime");
	glBindAttribLocation(gShaderProgramObject, VDG_ATTRIBUTE_COLOR, "vColor");


//	glBindAttribLocation(gShaderProgramObject, VDG_ATTRIBUTE_TEXTURE0, "vTexture0_coord");
	//link shader
	glLinkProgram(gShaderProgramObject);

	GLint iShaderProgramLinkStatus = 0;
	glGetProgramiv(gShaderProgramObject, GL_LINK_STATUS, &iShaderCompiledStatus);
	if (iShaderCompiledStatus == GL_FALSE)
	{
		glGetProgramiv(gShaderProgramObject, GL_INFO_LOG_LENGTH, &iInfoLength);
		if (iInfoLength > 0)
		{
			szInfoLog = (char*)malloc(iInfoLength);
			if (szInfoLog != NULL)
			{
				GLsizei written;
				glGetProgramInfoLog(gShaderProgramObject, iInfoLength, &written, szInfoLog);
				fprintf(gpFile, "Shader Program Link Status %s\n", szInfoLog);
				free(szInfoLog);
				uninitialize();
				exit(0);
			}
		}
	}

	//get uniform locations

	gMVPUniform = glGetUniformLocation(gShaderProgramObject, "u_mvp_matrix");
	timeUniform = glGetUniformLocation(gShaderProgramObject, "iTime");
	createPoints(1000, 1000);

	const GLfloat squareVertices[] =
	{
		////front
		1.0f, 1.0f, 1.0f,
		-1.0f, 1.0f, 1.0f,
		-1.0f, -1.0f, 1.0f,
		1.0f, -1.0f, 1.0f,


	};

	fprintf(gpFile, "After all vertices and normals..\n");



glGenVertexArrays(1, &gVaoParticle);
	glBindVertexArray(gVaoParticle);

	// position vbo
	glGenBuffers(1, &gVboParticlePosition);
	glBindBuffer(GL_ARRAY_BUFFER, gVboParticlePosition);
	glBufferData(GL_ARRAY_BUFFER, (arrayWidth * arrayHeight * 3 * sizeof(float)),verts, GL_STATIC_DRAW);
	glVertexAttribPointer(VDG_ATTRIBUTE_VERTEX, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(VDG_ATTRIBUTE_VERTEX);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	//color vbo
	glGenBuffers(1, &gVboParticleColor);
	glBindBuffer(GL_ARRAY_BUFFER, gVboParticleColor);
	glBufferData(GL_ARRAY_BUFFER, (arrayWidth * arrayHeight * 3 * sizeof(float)), colors, GL_STATIC_DRAW);
	glVertexAttribPointer(VDG_ATTRIBUTE_COLOR, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(VDG_ATTRIBUTE_COLOR);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	//vbo velocity
	glGenBuffers(1, &gVboParticleVelocity);
	glBindBuffer(GL_ARRAY_BUFFER, gVboParticleVelocity);
	glBufferData(GL_ARRAY_BUFFER, (arrayWidth * arrayHeight * 3 * sizeof(float)), velocities, GL_STATIC_DRAW);
	glVertexAttribPointer(VDG_ATTRUBUTE_VELOCITY, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(VDG_ATTRUBUTE_VELOCITY);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	//starttime vbo
	glGenBuffers(1, &gVboParticleStartTime);
	glBindBuffer(GL_ARRAY_BUFFER, gVboParticleStartTime);
	glBufferData(GL_ARRAY_BUFFER, (arrayWidth * arrayHeight * sizeof(float)), statrtTimes, GL_STATIC_DRAW);
	glVertexAttribPointer(VDG_ATTRIBUTE_STARTTIME, 1, GL_FLOAT, GL_FALSE, 0, NULL);

	glEnableVertexAttribArray(VDG_ATTRIBUTE_STARTTIME);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0);

	glShadeModel(GL_SMOOTH);
	//DEPTH
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	glEnable(GL_CULL_FACE);
	glClearColor(0.0f, 0.0f, 0.0, 0.0f);

	gPerspectiveProjectionMatrix = mat4::identity();
	gbAnimate = false;
	gbLight = false;
	resize(WIN_WIDTH, WIN_HEIGHT);
	fprintf(gpFile, "Exitting Initialize..\n");
//	update();
	ToggleFullScreen();
}

void createPoints(GLint w, GLint h)
{
	GLfloat *vptr, *cptr, *velptr, *stptr;
	GLfloat i, j;
	//GLint arrayWidth, arrayHeight;
	if (verts != NULL)
	{
		free(verts);
	}

	/* CH8 */
	verts = (GLfloat *)malloc(w * h * 3 * sizeof(float));
	colors = (GLfloat *)malloc(w * h * 3 * sizeof(float));
	velocities = (GLfloat *)malloc(w * h * 3 * sizeof(float));
	statrtTimes = (GLfloat *)malloc(w * h * sizeof(float));

	vptr = verts;
	cptr = colors;
	velptr = velocities;
	stptr = statrtTimes;

	for (i = 0.5 / w - 0.5; i < 0.5; i = i + 1.0 / w)
	{
		for (j = 0.5 / h - 0.5; j < 0.5; j = j + 1.0 / h)
		{
			*vptr = i;
			*(vptr + 1) = 0.0;
			*(vptr + 2) = j;
			vptr += 3;

			*cptr = ((float)rand() / RAND_MAX)*0.5 + 0.5;
			*(cptr + 1) = ((float)rand() / RAND_MAX)*0.5 + 0.5;
			*(cptr + 2) = ((float)rand() / RAND_MAX)*0.5 + 0.5;
			cptr += 3;

			*velptr = (((float)rand() / RAND_MAX)) + 3.0;
			*(velptr + 1) = ((float)rand() / RAND_MAX) * 10.0;
			*(velptr + 2) = (((float)rand() / RAND_MAX)) + 3.0;
			velptr += 3;

			*stptr = ((float)rand() / RAND_MAX) * 10.0;
			stptr++;
		}
	}
	arrayWidth = w;
	arrayHeight = h;
}


void CalcFPS()
{/*
	currTick = GetTickCount();
	ULONG tickDiff = currTick - lastTick;
	frames++;
	if (tickDiff >= UPDATESPEED)
	{
		lastTick = currTick;
		float calcVal = 1 / ((float)UPDATESPEED / 1000.0);
		float fpsCalc = (float)frames*calcVal;
		fps += fpsCalc;
		fps /= 2;
		frames = 0;

		g_speed = 60 / fps;

	}*/

	//static float framesPerSecond = 0.0f;
	//static float lastTime = 0.0f;
	//float currentTime = GetTickCount();
	//++framesPerSecond;
	//if (currentTime - lastTime > 1.0)
	//{
	//	lastTime = currentTime;
	//	fprintf(gpFile,"CURRENT FRAMES PER SECOND %d\n\n", (int)framesPerSecond);
	//	framesPerSecond = 0;
	//}
}



void display()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(gShaderProgramObject);
	vmath::mat4 modelViewMatrix = mat4::identity();
	mat4 modelViewProjectionMatrix = mat4::identity();

	modelViewMatrix = vmath::translate(0.0f, 0.0f, -12.0f);

	modelViewProjectionMatrix = gPerspectiveProjectionMatrix * modelViewMatrix;

	glUniformMatrix4fv(gMVPUniform, 1, GL_FALSE, modelViewProjectionMatrix);
	glUniform1f(timeUniform, ParticleTime);

	/*if (gbActiveparticleEngine == true)
	{
		gfParticleTime += 0.001f;
		glUniform1f(gTimeUniform, gfParticleTime);
	}
	*/


	glBindVertexArray(gVaoParticle);
	glPointSize(3.0f);
	glDrawArrays(GL_POINTS, 0, arrayWidth*arrayHeight);
	glBindVertexArray(0);
	glUseProgram(0);

	SwapBuffers(ghdc);
}

void resize(int width, int height)
{
	if (height == 0)
		height = 1;
	glViewport(0, 0, (GLsizei)width, (GLsizei)height);
	gPerspectiveProjectionMatrix = perspective(45.0f, (GLfloat)width / (GLfloat)height, 0.1f, 100.0f);
}

void uninitialize(void)
{

	if (gbFullScreen == true)
	{
		dwStyle = GetWindowLong(ghwnd, GWL_STYLE);
		SetWindowLong(ghwnd, GWL_STYLE, dwStyle | WS_OVERLAPPEDWINDOW);
		SetWindowPlacement(ghwnd, &wpPrev);
		SetWindowPos(ghwnd, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOOWNERZORDER | SWP_NOZORDER | SWP_FRAMECHANGED);
		ShowCursor(TRUE);
	}
	if (gVao_Cube)
	{
		glDeleteVertexArrays(1, &gVao_Cube);
		gVao_Cube = 0;
	}

	if (gVbo_CubePosition)
	{
		glDeleteBuffers(1, &gVbo_CubePosition);
		gVbo_CubePosition = 0;
	}

	if (gVbo_Cube_Normal)
	{
		glDeleteBuffers(1, &gVbo_Cube_Normal);
		gVbo_Cube_Normal = 0;
	}

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);
	glDisableVertexAttribArray(VELOCITY_ARRAY);
	glDisableVertexAttribArray(START_TIME_ARRAY);


	//detach vertex shader from shader program object
	glDetachShader(gShaderProgramObject, gVertexShaderObject);

	//detach fragment shader from shader program object
	glDetachShader(gShaderProgramObject, gFragmentShaderObject);

	//delete vertex shader object
	glDeleteShader(gVertexShaderObject);
	gVertexShaderObject = 0;

	//delete fragment shader object
	glDeleteShader(gFragmentShaderObject);
	gFragmentShaderObject = 0;

	//delete shader program object
	glDeleteProgram(gShaderProgramObject);
	gShaderProgramObject = 0;

	//unlink shader program
	glUseProgram(0);


	wglMakeCurrent(NULL, NULL);

	wglDeleteContext(ghrc);
	ghrc = NULL;

	ReleaseDC(ghwnd, ghdc);
	ghdc = NULL;

	DestroyWindow(ghwnd);
	ghwnd = NULL;


}

void update()
{
	NoOfWaves = NoOfWaves - 0.001;
	if (NoOfWaves > 10.0)
		NoOfWaves = 2.0f;

//	time = time + 0.01f;
	//if (time > 5.0)
	//	exit(1);
		//time = 0.0f;
	heightIncr = heightIncr + 0.1;
	if (heightIncr > 500.0)
	//	system("pause");
		//	exit(0);
			//	heightIncr = 0.0f;
		//	exit(0);
		yPos = yPos - 0.010;
	if (yPos < 0.0)
		yPos = 0.0f;

	updateX = updateX - 0.01f;
	updateX2 = updateX2 + 0.1f;
	gAnglePyramid = gAnglePyramid + 0.1;
	if (gAnglePyramid > 0.5)
		gAnglePyramid = 0.5;

	ParticleTime += 0.001f;
	if (ParticleTime >= 1.0f)
		ParticleTime = 0.01f;

	XRes = XRes + 25.0;
	if (XRes >= 1920)
		XRes = 1920;

	YRes = YRes + 35.0;
	if (YRes >= 1080)
		YRes = 1080;
}

void spin()
{
	/*gAngle = gAngle + 0.8f;
	if (gAngle >= 360.0f)
	gAngle = gAngle - 360.0f;*/
	//time = time + 0.1f;
}



void PlayWAV(HINSTANCE hInst)
{
	//PlaySoundA((LPCTSTR)"Hans Zimmer-No Time For Caution3.wav", NULL, SND_ASYNC);
}







































////
////
////
////
////
////
////
////
////
////
////
////
////
////
////
////
////
////
////
////
////
////
////
////
////
////
////
////
////
////
////
////
////
////
////
////
////
////
////
////
////
////
////
////
////







//
//
//
//
//
//#include<Windows.h>
//#include<stdio.h>
//#include<gl/glew.h>
//#include<gl/GL.h>
//
//#include"vmath.h"
//
//using namespace vmath;
//
//#define WIN_WIDTH 800
//#define WIN_HEIGHT 600
//
//#define VERTEX_ARRAY 1
//#define COLOR_ARRAY 2
//#define VELOCITY_ARRAY 3
//#define START_TIME_ARRAY 4
//
//#pragma comment(lib,"opengl32.lib")
//#pragma comment(lib,"glew32.lib")
//#pragma comment(lib,"winmm.lib")
//
////Prototype of WndProc() declared gloabally
//LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
//
////Global variable declarations
//HWND ghwnd = NULL;
//HDC ghdc = NULL;
//HGLRC ghrc = NULL;
//
//
//static GLint arrayWidth, arrayHeight;
//static GLfloat *verts = NULL;
//static GLfloat *colors = NULL;
//static GLfloat *velocities = NULL;
//static GLfloat *statrtTimes = NULL;
//
//
//GLuint uniform_background;
//
//
////5.2831853
//GLfloat gAnglePyramid = 1800.0f;
//
//
//GLfloat heightIncr = 0.0f;
//GLuint heightUniform;
//
//GLuint yPosUniform;
//GLfloat yPos = 2.0;
//GLfloat XRes = 100;
//GLfloat YRes = 50;
//
//ULONG lastTick = GetTickCount(), currTick = GetTickCount();
//ULONG UPDATESPEED;
//UINT frames = 0.0;
//float fps = 60.0;
//float g_speed = 1.0;
//
//GLfloat ParticleTime = 0.0f;
//
//DWORD dwStyle;
//WINDOWPLACEMENT wpPrev = { sizeof(WINDOWPLACEMENT) };
//
//bool gbActiveWindow = false;
//bool gbEscapeKeyIsPressed = false;
//bool gbFullScreen = false;
//
//
//float *pVertexBuffer = NULL;
//float *pColorBuffer = NULL;
//float *pVelocityBuffer = NULL;
//float *pStartTime = NULL;
//
//enum
//{
//	VDG_ATTRIBUTE_VERTEX = 0,
//	VDG_ATTRIBUTE_COLOR,
//	VDG_ATTRIBUTE_STARTTIME,
//	VDG_ATTRUBUTE_VELOCITY
//};
//
//GLuint gVertexShaderObject;
//GLuint gFragmentShaderObject;
//GLuint gShaderProgramObject;
//
//GLuint gVaoParticle;
//GLuint gVboParticlePosition;
//GLuint gVboParticleColor;
//GLuint gVboParticleVelocity;
//GLuint gVboParticleStartTime;
//
//GLuint textureID;
//GLuint gTexture_sampler_uniform;
//
//GLuint mouseUniform;
//
//GLfloat NoOfWaves = 5.2831853;
//GLuint noWavesUniform;
//
//GLuint timeUniform;
//GLfloat time = 0.1f;
//GLuint gMVPUniform;
//GLuint updateXUniform;
//GLuint updateX2Uniform;
//
//GLfloat updateX = 0.0f;
//GLfloat updateX2 = 0.0f;
//GLfloat mouse;
////GLuint timeUniform;
//
//GLuint gLKeyPressedUniform;
//mat4 gPerspectiveProjectionMatrix;
//GLuint gModelViewMatrixUniform, gProjectionMatrixUniform, gViewMatrixUniform;
//GLuint gLdUniform, gKdUniform, gLightPositonUniform;
//GLuint gUniformSkycolor, gUniformcloudColor, gUniformScale;
//
//
//
//GLfloat gAngle = 0.0f;
//
//bool gbAnimate;
//bool gbLight;
//
//FILE *gpFile = NULL;
//void PlayWAV(HINSTANCE);
////main()
//int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdLine, int nCmdShow)
//{
//	void display();
//	void initialize();
//	void uninitialize();
//	void spin();
//	void update();
//	void 	CalcFPS();
//	//	void PlayWAV(HINSTANCE);
//
//	WNDCLASSEX wndclass;
//	HWND hwnd;
//	MSG msg;
//	TCHAR szClassName[] = TEXT("RTROGL");
//	bool bDone = false;
//	//	PlayWAV(hInstance);
//
//	fopen_s(&gpFile, "LogFile.txt", "w");
//	if (gpFile == NULL)
//	{
//		printf("Error opening file\n");
//		exit(0);
//	}
//
//	//initializing members of wndclass
//	wndclass.cbSize = sizeof(WNDCLASSEX);
//	wndclass.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
//	wndclass.cbClsExtra = 0;
//	wndclass.cbWndExtra = 0;
//	wndclass.hInstance = hInstance;
//	wndclass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
//	wndclass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
//	wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
//	wndclass.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
//	wndclass.lpfnWndProc = WndProc;
//	wndclass.lpszClassName = szClassName;
//	wndclass.lpszMenuName = NULL;
//
//
//	//Registering class
//	RegisterClassEx(&wndclass);
//
//	hwnd = CreateWindowEx(WS_EX_APPWINDOW,
//		szClassName,
//		TEXT("OpenGL Programmable PipeLine Native Windowing"),
//		WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_VISIBLE,
//		0,
//		0,
//		WIN_WIDTH,
//		WIN_HEIGHT,
//		NULL,
//		NULL,
//		hInstance,
//		NULL);
//
//	ghwnd = hwnd;
//
//	initialize();
//
//	ShowWindow(hwnd, SW_SHOW);
//	SetForegroundWindow(hwnd);
//	SetFocus(hwnd);
//
//	//Game Loop
//	while (bDone == false)
//	{
//		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
//		{
//			if (msg.message == WM_QUIT)
//				bDone = true;
//			else
//			{
//				TranslateMessage(&msg);
//				DispatchMessage(&msg);
//			}
//		}
//		else
//		{
//			display();
//			update();
//			CalcFPS();
//
//			if (gbAnimate == true)
//				spin();
//
//			if (gbActiveWindow == true)
//			{
//				if (gbEscapeKeyIsPressed == true)
//					bDone = true;
//			}
//		}
//	}
//
//	uninitialize();
//	fprintf(gpFile, "Exitting Main..\n");
//	return((int)msg.wParam);
//}
//
//
//LRESULT CALLBACK WndProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
//{
//	void display();
//	void resize(int, int);
//	void ToggleFullScreen(void);
//	void uninitialize(void);
//
//	static bool gbIsLKeyPressed = false;
//	static bool gbIsAKeyPressed = false;
//	//	static bool 
//
//	fprintf(gpFile, "Entering WndProc..\n");
//
//	switch (iMsg)
//	{
//	case WM_ACTIVATE:
//		if (HIWORD(wParam) == 0)
//			gbActiveWindow = true;
//		else
//			gbActiveWindow = false;
//		break;
//
//	case WM_SIZE:
//		resize(LOWORD(lParam), HIWORD(lParam));
//		break;
//
//	case WM_KEYDOWN:
//		switch (wParam)
//		{
//		case VK_ESCAPE:
//			if (gbEscapeKeyIsPressed == false)
//				gbEscapeKeyIsPressed = true;
//			break;
//
//		case 0x46:
//			if (gbFullScreen == false)
//			{
//				ToggleFullScreen();
//				gbFullScreen = true;
//			}
//			else
//			{
//				ToggleFullScreen();
//				gbFullScreen = false;
//			}
//			break;
//		case 0x41:	//'A' or 'a' pressed (Animate)
//			if (gbIsAKeyPressed == false)
//			{
//				gbAnimate = true;
//				gbIsAKeyPressed = true;
//			}
//			else
//			{
//				gbAnimate = false;
//				gbIsAKeyPressed = false;
//			}
//			break;
//		case 0x4c:		//for l or L key (Light)
//			if (gbIsLKeyPressed == false)
//			{
//				gbLight = true;
//				gbIsLKeyPressed = true;
//			}
//			else
//			{
//				gbLight = false;
//				gbIsLKeyPressed = false;
//			}
//			break;
//		}
//		break;
//
//	case WM_LBUTTONDOWN:
//		break;
//
//	case WM_CLOSE:
//		uninitialize();
//		break;
//
//	case WM_DESTROY:
//		PostQuitMessage(0);
//		break;
//	}
//	return(DefWindowProc(hwnd, iMsg, wParam, lParam));
//	fprintf(gpFile, "Exitting WndProc..\n");
//
//}
//
//void ToggleFullScreen(void)
//{
//	MONITORINFO mi;
//
//	if (gbFullScreen == false)
//	{
//		dwStyle = GetWindowLong(ghwnd, GWL_STYLE);
//		if (dwStyle & WS_OVERLAPPEDWINDOW)
//		{
//			mi = { sizeof(MONITORINFO) };
//			if (GetWindowPlacement(ghwnd, &wpPrev) && GetMonitorInfo(MonitorFromWindow(ghwnd, MONITORINFOF_PRIMARY), &mi))
//			{
//				SetWindowLong(ghwnd, GWL_STYLE, dwStyle & ~WS_OVERLAPPEDWINDOW);
//				SetWindowPos(ghwnd, HWND_TOP, mi.rcMonitor.left,
//					mi.rcMonitor.top,
//					mi.rcMonitor.right - mi.rcMonitor.left,
//					mi.rcMonitor.bottom - mi.rcMonitor.top,
//					SWP_NOZORDER | SWP_FRAMECHANGED);
//			}
//		}
//		ShowCursor(FALSE);
//	}
//	else
//	{
//		SetWindowLong(ghwnd, GWL_STYLE, dwStyle | WS_OVERLAPPEDWINDOW);
//		SetWindowPlacement(ghwnd, &wpPrev);
//		SetWindowPos(ghwnd, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOOWNERZORDER | SWP_NOZORDER | SWP_FRAMECHANGED);
//		ShowCursor(TRUE);
//	}
//}
//
//
//
//void initialize(void)
//{
//
//	void resize(int, int);
//	void uninitialize();
//	void update();
//	fprintf(gpFile, "Entering Initialize..\n");
//	void createPoints();
//	int LoadGLTextures(GLuint *, TCHAR[]);
//
//	//Variable 
//	PIXELFORMATDESCRIPTOR pfd;
//	int iPixelFormatIndex;
//	//GLint num;
//
//	//code
//	ZeroMemory(&pfd, sizeof(PIXELFORMATDESCRIPTOR));
//
//	//initialize code
//	pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
//	pfd.nVersion = 1;
//	pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
//	pfd.iPixelType = PFD_TYPE_RGBA;
//	pfd.cColorBits = 8;
//	pfd.cRedBits = 8;
//	pfd.cGreenBits = 8;
//	pfd.cBlueBits = 8;
//	pfd.cAlphaBits = 8;
//
//	ghdc = GetDC(ghwnd);
//
//	iPixelFormatIndex = ChoosePixelFormat(ghdc, &pfd);
//	if (iPixelFormatIndex == 0)
//	{
//		ReleaseDC(ghwnd, ghdc);
//		ghdc = NULL;
//	}
//	if (SetPixelFormat(ghdc, iPixelFormatIndex, &pfd) == FALSE)
//	{
//		ReleaseDC(ghwnd, ghdc);
//		ghdc = NULL;
//	}
//
//	ghrc = wglCreateContext(ghdc);
//	if (ghrc == NULL)
//	{
//		ReleaseDC(ghwnd, ghdc);
//		ghdc = NULL;
//	}
//
//	if (wglMakeCurrent(ghdc, ghrc) == NULL)
//	{
//		wglDeleteContext(ghrc);
//		ghrc = NULL;
//		ReleaseDC(ghwnd, ghdc);
//		ghdc = NULL;
//	}
//
//	GLenum glew_error = glewInit();
//	if (glew_error != GLEW_OK)
//	{
//		wglDeleteContext(ghrc);
//		ghrc = NULL;
//		ReleaseDC(ghwnd, ghdc);
//		ghdc = NULL;
//	}
//
//	//*****VERTEX SHADER*******
//	//create shader 
//	gVertexShaderObject = glCreateShader(GL_VERTEX_SHADER);
//
//	//provide source code to shader
//	const GLchar * vertexShaderShaderCode =
//		"#version 430 core" \
//		"\n" \
//		"uniform mat4 u_mvp_matrix;" \
//		"uniform float iTime;"\
//		"uniform sampler2D u_texture_sampler;" \
//
//		"in vec4 vPosition;" \
//		"in vec4 vColor;"\
//		"in vec3 velocity;"\
//		"in float startTime;"\
//
//		"out vec4 Color;"\
//
//		"void main(void)"\
//		"{"\
//		"vec4 vert;"\
//		"float t = iTime - startTime;"\
//		"if(t >= 0.0)" \
//		"{"\
//		"vert = vPosition + vec4(velocity * t,0.0);"\
//		"vert.y -= 4.9 * t * t;"\
//		"Color = vColor;"\
//		"}"\
//		"else"\
//		"{"\
//		"vert = vPosition;"\
//		"}"\
//		"gl_Position =u_mvp_matrix * vert;"
//
//		//	"Color = Background;"\
//			
//		"}";
//
//
//
//	glShaderSource(gVertexShaderObject, 1, (const GLchar**)&vertexShaderShaderCode, NULL);
//
//	//compile shader
//	glCompileShader(gVertexShaderObject);
//
//	GLint iInfoLength = 0;
//	GLint iShaderCompiledStatus = 0;
//	char * szInfoLog = NULL;
//
//	glGetShaderiv(gVertexShaderObject, GL_COMPILE_STATUS, &iShaderCompiledStatus);
//	if (iShaderCompiledStatus == GL_FALSE)
//	{
//		glGetShaderiv(gVertexShaderObject, GL_INFO_LOG_LENGTH, &iInfoLength);
//		if (iInfoLength > 0)
//		{
//			szInfoLog = (char *)malloc(iInfoLength);
//			if (szInfoLog != NULL)
//			{
//				GLsizei written;
//				glGetShaderInfoLog(gVertexShaderObject, iInfoLength, &written, szInfoLog);
//				fprintf(gpFile, "Vertex Shader Compilation Log:%s\n", szInfoLog);
//				free(szInfoLog);
//				uninitialize();
//				exit(0);
//			}
//		}
//	}
//
//
//	//*****FRAGMENT SHADER****
//	gFragmentShaderObject = glCreateShader(GL_FRAGMENT_SHADER);
//
//	//											//provide source code to shader
//	const GLchar *fragmentShaderSourceCode =
//		"#version 430 core" \
//		"\n" \
//		"in vec4 Color;"\
//
//		"out vec4 Fragcolor;"\
//		"void main(void)"\
//		"{"\
//		"Fragcolor = Color;"\
//
//		"}";
//
//
//	glShaderSource(gFragmentShaderObject, 1, (const GLchar**)&fragmentShaderSourceCode, NULL);
//
//	//compile shader
//	glCompileShader(gFragmentShaderObject);
//	glGetShaderiv(gFragmentShaderObject, GL_COMPILE_STATUS, &iShaderCompiledStatus);
//	if (iShaderCompiledStatus == GL_FALSE)
//	{
//		glGetShaderiv(gFragmentShaderObject, GL_INFO_LOG_LENGTH, &iShaderCompiledStatus);
//		if (iInfoLength > 0)
//		{
//			szInfoLog = (char *)malloc(iInfoLength);
//			if (szInfoLog != NULL)
//			{
//				GLsizei written;
//				glGetShaderInfoLog(gFragmentShaderObject, iInfoLength, &written, szInfoLog);
//				fprintf(gpFile, "Fragment shader compilation Log %s\n", szInfoLog);
//				free(szInfoLog);
//				uninitialize();
//				exit(0);
//
//			}
//		}
//	}
//
//
//	//*****SHADER PROGRAM******
//	//ate shader program
//	gShaderProgramObject = glCreateProgram();
//
//	//attach vertex shader to shader program
//	glAttachShader(gShaderProgramObject, gVertexShaderObject);
//
//	//attach fragment shader to shader program
//	glAttachShader(gShaderProgramObject, gFragmentShaderObject);
//
//	glBindAttribLocation(gShaderProgramObject, VDG_ATTRIBUTE_VERTEX, "vPosition");
//	glBindAttribLocation(gShaderProgramObject, VDG_ATTRIBUTE_COLOR, "vColor");
//	glBindAttribLocation(gShaderProgramObject, VDG_ATTRUBUTE_VELOCITY, "velocity");
//	glBindAttribLocation(gShaderProgramObject, VDG_ATTRIBUTE_STARTTIME, "startTime");
//
//
//	//	glBindAttribLocation(gShaderProgramObject, VDG_ATTRIBUTE_TEXTURE0, "vTexture0_coord");
//	//link shader
//	glLinkProgram(gShaderProgramObject);
//
//	GLint iShaderProgramLinkStatus = 0;
//	glGetProgramiv(gShaderProgramObject, GL_LINK_STATUS, &iShaderCompiledStatus);
//	if (iShaderCompiledStatus == GL_FALSE)
//	{
//		glGetProgramiv(gShaderProgramObject, GL_INFO_LOG_LENGTH, &iInfoLength);
//		if (iInfoLength > 0)
//		{
//			szInfoLog = (char*)malloc(iInfoLength);
//			if (szInfoLog != NULL)
//			{
//				GLsizei written;
//				glGetProgramInfoLog(gShaderProgramObject, iInfoLength, &written, szInfoLog);
//				fprintf(gpFile, "Shader Program Link Status %s\n", szInfoLog);
//				free(szInfoLog);
//				uninitialize();
//				exit(0);
//			}
//		}
//	}
//
//	//get uniform locations
//	createPoints();
//	gMVPUniform = glGetUniformLocation(gShaderProgramObject, "u_mvp_matrix");
//	timeUniform = glGetUniformLocation(gShaderProgramObject, "iTime");
//	//const GLfloat squareVertices[] =
//	//{
//	//	////front
//	//	1.0f, 1.0f, 1.0f,
//	//	-1.0f, 1.0f, 1.0f,
//	//	-1.0f, -1.0f, 1.0f,
//	//	1.0f, -1.0f, 1.0f,
//	//};
//
//	fprintf(gpFile, "After all vertices and normals..\n");
//
//
//	glGenVertexArrays(1, &gVaoParticle);
//	glBindVertexArray(gVaoParticle);
//
//	// position vbo
//	glGenBuffers(1, &gVboParticlePosition);
//	glBindBuffer(GL_ARRAY_BUFFER, gVboParticlePosition);
//	glBufferData(GL_ARRAY_BUFFER, (sizeof(float) * 3 * 10000), pVertexBuffer, GL_STATIC_DRAW);
//	glVertexAttribPointer(VDG_ATTRIBUTE_VERTEX, 3, GL_FLOAT, GL_FALSE, 0, NULL);
//	glEnableVertexAttribArray(VDG_ATTRIBUTE_VERTEX);
//	glBindBuffer(GL_ARRAY_BUFFER, 0);
//
//	//color vbo
//	glGenBuffers(1, &gVboParticleColor);
//	glBindBuffer(GL_ARRAY_BUFFER, gVboParticleColor);
//	glBufferData(GL_ARRAY_BUFFER, (sizeof(float) * 3 * 10000), pColorBuffer, GL_STATIC_DRAW);
//	glVertexAttribPointer(VDG_ATTRIBUTE_COLOR, 3, GL_FLOAT, GL_FALSE, 0, NULL);
//	glEnableVertexAttribArray(VDG_ATTRIBUTE_COLOR);
//	glBindBuffer(GL_ARRAY_BUFFER, 0);
//
//	//vbo velocity
//	glGenBuffers(1, &gVboParticleVelocity);
//	glBindBuffer(GL_ARRAY_BUFFER, gVboParticleVelocity);
//	glBufferData(GL_ARRAY_BUFFER, (sizeof(float) * 3 * 10000), pVelocityBuffer, GL_STATIC_DRAW);
//	glVertexAttribPointer(VDG_ATTRUBUTE_VELOCITY, 3, GL_FLOAT, GL_FALSE, 0, NULL);
//	glEnableVertexAttribArray(VDG_ATTRUBUTE_VELOCITY);
//	glBindBuffer(GL_ARRAY_BUFFER, 0);
//
//	//starttime vbo
//	glGenBuffers(1, &gVboParticleStartTime);
//	glBindBuffer(GL_ARRAY_BUFFER, gVboParticleStartTime);
//	glBufferData(GL_ARRAY_BUFFER, (sizeof(float) * 3 * 10000), pStartTime, GL_STATIC_DRAW);
//	glEnableVertexAttribArray(VDG_ATTRIBUTE_STARTTIME);
//	glBindBuffer(GL_ARRAY_BUFFER, 0);
//
//	glBindVertexArray(0);
//
//	glShadeModel(GL_SMOOTH);
//	//DEPTH
//	glClearDepth(1.0f);
//	glEnable(GL_DEPTH_TEST);
//	glDepthFunc(GL_LEQUAL);
//	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
//	glEnable(GL_CULL_FACE);
//	glClearColor(0.0f, 0.0f, 0.0, 0.0f);
//
//	gPerspectiveProjectionMatrix = mat4::identity();
//	resize(WIN_WIDTH, WIN_HEIGHT);
//	fprintf(gpFile, "Exitting Initialize..\n");
//	//	update();
//	ToggleFullScreen();
//}
//
//void createPoints()
//{
//
//	GLfloat *vptr, *cptr, *velptr, *stptr;
//
//	GLfloat radius = 0.5f;
//	GLfloat xCoord = 0.0f;
//	GLfloat yCoord = 0.0f;
//	GLfloat startTimerCounter = 1.0;
//
//
//	GLfloat *verts, *colors, *velocities, *statrtTimes;
//
///*
//	if (verts != NULL)
//		free(verts);*/
//
//	//verts = (GLfloat*)malloc( 3 *10000* sizeof(float));
//	//colors = (GLfloat*)malloc(3 * 10000 * sizeof(float));
//	//velocities = (GLfloat *)malloc( sizeof(float) * 10000 * 3);
//	//statrtTimes = (GLfloat*)malloc(sizeof(float) * 10000);
//
//
//	//vptr = verts;	
//	//cptr = colors;
//	//velptr = velocities;
//	//stptr = statrtTimes;
//	//*(statrtTimes) = startTimerCounter;
//	////////	
//
//
//
//	////////for (int i = 0; i < 10000;i++)
//	////////	{
//	////////		/////////////////////////////////////////////////////// For straight moving particles ///////////////////////////////////////////
//
//	////////		//*vptr = i;
//	////////		//*(vptr + 1) = 0.0f;
//	////////		//*(vptr + 2) = j;
//	////////		//vptr += 3;
//	////////		//*cptr = ((float)rand() / RAND_MAX)*0.5 + 0.5;
//	////////		//*(cptr + 1) = ((float)rand() / RAND_MAX) * 0.5 + 0.5;
//	////////		//*(cptr + 2) = ((float)rand() / RAND_MAX) * 0.5 + 0.5;
//
//	////////		//cptr += 3;
//
//	////////		//*velptr = (((float)rand() / 1000)) + 3.0;
//	////////		//*(velptr + 1) = ((float)rand() / 1000) * 10.0f;
//	////////		//*(velptr + 2) = (((float)rand() / 1000)) + 3.0f;
//	////////		//velptr += 3;
//
//	////////		//*stptr = ((float)rand() / RAND_MAX) * 10.0F;
//	////////		//stptr++;
//	////////		///////////////////////////////////////////////////////////////////////
//	////////	
//	////////	//////	if (gAngle >= 360.0f)
//	////////	//////	{
//	////////	//////		gAngle = 0.0f;
//	////////	//////	}
//
//	////////	//////	radius -= 0.0010f;
//	////////	//////	xCoord = (radius * cos(gAngle));
//	////////	//////	yCoord = (radius* sin(gAngle));
//	////////	///////*	*vptr = i;
//	////////	//////	*(vptr + 1) = 0.0f;
//	////////	//////	*(vptr + 2) = j;
//	////////	//////	vptr += 3;*/
//
//	////////	//////	*(vptr + 0) = xCoord;
//	////////	//////	*(vptr + 1) = yPos;
//	////////	//////	*(vptr + 2) = -1.0f;
//
//	////////	//////	vptr += 3;
//
//	////////	//////	*cptr = ((float)rand() / 256) * i * 0.5;
//	////////	//////	*(cptr + 1) = ((float)rand() / 128) * 0.5 ;
//	////////	//////	*(cptr + 2) = ((float)rand() / 56) * 0.5;
//
//	////////	//////	cptr += 3;
//
//	////////	//////	*velptr = ((float)(rand() % 1000)) * 0.0005;
//	////////	//////	*(velptr + 1) = ((float)(rand() % 1000)) * 0.0002f;
//	////////	//////	*(velptr + 2) = ((float)(rand() % 1000)) * 0.0008f;
//	////////	//////	velptr += 3;
//
//
//	////////	//////	*statrtTimes = startTimerCounter;
//
//
//
//
//
//
//	////////		gAngle = gAngle + 0.01f;
//	////////		if (gAngle >= 360.0f)
//	////////		{
//	////////			gAngle = 0.0f;
//	////////		}
//
//	////////		radius -= 0.0010f;
//	////////		xCoord = (GLfloat)(radius * cos(gAngle));
//	////////		yCoord = (GLfloat)(radius * sin(gAngle));
//
//	////////		*(vptr + 0) = xCoord;
//	////////		*(vptr + 1) = yCoord;
//	////////		*(vptr + 2) = -1.0f;
//
//	////////		*(cptr + 0) = ((float)(rand() / 256)) *i * 0.5f;
//	////////		*(cptr + 1) = ((float)(rand() / 128)) *0.5f;
//	////////		*(cptr + 2) = ((float)(rand() / 56)) *0.5f;
//
//	////////		*(velptr + 0) = ((float)(rand() % 1000)) * 0.0005f;
//	////////		*(velptr + 1) = ((float)(rand() % 1000)) * 0.0002f;
//	////////		*(velptr + 2) = ((float)(rand() % 1000)) * 0.0008f;
//
//	////////	//	*(stptr) = ((float)(rand() / 1000)) * i;
//	////////		*(stptr) = startTimerCounter;
//
//	////////	//	startTimerCounter += 0.005f;
//
//
//	////////		
//	////////		vptr += 3;
//	////////		cptr += 3;
//	////////		velptr += 3;
//	////////		stptr += 3;
//	////////		}
//	////////		
//	////////	}
//	////////
//
//
//
//	GLfloat gfangle;
//	GLfloat j = 0.0f;
//	GLfloat gfXCord = 0.0f;
//	GLfloat gfYCord = 0.0f;
//
//	GLfloat VaryingRadiousOfSpiral = 0.5f;
//
//	GLfloat *vertexBuffer, *colorBuffer, *velocityBuffer, *startTimeBuffer;
//
//	if (pVertexBuffer == NULL)
//	{
//		pVertexBuffer = (float *)malloc(3 * 10000 * sizeof(float));
//		if (pVertexBuffer == NULL)
//		{
//			fprintf(gpFile, "\nERROR WHILE ALLOCATINFG MEMORY TO THE VERTEX BUFFER \n");
//			//uninitialize();
//			exit(1);
//		}
//	}
//	if (pColorBuffer == NULL)
//	{
//		pColorBuffer = (float *)malloc(3 * 10000 * sizeof(float));
//		if (pColorBuffer == NULL)
//		{
//			fprintf(gpFile, "\nERROR WHILE ALLOCATINFG MEMORY TO THE COLOR BUFFER \n");
//			//	uninitialize();
//			exit(1);
//		}
//	}
//
//	if (pVelocityBuffer == NULL)
//	{
//		pVelocityBuffer = (float *)malloc(sizeof(float) * 10000 * 3);
//		if (pVelocityBuffer == NULL)
//		{
//			fprintf(gpFile, "\nERROR WHILE ALLOCATINFG MEMORY TO THE VELOCITY BUFFER \n");
//			//	uninitialize();
//			exit(1);
//		}
//	}
//	if (pStartTime == NULL)
//	{
//		pStartTime = (float *)malloc(sizeof(float) * 10000);
//		if (pStartTime == NULL)
//		{
//			fprintf(gpFile, "\nERROR WHILE ALLOCATINFG MEMORY TO THE START_TIME BUFFER \n");
//			//	uninitialize();
//			exit(1);
//		}
//	}
//	//vertexBuffer = pVertexBuffer;
//	//colorBuffer = pColorBuffer;
//	//velocityBuffer = pVelocityBuffer;
//	//startTimeBuffer = pStartTime;
//
//	//gfangle = 0.0f;
//	//float gfStartTimeCounter = 1.0f;
//	//for (int i = 0; i < 10000; i++)
//	//{
//	//	//gfangle = 2 * PI * i / NumberOfPoint;
//	//	gfangle = gfangle + 0.01f;
//	//	if (gfangle >= 360.0f)
//	//	{
//	//		gfangle = 0.0f;
//	//	}
//
//	//	VaryingRadiousOfSpiral -= 0.0020f;
//	//	gfXCord = (GLfloat)(VaryingRadiousOfSpiral * cos(gfangle)/4);
//	//	gfYCord = (GLfloat)(VaryingRadiousOfSpiral * sin(gfangle)/4);
//
//	//	*(vertexBuffer + 0) = gfXCord;
//	//	*(vertexBuffer + 1) = gfYCord;
//	//	*(vertexBuffer + 2) = -1.0f;
//
//	//	*(colorBuffer + 0) = ((float)(rand() / 256)) *i * 0.5f;
//	//	*(colorBuffer + 1) = ((float)(rand() / 128)) *0.5f;
//	//	*(colorBuffer + 2) = ((float)(rand() / 356)) *3.5f;
//
//	//	*(velocityBuffer + 0) = ((float)(rand() % 1000)) * 0.0005f;
//	//	*(velocityBuffer + 1) = ((float)(rand() % 1000)) * 0.0002f;
//	//	*(velocityBuffer + 2) = ((float)(rand() % 4000)) * 0.0508f;
//
//	//	//*(StartTimeBuffer) = ((float)(rand() / 1000)) * i;
//	//	*(startTimeBuffer) = gfStartTimeCounter;
//
//	//	//gfStartTimeCounter += 0.005f;
//
//	//	vertexBuffer += 3;
//	//	colorBuffer += 3;
//	//	velocityBuffer += 3;
//	//	startTimeBuffer += 1;
//
//	//}
//
//
//	//arrayWidth = w;
//	//arrayHeight = h;
//
//
//	verts = (GLfloat*)malloc( 3 *10000* sizeof(float));
//	colors = (GLfloat*)malloc(3 * 10000 * sizeof(float));
//	velocities = (GLfloat *)malloc( sizeof(float) * 10000 * 3);
//	statrtTimes = (GLfloat*)malloc(sizeof(float) * 10000);
//
//
//	vptr = verts;	
//	cptr = colors;
//	velptr = velocities;
//	stptr = statrtTimes;
//	*(statrtTimes) = startTimerCounter;
//		
//
//
//
//	for (int i = 0; i < 10000;i++)
//		{
//			/////////////////////////////////////////////////// For straight moving particles ///////////////////////////////////////////
//
//			*vptr = i;
//			*(vptr + 1) = 0.0f;
//			*(vptr + 2) = 0.0f;
//			vptr += 3;
//			*cptr = ((float)rand() / RAND_MAX)*0.5 + 0.5;
//			*(cptr + 1) = ((float)rand() / RAND_MAX) * 0.5 + 0.5;
//			*(cptr + 2) = ((float)rand() / RAND_MAX) * 0.5 + 0.5;
//
//			cptr += 3;
//
//			*velptr = (((float)rand() / 1000)) + 3.0;
//			*(velptr + 1) = ((float)rand() / 1000) * 10.0f;
//			*(velptr + 2) = (((float)rand() / 1000)) + 3.0f;
//			velptr += 3;
//
//			*stptr = ((float)rand() / RAND_MAX) * 10.0f;
//			stptr++;
//			///////////////////////////////////////////////////////////////////
//		
//		//	if (gAngle >= 360.0f)
//		//	{
//		//		gAngle = 0.0f;
//		//	}
//
//		//	radius -= 0.0010f;
//		//	xCoord = (radius * cos(gAngle));
//		//	yCoord = (radius* sin(gAngle));
//		///*	*vptr = i;
//		//	*(vptr + 1) = 0.0f;
//		//	*(vptr + 2) = j;
//		//	vptr += 3;*/
//
//		//	*(vptr + 0) = xCoord;
//		//	*(vptr + 1) = yPos;
//		//	*(vptr + 2) = -1.0f;
//
//		//	vptr += 3;
//
//		//	*cptr = ((float)rand() / 256) * i * 0.5;
//		//	*(cptr + 1) = ((float)rand() / 128) * 0.5 ;
//		//	*(cptr + 2) = ((float)rand() / 56) * 0.5;
//
//		//	cptr += 3;
//
//		//	*velptr = ((float)(rand() % 1000)) * 0.0005;
//		//	*(velptr + 1) = ((float)(rand() % 1000)) * 0.0002f;
//		//	*(velptr + 2) = ((float)(rand() % 1000)) * 0.0008f;
//		//	velptr += 3;
//
//
//		//	*statrtTimes = startTimerCounter;
//
//
//
//
//
//
//			gAngle = gAngle + 0.01f;
//			if (gAngle >= 360.0f)
//			{
//				gAngle = 0.0f;
//			}
//
//			radius -= 0.0010f;
//			xCoord = (GLfloat)(radius * cos(gAngle));
//			yCoord = (GLfloat)(radius * sin(gAngle));
//
//			*(vptr + 0) = xCoord;
//			*(vptr + 1) = yCoord;
//			*(vptr + 2) = -1.0f;
//
//			*(cptr + 0) = ((float)(rand() / 256)) *i * 0.5f;
//			*(cptr + 1) = ((float)(rand() / 128)) *0.5f;
//			*(cptr + 2) = ((float)(rand() / 56)) *0.5f;
//
//			*(velptr + 0) = ((float)(rand() % 1000)) * 0.0005f;
//			*(velptr + 1) = ((float)(rand() % 1000)) * 0.0002f;
//			*(velptr + 2) = ((float)(rand() % 1000)) * 0.0008f;
//
//			*(stptr) = ((float)(rand() / 1000)) * i;
//			*(stptr) = startTimerCounter;
//
//			startTimerCounter += 0.005f;
//
//
//			
//			vptr += 3;
//			cptr += 3;
//			velptr += 3;
//			stptr += 3;
//			}
//			
//		
//	
//
//
//}
//
//
//void CalcFPS()
//{/*
// currTick = GetTickCount();
// ULONG tickDiff = currTick - lastTick;
// frames++;
// if (tickDiff >= UPDATESPEED)
// {
// lastTick = currTick;
// float calcVal = 1 / ((float)UPDATESPEED / 1000.0);
// float fpsCalc = (float)frames*calcVal;
// fps += fpsCalc;
// fps /= 2;
// frames = 0;
//
// g_speed = 60 / fps;
//
// }*/
//
// //static float framesPerSecond = 0.0f;
// //static float lastTime = 0.0f;
// //float currentTime = GetTickCount();
// //++framesPerSecond;
// //if (currentTime - lastTime > 1.0)
// //{
// //	lastTime = currentTime;
// //	fprintf(gpFile,"CURRENT FRAMES PER SECOND %d\n\n", (int)framesPerSecond);
// //	framesPerSecond = 0;
// //}
//}
//
//
//
//void display()
//{
//	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
//
//	//start using OpenGL program object
//	glUseProgram(gShaderProgramObject);
//	mat4 modelViewmatrix = mat4::identity();
//	mat4 modelviewProjectionMatrix = mat4::identity();
//
//	modelViewmatrix = translate(0.0f, 0.0f, -25.0f);
//
//
//	modelviewProjectionMatrix = gPerspectiveProjectionMatrix * modelViewmatrix;
//
//	glUniformMatrix4fv(gMVPUniform, 1, GL_FALSE, modelviewProjectionMatrix);
//
//	//	glUniform3f(iResolutionUniform, 1920, 1080,0.0f);
//	//	glUniform4f(uniform_background, 1.0f, 1.0, 0.0, 0.0f);
//	//	GLfloat time = 1000.0f;
//	glUniform1f(timeUniform, ParticleTime);
//	//OpenGL Drawing
//	glPointSize(3.0);
//	glBindVertexArray(gVaoParticle);
//
//
//	glDrawArrays(GL_POINTS, 0, 10000);
//
//	glBindVertexArray(0);
//	glUseProgram(0);
//	SwapBuffers(ghdc);
//}
//
//void resize(int width, int height)
//{
//	if (height == 0)
//		height = 1;
//	glViewport(0, 0, (GLsizei)width, (GLsizei)height);
//	gPerspectiveProjectionMatrix = perspective(45.0f, (GLfloat)width / (GLfloat)height, 0.1f, 100.0f);
//}
//
//void uninitialize(void)
//{
//
//	if (gbFullScreen == true)
//	{
//		dwStyle = GetWindowLong(ghwnd, GWL_STYLE);
//		SetWindowLong(ghwnd, GWL_STYLE, dwStyle | WS_OVERLAPPEDWINDOW);
//		SetWindowPlacement(ghwnd, &wpPrev);
//		SetWindowPos(ghwnd, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOOWNERZORDER | SWP_NOZORDER | SWP_FRAMECHANGED);
//		ShowCursor(TRUE);
//	}
//	if (gVaoParticle)
//	{
//		glDeleteVertexArrays(1, &gVaoParticle);
//		gVaoParticle = 0;
//	}
//
//
//	if (gVboParticleColor)
//	{
//		glDeleteBuffers(1, &gVboParticleColor);
//		gVboParticleColor = 0;
//	}
//
//	if (gVboParticlePosition)
//	{
//		glDeleteBuffers(1, &gVboParticlePosition);
//		gVboParticlePosition = 0;
//	}
//
//	if (gVboParticleStartTime)
//	{
//		glDeleteBuffers(1, &gVboParticleStartTime);
//		gVboParticleStartTime = 0;
//	}
//
//	if (gVboParticleVelocity)
//	{
//		glDeleteBuffers(1, &gVboParticleVelocity);
//		gVboParticleVelocity = 0;
//	}
//
//
//
//
//	//detach vertex shader from shader program object
//	glDetachShader(gShaderProgramObject, gVertexShaderObject);
//
//	//detach fragment shader from shader program object
//	glDetachShader(gShaderProgramObject, gFragmentShaderObject);
//
//	//delete vertex shader object
//	glDeleteShader(gVertexShaderObject);
//	gVertexShaderObject = 0;
//
//	//delete fragment shader object
//	glDeleteShader(gFragmentShaderObject);
//	gFragmentShaderObject = 0;
//
//	//delete shader program object
//	glDeleteProgram(gShaderProgramObject);
//	gShaderProgramObject = 0;
//
//	//unlink shader program
//	glUseProgram(0);
//
//
//	wglMakeCurrent(NULL, NULL);
//
//	wglDeleteContext(ghrc);
//	ghrc = NULL;
//
//	ReleaseDC(ghwnd, ghdc);
//	ghdc = NULL;
//
//	DestroyWindow(ghwnd);
//	ghwnd = NULL;
//
//
//}
//
//void update()
//{
//	NoOfWaves = NoOfWaves - 0.001;
//	if (NoOfWaves > 10.0)
//		NoOfWaves = 2.0f;
//
//	//	time = time + 0.01f;
//	//if (time > 5.0)
//	//	exit(1);
//	//time = 0.0f;
//	heightIncr = heightIncr + 0.1;
//	if (heightIncr > 500.0)
//		//	system("pause");
//		//	exit(0);
//		//	heightIncr = 0.0f;
//		//	exit(0);
//		yPos = yPos - 0.010;
//	if (yPos < 0.0)
//		yPos = 0.0f;
//
//	updateX = updateX - 0.01f;
//	updateX2 = updateX2 + 0.1f;
//	gAnglePyramid = gAnglePyramid + 0.1;
//	if (gAnglePyramid > 0.5)
//		gAnglePyramid = 0.5;
//
//
//	XRes = XRes + 25.0;
//	if (XRes >= 1920)
//		XRes = 1920;
//
//	YRes = YRes + 35.0;
//	if (YRes >= 1080)
//		YRes = 1080;
//
//	ParticleTime = ParticleTime + 0.001f;
//	/*if (ParticleTime >= 1.0)
//		ParticleTime = 0.01f;*/
//}
//
//void spin()
//{
//	/*gAngle = gAngle + 0.8f;
//	if (gAngle >= 360.0f)
//	gAngle = gAngle - 360.0f;*/
//	//time = time + 0.1f;
//}
////
////
////
////void PlayWAV(HINSTANCE hInst)
////{
////	PlaySoundA((LPCTSTR)"Hans Zimmer-No Time For Caution3.wav", NULL, SND_ASYNC);
////}
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//




































































//
//#include<Windows.h>
//#include<gl/GL.h>
//#include<gl/GLU.h>
//#include <stdio.h> 
//
//
//#pragma comment(lib,"gdi32.lib")
//#pragma comment(lib,"user32.lib")
//#pragma comment(lib,"kernel32.lib")
//
//#pragma comment(lib,"opengl32.lib")
//#pragma comment(lib,"glu32.lib")
//
//LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
//
//
//bool gbEscapeKeyIsPressed = false;
//bool gbFullscreen = false;
//HWND ghwnd = NULL;
//HDC ghdc = NULL;
//HGLRC ghrc = NULL;
////fulscreen
//DWORD dwStyle;
//
//FILE *gpFile = NULL;
//
//GLfloat gRandom, gRandom1, gRandom2;
//
//WINDOWPLACEMENT wpPrev = { sizeof(WINDOWPLACEMENT) };
//
//
//GLfloat x[250], y[250], z[250];
//
//GLfloat Xl[70], Yl[70], Zl[70];
//
//
//
//
//
//int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdLine, int iCmdShow)
//{
//
//	WNDCLASSEX wndclass;
//	TCHAR szAppName[] = TEXT("Rasterization");
//	HWND hwnd;
//	bool bDone = false;
//	MSG msg;
//
//	void display(void);
//	void initialize(void);
//	void uninitialize(void);
//
//	if (fopen_s(&gpFile, "WinMainLog.txt", "w") != 0)
//	{
//		MessageBox(NULL, TEXT("Log File Can Not Be Created\nExitting ..."), TEXT("Error"), MB_OK | MB_TOPMOST | MB_ICONSTOP);
//		exit(0);
//	}
//	else
//	{
//		fprintf(gpFile, "Log File Is Successfully Opened.\n");
//	}
//
//	wndclass.cbSize = sizeof(WNDCLASSEX);
//	wndclass.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
//	wndclass.cbClsExtra = 0;
//	wndclass.cbWndExtra = 0;
//	wndclass.lpfnWndProc = WndProc;
//	wndclass.hInstance = hInstance;
//	wndclass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
//	wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
//	wndclass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
//	wndclass.lpszClassName = szAppName;
//	wndclass.lpszMenuName = NULL;
//	wndclass.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
//
//	RegisterClassEx(&wndclass);
//
//	hwnd = CreateWindowEx(WS_EX_APPWINDOW,
//		szAppName,
//		TEXT("Demo representing Rasterization Stage"),
//		WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_VISIBLE,
//		300,
//		300,
//		800,
//		600,
//		NULL,
//		NULL,
//		hInstance,
//		NULL);
//
//	ghwnd = hwnd;
//
//	initialize();
//
//	ShowWindow(hwnd, SW_SHOW);
//	SetForegroundWindow(hwnd);
//	SetFocus(hwnd);
//
//	while (bDone == false)
//	{
//		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
//		{
//			if (msg.message == WM_QUIT)
//				bDone = true;
//			else
//			{
//				TranslateMessage(&msg);
//				DispatchMessage(&msg);
//			}
//		}
//		else
//		{
//
//			if (gbEscapeKeyIsPressed == true)
//				bDone = true;
//
//			display();
//
//		}
//	}
//
//	uninitialize();
//	return((int)msg.wParam);
//}
//
//LRESULT CALLBACK WndProc(HWND hwnd, UINT iMsg, WPARAM wparam, LPARAM lparam)
//{
//	void ToggleFullscreen(void);
//	void uninitialize(void);
//	void resize(int height, int width);
//	switch (iMsg)
//	{
//	case WM_ACTIVATE:
//		break;
//
//		//case WM_ERASEBKGND:
//		//	return(0);
//
//	case WM_KEYDOWN:
//		switch (wparam)
//		{
//		case VK_ESCAPE:
//
//			if (gbEscapeKeyIsPressed == false)
//				gbEscapeKeyIsPressed = true;
//			break;
//
//		case 0x46: //'F' or 'f' Key
//			if (gbFullscreen == false)
//			{
//				ToggleFullscreen();
//				gbFullscreen = true;
//			}
//			else
//			{
//				ToggleFullscreen();
//				gbFullscreen = false;
//			}
//			break;
//		default:
//			break;
//		}
//		break;
//	case WM_LBUTTONDOWN:
//		break;
//	case WM_DESTROY:
//
//		PostQuitMessage(0);
//		break;
//
//	case WM_SIZE:
//		resize(LOWORD(lparam), HIWORD(lparam));
//		break;
//	case WM_CLOSE:
//		uninitialize();
//		break;
//
//	default:
//		break;
//	}
//	return(DefWindowProc(hwnd, iMsg, wparam, lparam));
//}
//
//void ToggleFullscreen(void)
//{
//	//variable declarations
//	MONITORINFO mi;
//
//	//code
//	if (gbFullscreen == false)
//	{
//		dwStyle = GetWindowLong(ghwnd, GWL_STYLE);
//		if (dwStyle & WS_OVERLAPPEDWINDOW)
//		{
//			mi = { sizeof(MONITORINFO) };
//			if (GetWindowPlacement(ghwnd, &wpPrev) && GetMonitorInfo(MonitorFromWindow(ghwnd, MONITORINFOF_PRIMARY), &mi))
//			{
//				SetWindowLong(ghwnd, GWL_STYLE, dwStyle & ~WS_OVERLAPPEDWINDOW);
//				SetWindowPos(ghwnd, HWND_TOP, mi.rcMonitor.left, mi.rcMonitor.top, mi.rcMonitor.right - mi.rcMonitor.left, mi.rcMonitor.bottom - mi.rcMonitor.top, SWP_NOZORDER | SWP_FRAMECHANGED);
//			}
//		}
//		ShowCursor(FALSE);
//	}
//
//	else
//	{
//		//code
//		SetWindowLong(ghwnd, GWL_STYLE, dwStyle | WS_OVERLAPPEDWINDOW);
//		SetWindowPlacement(ghwnd, &wpPrev);
//		SetWindowPos(ghwnd, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOOWNERZORDER | SWP_NOZORDER | SWP_FRAMECHANGED);
//
//		ShowCursor(TRUE);
//	}
//}
//void initialize(void)
//{
//	//	fprintf(gpFile, "in initialize \n");
//	//function prototypes
//	void resize(int, int);
//
//	RECT rt;
//	//variable declarations
//	PIXELFORMATDESCRIPTOR pfd;
//	int iPixelFormatIndex;
//
//	//code
//	ZeroMemory(&pfd, sizeof(PIXELFORMATDESCRIPTOR));
//
//	//Initialization of structure 'PIXELFORMATDESCRIPTOR'
//	pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
//	pfd.nVersion = 1;
//	pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
//	pfd.iPixelType = PFD_TYPE_RGBA;
//	pfd.cColorBits = 32;
//	pfd.cRedBits = 8;
//	pfd.cGreenBits = 8;
//	pfd.cBlueBits = 8;
//	pfd.cAlphaBits = 8;
//	pfd.cDepthBits = 24;
//
//	ghdc = GetDC(ghwnd);
//
//	iPixelFormatIndex = ChoosePixelFormat(ghdc, &pfd);
//	if (iPixelFormatIndex == 0)
//	{
//		ReleaseDC(ghwnd, ghdc);
//		ghdc = NULL;
//	}
//
//	if (SetPixelFormat(ghdc, iPixelFormatIndex, &pfd) == false)
//	{
//		ReleaseDC(ghwnd, ghdc);
//		ghdc = NULL;
//	}
//
//	ghrc = wglCreateContext(ghdc);
//	if (ghrc == NULL)
//	{
//		ReleaseDC(ghwnd, ghdc);
//		ghdc = NULL;
//	}
//
//	if (wglMakeCurrent(ghdc, ghrc) == false)
//	{
//		wglDeleteContext(ghrc);
//		ghrc = NULL;
//		ReleaseDC(ghwnd, ghdc);
//		ghdc = NULL;
//	}
//
//	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
//
//	glShadeModel(GL_SMOOTH);
//	glEnable(GL_DEPTH_TEST);
//	glClearDepth(1.0f);
//	glDepthFunc(GL_LEQUAL);
//	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
//
//	GetClientRect(ghwnd, &rt);
//	resize(rt.right - rt.left, rt.bottom - rt.top);
//	fprintf(gpFile, "out initialize \n");
//
//
//	srand(4);
//	for (int i = 0; i < 250; i += 1)
//	{
//		x[i] = ((float)rand() / (float)RAND_MAX)*800.0f - 400.0f;
//		y[i] = ((float)rand() / (float)RAND_MAX)*600.0f - 300.0f;
//		z[i] = ((float)rand() / (float)RAND_MAX)*(-1000.0f);
//	}
//
//	srand(7);
//	for (int i = 0; i < 70; i += 1)
//	{
//		Xl[i] = ((float)rand() / (float)RAND_MAX)*800.0f - 400.0f;
//		Yl[i] = ((float)rand() / (float)RAND_MAX)*600.0f - 300.0f;
//		Zl[i] = ((float)rand() / (float)RAND_MAX)*(-1000.0f);
//	}
//}
//
//void resize(int width, int height)
//{
//	if (height == 0)
//		height = 1;
//	glViewport(0, 0, (GLsizei)width, (GLsizei)height);
//
//	glMatrixMode(GL_PROJECTION);
//	glLoadIdentity();
//
//	gluPerspective(45.0f, (GLfloat)width / (GLfloat)height, 0.1f, 1000.0f);
//
//}
//
//void uninitialize(void)
//{
//	//UNINITIALIZATION CODE
//
//	if (gbFullscreen == true)
//	{
//		dwStyle = GetWindowLong(ghwnd, GWL_STYLE);
//		SetWindowLong(ghwnd, GWL_STYLE, dwStyle | WS_OVERLAPPEDWINDOW);
//		SetWindowPlacement(ghwnd, &wpPrev);
//		SetWindowPos(ghwnd, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOOWNERZORDER | SWP_NOZORDER | SWP_FRAMECHANGED);
//
//		ShowCursor(TRUE);
//
//	}
//
//	wglMakeCurrent(NULL, NULL);
//
//	wglDeleteContext(ghrc);
//	ghrc = NULL;
//
//	ReleaseDC(ghwnd, ghdc);
//	ghdc = NULL;
//
//	DestroyWindow(ghwnd);
//	if (gpFile)
//	{
//		fclose(gpFile);
//		gpFile = NULL;
//	}
//}
//void display(void)
//{
//	//	fprintf(gpFile, "in display \n");
//	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//	glMatrixMode(GL_MODELVIEW);
//	glLoadIdentity();
//
//	glColor3f(1.0f, 1.0f, 1.0f);
//
//	//small stars
//	glPointSize(1.0f);
//	glBegin(GL_POINTS);
//
//	for (int i = 0; i < 250; i += 1)
//	{
//		if (z[i] > 0.0f)
//		{
//			z[i] = -1000.0f;
//		}
//		z[i] += 10.0f;
//		glVertex3f(x[i], y[i], z[i]);
//
//	}
//	glEnd();
//
//	//big stars
//	glPointSize(2.5f);
//	glBegin(GL_POINTS);
//
//	for (int i = 0; i < 70; i += 1)
//	{
//		if (Zl[i] > 0.0f)
//		{
//			Zl[i] = -1000.0f;
//		}
//		Zl[i] += 10.0f;
//		glVertex3f(Xl[i], Yl[i], Zl[i]);
//
//	}
//	glEnd();
//
//	SwapBuffers(ghdc);
//
//	//	fprintf(gpFile, "out display \n");
//}
//

