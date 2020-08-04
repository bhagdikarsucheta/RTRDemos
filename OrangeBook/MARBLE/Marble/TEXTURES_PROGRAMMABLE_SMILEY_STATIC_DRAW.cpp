#include<Windows.h>
#include<stdio.h>
#include<gl/glew.h>
#include<gl/GL.h>

#include"vmath.h"

using namespace vmath;

#define WIN_WIDTH 800
#define WIN_HEIGHT 600

#pragma comment(lib,"opengl32.lib")
#pragma comment(lib,"glew32.lib")
#pragma comment(lib,"winmm.lib")

//Prototype of WndProc() declared gloabally
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

//Global variable declarations
HWND ghwnd = NULL;
HDC ghdc = NULL;
HGLRC ghrc = NULL;

//5.2831853
GLfloat gAnglePyramid = 1800.0f;


GLfloat heightIncr = 0.0f;
GLuint heightUniform;

GLuint yPosUniform;
GLfloat yPos = 2.0;

ULONG lastTick = GetTickCount(), currTick = GetTickCount();
ULONG UPDATESPEED;
float frames = 0.0;
float fps = 0.0;
float lastTime = 0.0f;

float g_speed=1.0;

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
};

GLuint gVertexShaderObject;
GLuint gFragmentShaderObject;
GLuint gShaderProgramObject;

GLuint gVao_Cube;
GLuint gVbo_CubePosition;
GLuint gVbo_Cube_Normal;

GLuint gTexture_sampler_uniform;
GLuint gTexture_sampler_uniform1;
GLuint gTexture_sampler_uniform2;

GLuint mouseUniform;

GLfloat NoOfWaves = 5.2831853;
GLuint noWavesUniform;

GLuint timeUniform;
GLfloat time = 2.0f;
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

GLfloat skycolor[] = { 0.8f,0.8f,0.8f };
GLfloat cloudcolor[] = { 0.0f,0.0f,0.8f };
GLfloat scaleVal = 0.5f;

GLuint iResolutionUniform;

GLfloat gAngle = 0.0f;

bool gbAnimate;
bool gbLight;

FILE *gpFile = NULL;

//main()
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdLine, int nCmdShow)
{
	void display();
	void initialize();
	void uninitialize();
	void spin();
	void update();
	void 	CalcFPS();

	WNDCLASSEX wndclass;
	HWND hwnd;
	MSG msg;
	TCHAR szClassName[] = TEXT("RTROGL");
	bool bDone = false;

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
		"uniform mat4 u_mvp_matrix;" \
		"void main(void)"\
		"{"\
		"gl_Position =u_mvp_matrix * vPosition;" \

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
													"uniform float iTime;" \
													"uniform vec3 iResolution;" \
													"uniform float YPos;" \
													"out vec4 FragColor;" \
													"const vec3 tileSize = vec3(0.98,1.0,0.98);"\

													"vec3 marble_color(float x)"\
													"{"\
													"vec3 col;"\
													"x = 0.5*(x+1.0);"\
													"x =sqrt(x);"\
													"x =sqrt(x);"\
													"x =sqrt(x);"\
													"col = vec3(0.2+0.75*x);"\
													"col.b *=0.95;"\
													"return col;"\
													"}"\

													"float turbulence_noise(vec2 P,int numFreq)"\
													"{"\
													"float val = 0.0;"\
													"float freq = 1.0f;"\
														"for(int i=0;i<numFreq;i++)"\
														"{"\
																"val += abs(noise1(P*freq)/freq);"\
																"freq *= 2.07;"\
														"}"\
														"return val;"\
													"}"\

														"void main(void)"\
														"{"\

														"vec2 position = gl_FragCoord.xy/iResolution.xy;"\

														"float amplitude = 8.0;"\
														"const int roughness=4;"\
														"float t=36.28*position.x/tileSize.x;"\
														"t += amplitude * turbulence_noise(position.xy,roughness);"\
														"t = sin(t);"\
														"vec3 marbleColor = marble_color(t);"\
														"FragColor = vec4(marbleColor,1.0);"\
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

	iResolutionUniform = glGetUniformLocation(gShaderProgramObject, "iResolution");
	gMVPUniform = glGetUniformLocation(gShaderProgramObject, "u_mvp_matrix");
	gTexture_sampler_uniform = glGetUniformLocation(gShaderProgramObject, "u_texture0_sampler");
	gTexture_sampler_uniform1 = glGetUniformLocation(gShaderProgramObject, "u_texture1_sampler");
	gTexture_sampler_uniform2 = glGetUniformLocation(gShaderProgramObject, "u_texture2_sampler");
	mouseUniform = glGetUniformLocation(gShaderProgramObject, "iMouse");
	timeUniform = glGetUniformLocation(gShaderProgramObject, "iTime");
	noWavesUniform = glGetUniformLocation(gShaderProgramObject, "noOfWaves");
	updateXUniform = glGetUniformLocation(gShaderProgramObject, "updatex");
	updateX2Uniform = glGetUniformLocation(gShaderProgramObject, "updatex2");
	heightUniform = glGetUniformLocation(gShaderProgramObject, "height");
	yPosUniform = glGetUniformLocation(gShaderProgramObject, "YPos");

	const GLfloat squareVertices[] =
	{
		////front
		1.0f, 1.0f, 0.0f,
		-1.0f, 1.0f, 0.0f,
		-1.0f, -1.0f, 0.0f,
		1.0f, -1.0f, 0.0f,


	};

	fprintf(gpFile, "After all vertices and normals..\n");


	glGenVertexArrays(1, &gVao_Cube);
	glBindVertexArray(gVao_Cube);

	// position vbo
	glGenBuffers(1, &gVbo_CubePosition);
	glBindBuffer(GL_ARRAY_BUFFER, gVbo_CubePosition);
	glBufferData(GL_ARRAY_BUFFER, sizeof(squareVertices), squareVertices, GL_STATIC_DRAW);

	glVertexAttribPointer(VDG_ATTRIBUTE_VERTEX, 3, GL_FLOAT, GL_FALSE, 0, NULL);

	glEnableVertexAttribArray(VDG_ATTRIBUTE_VERTEX);

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
//	ToggleFullScreen();
}



