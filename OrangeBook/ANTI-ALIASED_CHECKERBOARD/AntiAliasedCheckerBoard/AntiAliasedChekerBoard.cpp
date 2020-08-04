////

////////////////////////////////////////////////////////////////////// SIR'S CODE CHECKERBOARD DYNAMIC
////#include<Windows.h>
////#include<stdio.h>
////#include<gl/glew.h>
////#include<gl/GL.h>
////
////#include"vmath.h"
////
////using namespace vmath;
////
////#define WIN_WIDTH 800
////#define WIN_HEIGHT 600
////
////#pragma comment(lib,"opengl32.lib")
////#pragma comment(lib,"glew32.lib")
////
////#define checkImageWidth 64
////#define checkImageHeight 64
////
//////Prototype of WndProc() declared gloabally
////LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
////
//////Global variable declarations
////HWND ghwnd = NULL;
////HDC ghdc = NULL;
////HGLRC ghrc = NULL;
////
////DWORD dwStyle;
////WINDOWPLACEMENT wpPrev = { sizeof(WINDOWPLACEMENT) };
////
////bool gbActiveWindow = false;
////bool gbEscapeKeyIsPressed = false;
////bool gbFullScreen = false;
////
////enum
////{
////	VDG_ATTRIBUTE_VERTEX = 0,
////	VDG_ATTRIBUTE_COLOR,
////	VDG_ATTRIBUTE_NORMAL,
////	VDG_ATTRIBUTE_TEXTURE0,
////};
////
////GLuint gVertexShaderObject;
////GLuint gFragmentShaderObject;
////GLuint gShaderProgramObject;
////
//////GLuint gVao_Smiley;
//////GLuint gVbo_Smiley_position;
//////GLuint gVbo_Smiley_texture;
////
////GLuint gVao_CheckerBoard;
////GLuint gVao_CheckerBoard2;
//////GLuint gVbo_CheckerBoard;
////
////GLuint gVao_Textures;
////GLuint gVbo_Textures;
////
////GLuint gVao_Position;
////GLuint gVbo_Position;
////GLuint gVbo_Position2;
////
////GLuint gTexture_sampler_uniform;
////
////GLuint gMVPUniform;
////GLuint gTextureSmiley;
////
////GLubyte checkImage[checkImageHeight][checkImageWidth][4];
////
////GLint IsDigitPressed;
////
////mat4 gPerspectiveProjectionMatrix;
////
////GLuint texName;
////
////FILE *gpFile = NULL;
////
//////main()
////int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdLine, int nCmdShow)
////{
////	void display();
////	void initialize();
////	void uninitialize();
////
////	WNDCLASSEX wndclass;
////	HWND hwnd;
////	MSG msg;
////	TCHAR szClassName[] = TEXT("RTROGL");
////	bool bDone = false;
////
////	fopen_s(&gpFile, "LogFile.txt", "w");
////	if (gpFile == NULL)
////	{
////		printf("Error opening file\n");
////		exit(0);
////	}
////
////	//initializing members of wndclass
////	wndclass.cbSize = sizeof(WNDCLASSEX);
////	wndclass.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
////	wndclass.cbClsExtra = 0;
////	wndclass.cbWndExtra = 0;
////	wndclass.hInstance = hInstance;
////	wndclass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
////	wndclass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
////	wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
////	wndclass.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
////	wndclass.lpfnWndProc = WndProc;
////	wndclass.lpszClassName = szClassName;
////	wndclass.lpszMenuName = NULL;
////
////
////	//Registering class
////	RegisterClassEx(&wndclass);
////
////	hwnd = CreateWindowEx(WS_EX_APPWINDOW,
////		szClassName,
////		TEXT("OpenGL Programmable PipeLine Native Windowing"),
////		WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_VISIBLE,
////		0,
////		0,
////		WIN_WIDTH,
////		WIN_HEIGHT,
////		NULL,
////		NULL,
////		hInstance,
////		NULL);
////
////	ghwnd = hwnd;
////
////	initialize();
////
////	ShowWindow(hwnd, SW_SHOW);
////	SetForegroundWindow(hwnd);
////	SetFocus(hwnd);
////
////	//Game Loop
////	while (bDone == false)
////	{
////		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
////		{
////			if (msg.message == WM_QUIT)
////				bDone = true;
////			else
////			{
////				TranslateMessage(&msg);
////				DispatchMessage(&msg);
////			}
////		}
////		else
////		{
////			if (gbActiveWindow == true)
////			{
////				display();
////
////				if (gbEscapeKeyIsPressed == true)
////					bDone = true;
////			}
////		}
////	}
////
////	uninitialize();
////	fprintf(gpFile, "Exitting Main..\n");
////	return((int)msg.wParam);
////}
////
////
////LRESULT CALLBACK WndProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
////{
////	void display();
////	void resize(int, int);
////	void ToggleFullScreen(void);
////	void uninitialize(void);
////
////	fprintf(gpFile, "Entering WndProc..\n");
////
////	switch (iMsg)
////	{
////	case WM_ACTIVATE:
////		if (HIWORD(wParam) == 0)
////			gbActiveWindow = true;
////		else
////			gbActiveWindow = false;
////		break;
////
////	case WM_SIZE:
////		resize(LOWORD(lParam), HIWORD(lParam));
////		break;
////
////	case WM_KEYDOWN:
////		switch (wParam)
////		{
////		case VK_ESCAPE:
////			gbEscapeKeyIsPressed = true;
////			break;
////		case 0x46:
////			if (gbFullScreen == false)
////			{
////				ToggleFullScreen();
////				gbFullScreen = true;
////			}
////			else
////			{
////				ToggleFullScreen();
////				gbFullScreen = false;
////			}
////			break;
////
////		case 0x31:
////			IsDigitPressed = 1;
////			break;
////		case 0x32:
////			IsDigitPressed = 2;
////			break;
////		case 0x33:
////			IsDigitPressed = 3;
////			break;
////		case 0x34:
////			IsDigitPressed = 4;
////			break;
////
////		}
////		break;
////
////	case WM_LBUTTONDOWN:
////		break;
////
////	case WM_DESTROY:
////		PostQuitMessage(0);
////		break;
////	}
////	return(DefWindowProc(hwnd, iMsg, wParam, lParam));
////	fprintf(gpFile, "Exitting WndProc..\n");
////
////}
////
////void ToggleFullScreen(void)
////{
////	MONITORINFO mi;
////
////	if (gbFullScreen == false)
////	{
////		dwStyle = GetWindowLong(ghwnd, GWL_STYLE);
////		if (dwStyle & WS_OVERLAPPEDWINDOW)
////		{
////			mi = { sizeof(MONITORINFO) };
////			if (GetWindowPlacement(ghwnd, &wpPrev) && GetMonitorInfo(MonitorFromWindow(ghwnd, MONITORINFOF_PRIMARY), &mi))
////			{
////				SetWindowLong(ghwnd, GWL_STYLE, dwStyle & ~WS_OVERLAPPEDWINDOW);
////				SetWindowPos(ghwnd, HWND_TOP, mi.rcMonitor.left,
////					mi.rcMonitor.top,
////					mi.rcMonitor.right - mi.rcMonitor.left,
////					mi.rcMonitor.bottom - mi.rcMonitor.top,
////					SWP_NOZORDER | SWP_FRAMECHANGED);
////			}
////		}
////		ShowCursor(FALSE);
////	}
////	else
////	{
////		SetWindowLong(ghwnd, GWL_STYLE, dwStyle | WS_OVERLAPPEDWINDOW);
////		SetWindowPlacement(ghwnd, &wpPrev);
////		SetWindowPos(ghwnd, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOOWNERZORDER | SWP_NOZORDER | SWP_FRAMECHANGED);
////		ShowCursor(TRUE);
////	}
////}
////
////
////
////void initialize(void)
////{
////
////	void resize(int, int);
////	void uninitialize();
////	fprintf(gpFile, "Entering Initialize..\n");
////
////	void LoadGLTextures();
////
////	//Variable 
////	PIXELFORMATDESCRIPTOR pfd;
////	int iPixelFormatIndex;
////	//GLint num;
////
////	//code
////	ZeroMemory(&pfd, sizeof(PIXELFORMATDESCRIPTOR));
////
////	//initialize code
////	pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
////	pfd.nVersion = 1;
////	pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
////	pfd.iPixelType = PFD_TYPE_RGBA;
////	pfd.cColorBits = 8;
////	pfd.cRedBits = 8;
////	pfd.cGreenBits = 8;
////	pfd.cBlueBits = 8;
////	pfd.cAlphaBits = 8;
////
////	ghdc = GetDC(ghwnd);
////
////	iPixelFormatIndex = ChoosePixelFormat(ghdc, &pfd);
////	if (iPixelFormatIndex == 0)
////	{
////		ReleaseDC(ghwnd, ghdc);
////		ghdc = NULL;
////	}
////	if (SetPixelFormat(ghdc, iPixelFormatIndex, &pfd) == FALSE)
////	{
////		ReleaseDC(ghwnd, ghdc);
////		ghdc = NULL;
////	}
////
////	ghrc = wglCreateContext(ghdc);
////	if (ghrc == NULL)
////	{
////		ReleaseDC(ghwnd, ghdc);
////		ghdc = NULL;
////	}
////
////	if (wglMakeCurrent(ghdc, ghrc) == NULL)
////	{
////		wglDeleteContext(ghrc);
////		ghrc = NULL;
////		ReleaseDC(ghwnd, ghdc);
////		ghdc = NULL;
////	}
////
////	glClearColor(0.0f, 0.0f, 0.0, 0.0f);
////	GLenum glew_error = glewInit();
////	if (glew_error != GLEW_OK)
////	{
////		wglDeleteContext(ghrc);
////		ghrc = NULL;
////		ReleaseDC(ghwnd, ghdc);
////		ghdc = NULL;
////	}
////
////	//*****VERTEX SHADER*******
////	//create shader 
////	gVertexShaderObject = glCreateShader(GL_VERTEX_SHADER);
////
////	//provide source code to shader
////	const GLchar * vertexShaderShaderCode =
////		"#version 430 core" \
////		"\n" \
////		"in vec4 vPosition;" \
////		"in vec2 vTexture0_coord;" \
////		"out vec2 out_texture0_coord;" \
////		"uniform mat4 u_mvp_matrix;" \
////		"void main(void)" \
////		"{" \
////		"gl_Position = u_mvp_matrix * vPosition;" \
////		"out_texture0_coord = vTexture0_coord;" \
////		"}";
////
////	glShaderSource(gVertexShaderObject, 1, (const GLchar**)&vertexShaderShaderCode, NULL);
////
////	//compile shader
////	glCompileShader(gVertexShaderObject);
////
////	GLint iInfoLength = 0;
////	GLint iShaderCompiledStatus = 0;
////	char * szInfoLog = NULL;
////
////	glGetShaderiv(gVertexShaderObject, GL_COMPILE_STATUS, &iShaderCompiledStatus);
////	if (iShaderCompiledStatus == GL_FALSE)
////	{
////		glGetShaderiv(gVertexShaderObject, GL_INFO_LOG_LENGTH, &iInfoLength);
////		if (iInfoLength > 0)
////		{
////			szInfoLog = (char *)malloc(iInfoLength);
////			if (szInfoLog != NULL)
////			{
////				GLsizei written;
////				glGetShaderInfoLog(gVertexShaderObject, iInfoLength, &written, szInfoLog);
////				fprintf(gpFile, "Vertex Shader Compilation Log:%s\n", szInfoLog);
////				free(szInfoLog);
////				uninitialize();
////				exit(0);
////			}
////		}
////	}
////
////
////
////	//*****FRAGMENT SHADER****
////	gFragmentShaderObject = glCreateShader(GL_FRAGMENT_SHADER);
////
////	//provide source code to shader
////	const GLchar *fragmentShaderSourceCode =
////		"#version 430 core" \
////		"\n" \
////		"in vec2 out_texture0_coord;" \
////		"out vec4 FragColor;" \
////		"uniform sampler2D u_texture0_sampler;" \
////		"void main(void)" \
////		"{" \
////		"FragColor = texture(u_texture0_sampler,out_texture0_coord);" \
////		"}";
////
////	glShaderSource(gFragmentShaderObject, 1, (const GLchar**)&fragmentShaderSourceCode, NULL);
////
////	//compile shader
////	glCompileShader(gFragmentShaderObject);
////	glGetShaderiv(gFragmentShaderObject, GL_COMPILE_STATUS, &iShaderCompiledStatus);
////	if (iShaderCompiledStatus == GL_FALSE)
////	{
////		glGetShaderiv(gFragmentShaderObject, GL_INFO_LOG_LENGTH, &iShaderCompiledStatus);
////		if (iInfoLength > 0)
////		{
////			szInfoLog = (char *)malloc(iInfoLength);
////			if (szInfoLog != NULL)
////			{
////				GLsizei written;
////				glGetShaderInfoLog(gFragmentShaderObject, iInfoLength, &written, szInfoLog);
////				fprintf(gpFile, "Fragment shader compilation Log %s\n", szInfoLog);
////				free(szInfoLog);
////				uninitialize();
////				exit(0);
////			}
////		}
////	}
////
////
////	//*****SHADER PROGRAM******
////	//create shader program
////	gShaderProgramObject = glCreateProgram();
////
////	//attach vertex shader to shader program
////	glAttachShader(gShaderProgramObject, gVertexShaderObject);
////
////	//attach fragment shader to shader program
////	glAttachShader(gShaderProgramObject, gFragmentShaderObject);
////
////	glBindAttribLocation(gShaderProgramObject, VDG_ATTRIBUTE_VERTEX, "vPosition");
////
////	glBindAttribLocation(gShaderProgramObject, VDG_ATTRIBUTE_TEXTURE0, "vTexture0_coord");
////
////	//link shader
////	glLinkProgram(gShaderProgramObject);
////
////	GLint iShaderProgramLinkStatus = 0;
////	glGetProgramiv(gShaderProgramObject, GL_LINK_STATUS, &iShaderCompiledStatus);
////	if (iShaderCompiledStatus == GL_FALSE)
////	{
////		glGetProgramiv(gShaderProgramObject, GL_INFO_LOG_LENGTH, &iInfoLength);
////		if (iInfoLength > 0)
////		{
////			szInfoLog = (char*)malloc(iInfoLength);
////			if (szInfoLog != NULL)
////			{
////				GLsizei written;
////				glGetProgramInfoLog(gShaderProgramObject, iInfoLength, &written, szInfoLog);
////				fprintf(gpFile, "Shader Program Link Status %s\n", szInfoLog);
////				free(szInfoLog);
////				uninitialize();
////				exit(0);
////			}
////		}
////	}
////
////	gMVPUniform = glGetUniformLocation(gShaderProgramObject, "u_mvp_matrix");
////
////	gTexture_sampler_uniform = glGetUniformLocation(gShaderProgramObject, "u_texture0_sampler");
////
////	const GLfloat texCoords[] =
////	{
////		0.0f, 0.0f,
////		0.0f, 1.0f,
////		1.0f, 1.0f,
////		1.0f, 0.0f
////	};
////
////	
////	//VAO 
////	glGenVertexArrays(1, &gVao_CheckerBoard);
////	glBindVertexArray(gVao_CheckerBoard);
////
////	// VBO POSITION DYNAMIC
////	glGenBuffers(1, &gVbo_Position);
////	glBindBuffer(GL_ARRAY_BUFFER, gVbo_Position);
////
////	glBufferData(GL_ARRAY_BUFFER,4 *12 *(sizeof(GLfloat)), NULL, GL_DYNAMIC_DRAW);
////	glVertexAttribPointer(VDG_ATTRIBUTE_VERTEX, 3, GL_FLOAT, GL_FALSE, 0, NULL);
////	glEnableVertexAttribArray(VDG_ATTRIBUTE_VERTEX);
////	glBindBuffer(GL_ARRAY_BUFFER, 0);
////
////	//VBO TEXCOORDS STATIC
////	glGenBuffers(1, &gVbo_Textures);
////	glBindBuffer(GL_ARRAY_BUFFER, gVbo_Textures);
////
////	glBufferData(GL_ARRAY_BUFFER, sizeof(texCoords), texCoords, GL_STATIC_DRAW);
////	glVertexAttribPointer(VDG_ATTRIBUTE_TEXTURE0, 2, GL_FLOAT, GL_FALSE, 0, NULL);
////	glEnableVertexAttribArray(VDG_ATTRIBUTE_TEXTURE0);
////	glBindBuffer(GL_ARRAY_BUFFER, 0);
////	glBindVertexArray(0);
////
////
////	glShadeModel(GL_FLAT);
////	//DEPTH
////	glClearDepth(1.0f);
////	glEnable(GL_DEPTH_TEST);
////	glDepthFunc(GL_LEQUAL);
////	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
////	//	glEnable(GL_TEXTURE_2D);
////	LoadGLTextures();
////	gPerspectiveProjectionMatrix = mat4::identity();
////
////	resize(WIN_WIDTH, WIN_HEIGHT);
////	fprintf(gpFile, "Exitting Initialize..\n");
////
////}
////
////void display()
////{
////	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
////	fprintf(gpFile, "Entering Display\n");
////
////	//start using OpenGL program object
////	glUseProgram(gShaderProgramObject);
////	mat4 modelViewMatrix;
////	mat4 rotationMatrix;
////	mat4 modelViewProjectionMatrix;
////
////	modelViewMatrix = mat4::identity();
////	modelViewMatrix = translate(0.0f, 0.0f, -3.6f);
////
////	modelViewProjectionMatrix = gPerspectiveProjectionMatrix * modelViewMatrix;
////
////	glUniformMatrix4fv(gMVPUniform, 1, GL_FALSE, modelViewProjectionMatrix);
////
////	//Start Recording our Data
////	glBindVertexArray(gVao_CheckerBoard);
////
////	GLfloat quadPos[12];
////
////	quadPos[0] = -2.0f;
////	quadPos[1] = -1.0f;
////	quadPos[2] =  0.0f;
////	quadPos[3] = -2.0f;
////	quadPos[4] = 1.0f;
////	quadPos[5] = 0.0f;
////	quadPos[6] = 0.0f;
////	quadPos[7] = 1.0f;
////	quadPos[8] = 0.0f;
////	quadPos[9] = 0.0f;
////	quadPos[10] = -1.0f;
////	quadPos[11] = 0.0f;
////
////	glBindBuffer(GL_ARRAY_BUFFER, gVbo_Position);
////	glBufferData(GL_ARRAY_BUFFER, sizeof(quadPos), quadPos, GL_DYNAMIC_DRAW);
////	glBindBuffer(GL_ARRAY_BUFFER, 0);
////
////	//Bind our Texture Data
////	glActiveTexture(GL_TEXTURE0);
////	glBindTexture(GL_TEXTURE_2D, texName);
////	glUniform1i(gTexture_sampler_uniform, 0);
////
////	//Geometric entity specific Data
////	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
////	glDrawArrays(GL_TRIANGLE_FAN, 4, 4);
////	glDrawArrays(GL_TRIANGLE_FAN, 8, 4);
////	glDrawArrays(GL_TRIANGLE_FAN, 12, 4);
////	glDrawArrays(GL_TRIANGLE_FAN, 16, 4);
////	glDrawArrays(GL_TRIANGLE_FAN, 20, 4);
////
////	glBindVertexArray(0);
////
////
////	//////////////////////////////////
////	glBindVertexArray(gVao_CheckerBoard);
////	
////	GLfloat quadPos2[12];
////
////	quadPos2[0] = 1.0f;
////	quadPos2[1] = -1.0f;
////	quadPos2[2] = 0.0f;
////	quadPos2[3] = 1.0f;
////	quadPos2[4] = 1.0f;
////	quadPos2[5] = 0.0f;
////	quadPos2[6] = 2.41421f;
////	quadPos2[7] = 1.0f;
////	quadPos2[8] = -1.41421f;
////	quadPos2[9] = 2.41421f;
////	quadPos2[10] = -1.0f;
////	quadPos2[11] = -1.42421f;
////
////	glBindBuffer(GL_ARRAY_BUFFER, gVbo_Position);
////	glBufferData(GL_ARRAY_BUFFER, sizeof(quadPos2), quadPos2, GL_DYNAMIC_DRAW);
////	glBindBuffer(GL_ARRAY_BUFFER, 0);
////
////	//Bind our Texture Data
////	glActiveTexture(GL_TEXTURE0);
////	glBindTexture(GL_TEXTURE_2D, texName);
////	glUniform1i(gTexture_sampler_uniform, 0);
////
////	//Geometric entity specific Data
////	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
////	glDrawArrays(GL_TRIANGLE_FAN, 4, 4);
////	glDrawArrays(GL_TRIANGLE_FAN, 8, 4);
////	glDrawArrays(GL_TRIANGLE_FAN, 12, 4);
////	glDrawArrays(GL_TRIANGLE_FAN, 16, 4);
////	glDrawArrays(GL_TRIANGLE_FAN, 20, 4);
////
////	glBindVertexArray(0);
////	
////
////	glUseProgram(0);
////	fprintf(gpFile, "Exiting Display\n");
////
////	SwapBuffers(ghdc);
////}
////
////void LoadGLTextures(void)
////{
////	fprintf(gpFile, "Inside LoadGLTextures\n");
////
////	void MakeCheckImage();
////
////	HBITMAP hBitmap;
////	
////	MakeCheckImage();
////
////	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
////
////	glGenTextures(1, &texName);
////
////	glBindTexture(GL_TEXTURE_2D, texName);
////
////	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
////	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
////	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
////	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
////
////	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, checkImageWidth, checkImageHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, checkImage);
////
////	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
////
////	fprintf(gpFile, "Exitting LoadGLTextures");
////}
////
////
////void MakeCheckImage()
////{
////
////	int i, j, c;
////	for (i = 0;i < checkImageHeight; i++)
////	{
////		for (j = 0; j < checkImageWidth; j++)
////		{
////			c = (((i & 0x8) == 0) ^ ((j & 0x8) == 0)) * 255;
////
////			checkImage[i][j][0] = (GLubyte)c;
////			checkImage[i][j][1] = (GLubyte)c;
////			checkImage[i][j][2] = (GLubyte)c;
////			checkImage[i][j][3] = (GLubyte)255;
////		}
////	}
////}
////
////void resize(int width, int height)
////
////{
////	if (height == 0)
////		height = 1;
////	glViewport(0, 0, (GLsizei)width, (GLsizei)height);
////	gPerspectiveProjectionMatrix = perspective(60.0f, (GLfloat)width / (GLfloat)height, 1.0f, 30.0f);
////}
////
////void uninitialize(void)
////{
////
////	if (gbFullScreen == true)
////	{
////		dwStyle = GetWindowLong(ghwnd, GWL_STYLE);
////		SetWindowLong(ghwnd, GWL_STYLE, dwStyle | WS_OVERLAPPEDWINDOW);
////		SetWindowPlacement(ghwnd, &wpPrev);
////		SetWindowPos(ghwnd, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOOWNERZORDER | SWP_NOZORDER | SWP_FRAMECHANGED);
////		ShowCursor(TRUE);
////	}
////	if (gVao_CheckerBoard)
////	{
////		glDeleteVertexArrays(1, &gVao_CheckerBoard);
////		gVao_CheckerBoard = 0;
////	}
////
////	if (gVbo_Position)
////	{
////		glDeleteBuffers(1, &gVbo_Position);
////		gVbo_Position = 0;
////	}
////
////
////	//detach vertex shader from shader program object
////	glDetachShader(gShaderProgramObject, gVertexShaderObject);
////
////	//detach fragment shader from shader program object
////	glDetachShader(gShaderProgramObject, gFragmentShaderObject);
////
////	//delete vertex shader object
////	glDeleteShader(gVertexShaderObject);
////	gVertexShaderObject = 0;
////
////	//delete fragment shader object
////	glDeleteShader(gFragmentShaderObject);
////	gFragmentShaderObject = 0;
////
////	//delete shader program object
////	glDeleteProgram(gShaderProgramObject);
////	gShaderProgramObject = 0;
////
////	//unlink shader program
////	glUseProgram(0);
////
////
////	wglMakeCurrent(NULL, NULL);
////
////	wglDeleteContext(ghrc);
////	ghrc = NULL;
////
////	ReleaseDC(ghwnd, ghdc);
////	ghdc = NULL;
////
////	DestroyWindow(ghwnd);
////	ghwnd = NULL;
////
////
////}
////
////
////
