void CalcFPS()
{
	
	time = timeGetTime()*0.001f;
	++frames;

	if (time - lastTime > 1.0f)
	{
		fps = frames / (time - lastTime);
		lastTime = time;
		frames = 0L;
	}

	fprintf(gpFile, "FPS:%f\n", fps);
	
	/*currTick = GetTickCount();
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

	/*static float framesPerSecond = 0.0f;
	static float lastTime = 0.0f;
	float currentTime = GetTickCount();
	++framesPerSecond;
	if (currentTime - lastTime > 1.0)
	{
		lastTime = currentTime;
		fprintf(gpFile,"CURRENT FRAMES PER SECOND %d\n\n", (int)framesPerSecond);
		framesPerSecond = 0;
	}*/

	
}



void display()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//start using OpenGL program object
	glUseProgram(gShaderProgramObject);

	glUniform3f(iResolutionUniform, 1920, 1080.0,0.0f);
	//	GLfloat time = 1000.0f;
	glUniform1f(timeUniform, time);
	glUniform1f(mouseUniform, mouse);
	glUniform1f(updateXUniform, updateX);
	glUniform1f(updateX2Uniform, updateX2);
	glUniform1f(heightUniform, heightIncr);
	glUniform1f(yPosUniform, yPos);
	//	if (gbLight == true)
	//	{
	//	glUniform1i(gLKeyPressedUniform, 1);

	//	glUniform3f(gLdUniform, 1.0f, 1.0f, 1.0f);
	//	glUniform3f(gKdUniform, 0.5f, 0.5f, 0.5f);

	//float lightPosition[] = { 0.0f,0.0f,2.0f,1.0f };
	//glUniform4fv(gLightPositonUniform, 1, (GLfloat*)lightPosition);
	//glUniform3fv(gUniformSkycolor, 1, skycolor);
	//glUniform3fv(gUniformcloudColor, 1, cloudcolor);
	//glUniform1i(gUniformScale, scaleVal);
	//glUniform1i(timeUniform, time);

	//	}
	//	else
	//	{
	//		glUniform1i(gLKeyPressedUniform, 0);
	//	}



	//OpenGL Drawing
	mat4 modelmatrix = mat4::identity();
	mat4 modelViewMatrix = mat4::identity();
	mat4 rotationMatrix = mat4::identity();
	mat4 viewmatrix = mat4::identity();
	mat4 modelviewProjectionMatrix = mat4::identity();


	viewmatrix = vmath::rotate( gAnglePyramid, 0.0f, 0.0f, 1.0f);