/////////////////////////////////////////////////////////////ANTI ALIASING CODE ORANGEBOOK ////////////////////////////////////////////////


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

#define checkImageWidth 64
#define checkImageHeight 64

//Prototype of WndProc() declared gloabally
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

//Global variable declarations
HWND ghwnd = NULL;
HDC ghdc = NULL;
HGLRC ghrc = NULL;

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

//GLuint gVao_Smiley;
//GLuint gVbo_Smiley_position;
//GLuint gVbo_Smiley_texture;

GLuint gVao_CheckerBoard;
GLuint gVao_CheckerBoard2;
//GLuint gVbo_CheckerBoard;

GLuint gVao_Textures;
GLuint gVbo_Textures;

GLuint gVao_Position;
GLuint gVbo_Position;
GLuint gVbo_Position2;

GLuint gTexture_sampler_uniform;

GLuint gMVPUniform;
GLuint gTextureSmiley;

GLubyte checkImage[checkImageHeight][checkImageWidth][4];

GLint IsDigitPressed;

mat4 gPerspectiveProjectionMatrix;

GLuint texName;
GLuint uniColor1;
GLuint uniColor2;
GLuint frequency;
GLuint uniAvgcolor;

FILE *gpFile = NULL;

//main()
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdLine, int nCmdShow)
{
	void display();
	void initialize();
	void uninitialize();

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
			if (gbActiveWindow == true)
			{
				display();

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

		case 0x31:
			IsDigitPressed = 1;
			break;
		case 0x32:
			IsDigitPressed = 2;
			break;
		case 0x33:
			IsDigitPressed = 3;
			break;
		case 0x34:
			IsDigitPressed = 4;
			break;

		}
		break;

	case WM_LBUTTONDOWN:
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
	fprintf(gpFile, "Entering Initialize..\n");

	void LoadGLTextures();

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

	glClearColor(0.0f, 0.0f, 0.0, 0.0f);
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
		"in vec2 vTexture0_coord;" \
		"out vec2 out_texture0_coord;" \
		"uniform mat4 u_mvp_matrix;" \
		"void main(void)" \
		"{" \
		"gl_Position = u_mvp_matrix * vPosition;" \
		"out_texture0_coord = vTexture0_coord;" \
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

	//provide source code to shader
	const GLchar *fragmentShaderSourceCode =
		"#version 430 core" \
		"\n" \
		"in vec2 out_texture0_coord;" \
		"out vec4 FragColor;" \

		"uniform vec3 Color1;"\
		"uniform vec3 Color2;"\
		"uniform vec3 AvgColor;"\
		"uniform float frequency;"\

		"uniform sampler2D u_texture0_sampler;" \
		"void main(void)" \
		"{" \
		"vec3 color;"\
		"vec2 fw = fwidth(out_texture0_coord);"\
		"vec2 fuzz = fw * frequency * 2.0;"\
		"float fuzzMax = max(fuzz.s,fuzz.t);"\
		"vec2 checkPos = fract(out_texture0_coord * frequency);"\
		"if(fuzzMax < 0.5)"\
		"{"\
		"vec2 p =smoothstep(vec2(0.5),fuzz + vec2(0.5),checkPos) + (1.0-smoothstep(vec2(0.0),fuzz,checkPos));"\
		"color = mix(Color1,Color2,p.x * p.y + (1.0 - p.x) * (1.0 - p.y));"\
		"color = mix(color,AvgColor,smoothstep(0.125,0.5,fuzzMax));"\
		"}"\
		"else"\
		"{"\
		"color = AvgColor;"\
		"}"\
		"FragColor = vec4(color,1.0);" \
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

	glBindAttribLocation(gShaderProgramObject, VDG_ATTRIBUTE_TEXTURE0, "vTexture0_coord");

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

	gMVPUniform = glGetUniformLocation(gShaderProgramObject, "u_mvp_matrix");

	gTexture_sampler_uniform = glGetUniformLocation(gShaderProgramObject, "u_texture0_sampler");
	uniColor1 = glGetUniformLocation(gShaderProgramObject, "Color1");
	uniColor2 = glGetUniformLocation(gShaderProgramObject, "Color2");
	uniAvgcolor = glGetUniformLocation(gShaderProgramObject, "AvgColor");
	frequency = glGetUniformLocation(gShaderProgramObject, "frequency");

	const GLfloat texCoords[] =
	{
		0.0f, 0.0f,
		0.0f, 1.0f,
		1.0f, 1.0f,
		1.0f, 0.0f
	};


	//VAO 
	glGenVertexArrays(1, &gVao_CheckerBoard);
	glBindVertexArray(gVao_CheckerBoard);

	// VBO POSITION DYNAMIC
	glGenBuffers(1, &gVbo_Position);
	glBindBuffer(GL_ARRAY_BUFFER, gVbo_Position);

	glBufferData(GL_ARRAY_BUFFER, 4 * 12 * (sizeof(GLfloat)), NULL, GL_DYNAMIC_DRAW);
	glVertexAttribPointer(VDG_ATTRIBUTE_VERTEX, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(VDG_ATTRIBUTE_VERTEX);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	//VBO TEXCOORDS STATIC
	glGenBuffers(1, &gVbo_Textures);
	glBindBuffer(GL_ARRAY_BUFFER, gVbo_Textures);

	glBufferData(GL_ARRAY_BUFFER, sizeof(texCoords), texCoords, GL_STATIC_DRAW);
	glVertexAttribPointer(VDG_ATTRIBUTE_TEXTURE0, 2, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(VDG_ATTRIBUTE_TEXTURE0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);


	glShadeModel(GL_FLAT);
	//DEPTH
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	//	glEnable(GL_TEXTURE_2D);
//	LoadGLTextures();
	gPerspectiveProjectionMatrix = mat4::identity();

	resize(WIN_WIDTH, WIN_HEIGHT);
	fprintf(gpFile, "Exitting Initialize..\n");

}

void display()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	fprintf(gpFile, "Entering Display\n");

	//start using OpenGL program object
	glUseProgram(gShaderProgramObject);

	glUniform3f(uniColor1, 1.0f, 1.0f, 1.0f);
	glUniform3f(uniColor2, 0.0f, 0.0f, 0.0f);
	glUniform3f(uniAvgcolor, 0.0, 0.0, 0.0);
	glUniform1f(frequency, 4.0f);

	mat4 modelViewMatrix;
	mat4 rotationMatrix;
	mat4 modelViewProjectionMatrix;

	modelViewMatrix = mat4::identity();
	modelViewMatrix = translate(0.0f, 0.0f, -3.6f);

	modelViewProjectionMatrix = gPerspectiveProjectionMatrix * modelViewMatrix;

	glUniformMatrix4fv(gMVPUniform, 1, GL_FALSE, modelViewProjectionMatrix);

	//Start Recording our Data
	glBindVertexArray(gVao_CheckerBoard);

	GLfloat quadPos[12];

	quadPos[0] = -2.0f;
	quadPos[1] = -1.0f;
	quadPos[2] = 0.0f;
	quadPos[3] = -2.0f;
	quadPos[4] = 1.0f;
	quadPos[5] = 0.0f;
	quadPos[6] = 0.0f;
	quadPos[7] = 1.0f;
	quadPos[8] = 0.0f;
	quadPos[9] = 0.0f;
	quadPos[10] = -1.0f;
	quadPos[11] = 0.0f;

	glBindBuffer(GL_ARRAY_BUFFER, gVbo_Position);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadPos), quadPos, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	//Bind our Texture Data
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texName);
	glUniform1i(gTexture_sampler_uniform, 0);

	//Geometric entity specific Data
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
	glDrawArrays(GL_TRIANGLE_FAN, 4, 4);
	glDrawArrays(GL_TRIANGLE_FAN, 8, 4);
	glDrawArrays(GL_TRIANGLE_FAN, 12, 4);
	glDrawArrays(GL_TRIANGLE_FAN, 16, 4);
	glDrawArrays(GL_TRIANGLE_FAN, 20, 4);

	glBindVertexArray(0);


	//////////////////////////////////
	glBindVertexArray(gVao_CheckerBoard);

	GLfloat quadPos2[12];

	quadPos2[0] = 1.0f;
	quadPos2[1] = -1.0f;
	quadPos2[2] = 0.0f;
	quadPos2[3] = 1.0f;
	quadPos2[4] = 1.0f;
	quadPos2[5] = 0.0f;
	quadPos2[6] = 2.41421f;
	quadPos2[7] = 1.0f;
	quadPos2[8] = -1.41421f;
	quadPos2[9] = 2.41421f;
	quadPos2[10] = -1.0f;
	quadPos2[11] = -1.42421f;

	glBindBuffer(GL_ARRAY_BUFFER, gVbo_Position);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadPos2), quadPos2, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	//Bind our Texture Data
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texName);
	glUniform1i(gTexture_sampler_uniform, 0);

	//Geometric entity specific Data
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
	glDrawArrays(GL_TRIANGLE_FAN, 4, 4);
	glDrawArrays(GL_TRIANGLE_FAN, 8, 4);
	glDrawArrays(GL_TRIANGLE_FAN, 12, 4);
	glDrawArrays(GL_TRIANGLE_FAN, 16, 4);
	glDrawArrays(GL_TRIANGLE_FAN, 20, 4);

	glBindVertexArray(0);


	glUseProgram(0);
	fprintf(gpFile, "Exiting Display\n");

	SwapBuffers(ghdc);
}

void LoadGLTextures(void)
{
	fprintf(gpFile, "Inside LoadGLTextures\n");

	void MakeCheckImage();

	HBITMAP hBitmap;

//	MakeCheckImage();

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	glGenTextures(1, &texName);

	glBindTexture(GL_TEXTURE_2D, texName);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, checkImageWidth, checkImageHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, checkImage);

	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

	fprintf(gpFile, "Exitting LoadGLTextures");
}

void resize(int width, int height)

{
	if (height == 0)
		height = 1;
	glViewport(0, 0, (GLsizei)width, (GLsizei)height);
	gPerspectiveProjectionMatrix = perspective(60.0f, (GLfloat)width / (GLfloat)height, 1.0f, 30.0f);
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
	if (gVao_CheckerBoard)
	{
		glDeleteVertexArrays(1, &gVao_CheckerBoard);
		gVao_CheckerBoard = 0;
	}

	if (gVbo_Position)
	{
		glDeleteBuffers(1, &gVbo_Position);
		gVbo_Position = 0;
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