//	modelmatrix = translate(0.0f, 0.0f, -6.0f);
//	modelmatrix = modelmatrix * rotationMatrix;
	//	viewmatrix = scale(0.0f, 1.0f, 0.0f);

	//rotationMatrix = vmath::rotate(gAngle,1.0, 0.0, 0.0);
//	rotationMatrix = vmath::rotate(260.0f, 0.0f, 1.0f, 0.0f);
//	modelViewMatrix = modelmatrix * rotationMatrix;
	//	modelViewMatrix = modelmatrix * rotationMatrix;
	//	NoOfWaves = 0.0;

	glUniform1f(noWavesUniform, NoOfWaves);

	modelviewProjectionMatrix = gPerspectiveProjectionMatrix * viewmatrix;	//ORDER IS IMPORTANT

	glUniformMatrix4fv(gMVPUniform, 1, GL_FALSE, modelviewProjectionMatrix);

																				//	glUniformMatrix4fv(gModelViewMatrixUniform, 1, GL_FALSE, modelViewMatrix);

																				//	glUniformMatrix4fv(gProjectionMatrixUniform, 1, GL_FALSE, gPerspectiveProjectionMatrix);

glUniformMatrix4fv(gMVPUniform, 1, GL_FALSE, viewmatrix);

	glBindVertexArray(gVao_Cube);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
	glDrawArrays(GL_TRIANGLE_FAN, 4, 4);
	glDrawArrays(GL_TRIANGLE_FAN, 8, 4);
	glDrawArrays(GL_TRIANGLE_FAN, 12, 4);
	glDrawArrays(GL_TRIANGLE_FAN, 16, 4);
	glDrawArrays(GL_TRIANGLE_FAN, 20, 4);

	glBindVertexArray(0);
	glUseProgram(0);
	SwapBuffers(ghdc);

	//modelViewMatrix = mat4::identity();
	//modelViewMatrix = translate(0.0f, 0.0f, -6.0f);

	//modelViewProjectionMatrix = gPerspectiveProjectionMatrix * modelViewMatrix;

	//glUniformMatrix4fv(gMVPUniform, 1, GL_FALSE, modelViewProjectionMatrix);

	//glActiveTexture(GL_TEXTURE0);
	//glBindTexture(GL_TEXTURE_2D, gTextureSmiley);
	//glUniform1i(gTexture_sampler_uniform, 0);

	//glBindVertexArray(gVao_Smiley);
	//glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
	//glDrawArrays(GL_TRIANGLE_FAN, 4, 4);
	//glDrawArrays(GL_TRIANGLE_FAN, 8, 4);
	//glDrawArrays(GL_TRIANGLE_FAN, 12, 4);
	//glDrawArrays(GL_TRIANGLE_FAN, 16, 4);
	//glDrawArrays(GL_TRIANGLE_FAN, 20, 4);

	//glBindVertexArray(0);
	//glUseProgram(0);

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
		yPos = yPos - 0.1;
		if(yPos > 0.1)
//			YPos = 

	updateX = updateX - 0.01f;
	updateX2 = updateX2 + 0.1f;

}

void spin()
{
	/*gAngle = gAngle + 0.8f;
	if (gAngle >= 360.0f)
	gAngle = gAngle - 360.0f;*/
	//time = time + 0.1f;
}
























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
//#pragma comment(lib,"opengl32.lib")
//#pragma comment(lib,"glew32.lib")
//
//#define checkImageWidth 64
//#define checkImageHeight 64
//
////Prototype of WndProc() declared gloabally
//LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
//
////Global variable declarations
//HWND ghwnd = NULL;
//HDC ghdc = NULL;
//HGLRC ghrc = NULL;
//
//DWORD dwStyle;
//WINDOWPLACEMENT wpPrev = { sizeof(WINDOWPLACEMENT) };
//
//bool gbActiveWindow = false;
//bool gbEscapeKeyIsPressed = false;
//bool gbFullScreen = false;
//
//enum
//{
//	VDG_ATTRIBUTE_VERTEX = 0,
//	VDG_ATTRIBUTE_COLOR,
//	VDG_ATTRIBUTE_NORMAL,
//	VDG_ATTRIBUTE_TEXTURE0,
//};
//
//GLuint gVertexShaderObject;
//GLuint gFragmentShaderObject;
//GLuint gShaderProgramObject;
//
////GLuint gVao_Smiley;
////GLuint gVbo_Smiley_position;
////GLuint gVbo_Smiley_texture;
//
//GLuint gVao_CheckerBoard;
//GLuint gVao_CheckerBoard2;
////GLuint gVbo_CheckerBoard;
//
//GLuint gVao_Textures;
//GLuint gVbo_Textures;
//
//GLuint gVao_Position;
//GLuint gVbo_Position;
//GLuint gVbo_Position2;
//
//GLuint gTexture_sampler_uniform;
//
//GLuint gMVPUniform;
//GLuint gTextureSmiley;
//
//GLubyte checkImage[checkImageHeight][checkImageWidth][4];
//
//GLint IsDigitPressed;
//
//mat4 gPerspectiveProjectionMatrix;
//
//GLuint texName;
//
//FILE *gpFile = NULL;
//
////main()
//int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdLine, int nCmdShow)
//{
//	void display();
//	void initialize();
//	void uninitialize();
//
//	WNDCLASSEX wndclass;
//	HWND hwnd;
//	MSG msg;
//	TCHAR szClassName[] = TEXT("RTROGL");
//	bool bDone = false;
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
//			if (gbActiveWindow == true)
//			{
//				display();
//
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
//			gbEscapeKeyIsPressed = true;
//			break;
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
//
//		case 0x31:
//			IsDigitPressed = 1;
//			break;
//		case 0x32:
//			IsDigitPressed = 2;
//			break;
//		case 0x33:
//			IsDigitPressed = 3;
//			break;
//		case 0x34:
//			IsDigitPressed = 4;
//			break;
//
//		}
//		break;
//
//	case WM_LBUTTONDOWN:
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
//	fprintf(gpFile, "Entering Initialize..\n");
//
//	void LoadGLTextures();
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
//	glClearColor(0.0f, 0.0f, 0.0, 0.0f);
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
//		"in vec4 vPosition;" \
//		"in vec2 vTexture0_coord;" \
//		"out vec2 out_texture0_coord;" \
//		"uniform mat4 u_mvp_matrix;" \
//		"void main(void)" \
//		"{" \
//		"gl_Position = u_mvp_matrix * vPosition;" \
//		"out_texture0_coord = vTexture0_coord;" \
//		"}";
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
//
//	//*****FRAGMENT SHADER****
//	gFragmentShaderObject = glCreateShader(GL_FRAGMENT_SHADER);
//
//	//provide source code to shader
//	const GLchar *fragmentShaderSourceCode =
//		"#version 430 core" \
//		"\n" \
//		"in vec2 out_texture0_coord;" \
//		"out vec4 FragColor;" \
//		"uniform sampler2D u_texture0_sampler;" \
//		"void main(void)" \
//		"{" \
//		"FragColor = texture(u_texture0_sampler,out_texture0_coord);" \
//		"}";
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
//			}
//		}
//	}
//
//
//	//*****SHADER PROGRAM******
//	//create shader program
//	gShaderProgramObject = glCreateProgram();
//
//	//attach vertex shader to shader program
//	glAttachShader(gShaderProgramObject, gVertexShaderObject);
//
//	//attach fragment shader to shader program
//	glAttachShader(gShaderProgramObject, gFragmentShaderObject);
//
//	glBindAttribLocation(gShaderProgramObject, VDG_ATTRIBUTE_VERTEX, "vPosition");
//
//	glBindAttribLocation(gShaderProgramObject, VDG_ATTRIBUTE_TEXTURE0, "vTexture0_coord");
//
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
//	gMVPUniform = glGetUniformLocation(gShaderProgramObject, "u_mvp_matrix");
//
//	gTexture_sampler_uniform = glGetUniformLocation(gShaderProgramObject, "u_texture0_sampler");
//
//	const GLfloat texCoords[] =
//	{
//		0.0f, 0.0f,
//		0.0f, 1.0f,
//		1.0f, 1.0f,
//		1.0f, 0.0f
//	};
//
//
//	//VAO 
//	glGenVertexArrays(1, &gVao_CheckerBoard);
//	glBindVertexArray(gVao_CheckerBoard);
//
//	// VBO POSITION DYNAMIC
//	glGenBuffers(1, &gVbo_Position);
//	glBindBuffer(GL_ARRAY_BUFFER, gVbo_Position);
//
//	glBufferData(GL_ARRAY_BUFFER, 4 * 12 * (sizeof(GLfloat)), NULL, GL_DYNAMIC_DRAW);
//	glVertexAttribPointer(VDG_ATTRIBUTE_VERTEX, 3, GL_FLOAT, GL_FALSE, 0, NULL);
//	glEnableVertexAttribArray(VDG_ATTRIBUTE_VERTEX);
//	glBindBuffer(GL_ARRAY_BUFFER, 0);
//
//	//VBO TEXCOORDS STATIC
//	glGenBuffers(1, &gVbo_Textures);
//	glBindBuffer(GL_ARRAY_BUFFER, gVbo_Textures);
//
//	glBufferData(GL_ARRAY_BUFFER, sizeof(texCoords), texCoords, GL_STATIC_DRAW);
//	glVertexAttribPointer(VDG_ATTRIBUTE_TEXTURE0, 2, GL_FLOAT, GL_FALSE, 0, NULL);
//	glEnableVertexAttribArray(VDG_ATTRIBUTE_TEXTURE0);
//	glBindBuffer(GL_ARRAY_BUFFER, 0);
//	glBindVertexArray(0);
//
//
//	glShadeModel(GL_FLAT);
//	//DEPTH
//	glClearDepth(1.0f);
//	glEnable(GL_DEPTH_TEST);
//	glDepthFunc(GL_LEQUAL);
//	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
//	//	glEnable(GL_TEXTURE_2D);
//	LoadGLTextures();
//	gPerspectiveProjectionMatrix = mat4::identity();
//
//	resize(WIN_WIDTH, WIN_HEIGHT);
//	fprintf(gpFile, "Exitting Initialize..\n");
//
//}
//
//void display()
//{
//	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//	fprintf(gpFile, "Entering Display\n");
//
//	//start using OpenGL program object
//	glUseProgram(gShaderProgramObject);
//	mat4 modelViewMatrix;
//	mat4 rotationMatrix;
//	mat4 modelViewProjectionMatrix;
//
//	modelViewMatrix = mat4::identity();
//	modelViewMatrix = translate(0.0f, 0.0f, -3.6f);
//
//	modelViewProjectionMatrix = gPerspectiveProjectionMatrix * modelViewMatrix;
//
//	glUniformMatrix4fv(gMVPUniform, 1, GL_FALSE, modelViewProjectionMatrix);
//
//	//Start Recording our Data
//	glBindVertexArray(gVao_CheckerBoard);
//
//	GLfloat quadPos[12];
//
//	quadPos[0] = -2.0f;
//	quadPos[1] = -1.0f;
//	quadPos[2] = 0.0f;
//	quadPos[3] = -2.0f;
//	quadPos[4] = 1.0f;
//	quadPos[5] = 0.0f;
//	quadPos[6] = 0.0f;
//	quadPos[7] = 1.0f;
//	quadPos[8] = 0.0f;
//	quadPos[9] = 0.0f;
//	quadPos[10] = -1.0f;
//	quadPos[11] = 0.0f;
//
//	glBindBuffer(GL_ARRAY_BUFFER, gVbo_Position);
//	glBufferData(GL_ARRAY_BUFFER, sizeof(quadPos), quadPos, GL_DYNAMIC_DRAW);
//	glBindBuffer(GL_ARRAY_BUFFER, 0);
//
//	//Bind our Texture Data
//	glActiveTexture(GL_TEXTURE0);
//	glBindTexture(GL_TEXTURE_2D, texName);
//	glUniform1i(gTexture_sampler_uniform, 0);
//
//	//Geometric entity specific Data
//	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
//	glDrawArrays(GL_TRIANGLE_FAN, 4, 4);
//	glDrawArrays(GL_TRIANGLE_FAN, 8, 4);
//	glDrawArrays(GL_TRIANGLE_FAN, 12, 4);
//	glDrawArrays(GL_TRIANGLE_FAN, 16, 4);
//	glDrawArrays(GL_TRIANGLE_FAN, 20, 4);
//
//	glBindVertexArray(0);
//
//
//	//////////////////////////////////
//	glBindVertexArray(gVao_CheckerBoard);
//
//	GLfloat quadPos2[12];
//
//	quadPos2[0] = 1.0f;
//	quadPos2[1] = -1.0f;
//	quadPos2[2] = 0.0f;
//	quadPos2[3] = 1.0f;
//	quadPos2[4] = 1.0f;
//	quadPos2[5] = 0.0f;
//	quadPos2[6] = 2.41421f;
//	quadPos2[7] = 1.0f;
//	quadPos2[8] = -1.41421f;
//	quadPos2[9] = 2.41421f;
//	quadPos2[10] = -1.0f;
//	quadPos2[11] = -1.42421f;
//
//	glBindBuffer(GL_ARRAY_BUFFER, gVbo_Position);
//	glBufferData(GL_ARRAY_BUFFER, sizeof(quadPos2), quadPos2, GL_DYNAMIC_DRAW);
//	glBindBuffer(GL_ARRAY_BUFFER, 0);
//
//	//Bind our Texture Data
//	glActiveTexture(GL_TEXTURE0);
//	glBindTexture(GL_TEXTURE_2D, texName);
//	glUniform1i(gTexture_sampler_uniform, 0);
//
//	//Geometric entity specific Data
//	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
//	glDrawArrays(GL_TRIANGLE_FAN, 4, 4);
//	glDrawArrays(GL_TRIANGLE_FAN, 8, 4);
//	glDrawArrays(GL_TRIANGLE_FAN, 12, 4);
//	glDrawArrays(GL_TRIANGLE_FAN, 16, 4);
//	glDrawArrays(GL_TRIANGLE_FAN, 20, 4);
//
//	glBindVertexArray(0);
//
//
//	glUseProgram(0);
//	fprintf(gpFile, "Exiting Display\n");
//
//	SwapBuffers(ghdc);
//}
//
//void LoadGLTextures(void)
//{
//	fprintf(gpFile, "Inside LoadGLTextures\n");
//
//	void MakeCheckImage();
//
//	HBITMAP hBitmap;
//
//	MakeCheckImage();
//
//	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
//
//	glGenTextures(1, &texName);
//
//	glBindTexture(GL_TEXTURE_2D, texName);
//
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
//
//	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, checkImageWidth, checkImageHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, checkImage);
//
//	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
//
//	fprintf(gpFile, "Exitting LoadGLTextures");
//}
//
//
//void MakeCheckImage()
//{
//
//	int i, j, c;
//	for (i = 0;i < checkImageHeight; i++)
//	{
//		for (j = 0; j < checkImageWidth; j++)
//		{
//			c = (((i & 0x8) == 0) ^ ((j & 0x8) == 0)) * 255;
//
//			checkImage[i][j][0] = (GLubyte)c;
//			checkImage[i][j][1] = (GLubyte)c;
//			checkImage[i][j][2] = (GLubyte)c;
//			checkImage[i][j][3] = (GLubyte)255;
//		}
//	}
//}
//
//void resize(int width, int height)
//
//{
//	if (height == 0)
//		height = 1;
//	glViewport(0, 0, (GLsizei)width, (GLsizei)height);
//	gPerspectiveProjectionMatrix = perspective(60.0f, (GLfloat)width / (GLfloat)height, 1.0f, 30.0f);
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
//	if (gVao_CheckerBoard)
//	{
//		glDeleteVertexArrays(1, &gVao_CheckerBoard);
//		gVao_CheckerBoard = 0;
//	}
//
//	if (gVbo_Position)
//	{
//		glDeleteBuffers(1, &gVbo_Position);
//		gVbo_Position = 0;
//	}
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
//
//
