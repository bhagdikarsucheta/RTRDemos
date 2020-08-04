
#include <Windows.h>
#include<stdio.h>
#include<gl/glew.h>
#include<gl/GL.h>

#include"vmath.h"
#include"object.h"
//#include"sb7ktx.h"
#include"ktx_loading_header.h"

#define WIN_WIDTH 800
#define WIN_HEIGHT 600

#pragma comment(lib,"glew32.lib")
#pragma comment(lib,"opengl32.lib")

using namespace vmath;

enum
{
	VDG_ATTRIBUTE_VERTEX = 0,
	VDG_ATTRIBUTE_COLOR,
	VDG_ATTRIBUTE_NORMAL,
	VDG_ATTRIBUTE_TEXTURE0,
};

//Prototype of WndProc() declared Globally
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);


//Global variable declarations
FILE *gpFile = NULL;

GLfloat gAnglePyramid = 0.0f;
GLfloat gAngleCube = 0.0f;

HWND ghwnd = NULL;
HDC ghdc = NULL;
HGLRC ghrc = NULL;


DWORD dwStyle;
WINDOWPLACEMENT wpPrev = { sizeof(WINDOWPLACEMENT) };

bool gbActiveWindow = false;
bool gbEscapeKeyIsPressed = false;
bool gbFullScreen = false;

GLuint gVertexShaderObject;
GLuint gFragmentShaderObject;
GLuint gShaderProgramObject;

GLuint gVao_Pyramid;
GLuint gVbo_Position_Pyramid;
GLuint gVbo_Color_Pyramid;

GLuint gVao_Cube;
GLuint gVbo_Position_Cube;
GLuint gVbo_Color_Cube;

GLint gMVMatrix;
GLint proj_matrix;
GLint light_posUniform;
//GLuint textureColor, textureNormal;

struct
{
	GLuint color;
	GLuint normals;
}textures;

sb7::object object;

GLfloat gWidth, gHeight;

mat4 gPerspectiveProjectionMatrix;

//main()
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdLine, int nCmdShow)
{
	//function prototype
	void display();
	void initialize();
	void uninitialize(void);
	void update();

	//variable declaration
	WNDCLASSEX wndclass;
	HWND hwnd;
	MSG msg;
	TCHAR szClassName[] = TEXT("OpenGLPP");
	bool bDone = false;

	if (fopen_s(&gpFile, "Log.txt", "w") != 0)
	{
		MessageBox(NULL, TEXT("Log File Can not be Created\Exitting.."), TEXT("ERROR"), MB_OK | MB_TOPMOST | MB_ICONSTOP);
		exit(0);
	}
	else
	{
		fprintf(gpFile, "Log file is Successfully Openend \n");
	}

	//code
	//initializing member of WNDCLASS
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

	//Registering Class
	RegisterClassEx(&wndclass);

	//CreateWindow
	hwnd = CreateWindowEx(WS_EX_APPWINDOW,
		szClassName,
		TEXT("OpenGL Programmable PipeLine Using Native Windowing: First ortho Trianle Window Shree Ganeshaya Namaha"),
		WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_VISIBLE,
		0,
		0,
		WIN_WIDTH,
		WIN_HEIGHT,
		NULL,
		NULL,
		hInstance,
		NULL
	);

	ghwnd = hwnd;

	//intitialize()
	initialize();

	ShowWindow(hwnd, SW_SHOW);
	SetForegroundWindow(hwnd);
	SetFocus(hwnd);

	//Message Loop
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
			//display();
			if (gbActiveWindow == true)
			{
				update();
				display();

				if (gbEscapeKeyIsPressed == true)
					bDone = true;
			}
		}
	}

	uninitialize();
	return((int)msg.wParam);

}

//WndProc()
LRESULT CALLBACK WndProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	//function prototypes
	//	void display();
	void resize(int, int);
	void ToggleFullScreen(void);
	void uninitialize(void);

	//code
	switch (iMsg)
	{
	case WM_ACTIVATE:
		if (HIWORD(wParam) == 0)
			gbActiveWindow = true;
		else
			gbActiveWindow = false;
		break;
	case WM_PAINT:
		//		display();
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

		case 0x46:			//for 'f' or 'F'
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
		default:
			break;
		}
		break;


	case WM_LBUTTONDOWN:
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	default:
		break;
	}
	return(DefWindowProc(hwnd, iMsg, wParam, lParam));

}


void ToggleFullScreen(void)
{
	//variable declarations
	MONITORINFO mi;

	//code
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
		//code
		SetWindowLong(ghwnd, GWL_STYLE, dwStyle | WS_OVERLAPPEDWINDOW);
		SetWindowPlacement(ghwnd, &wpPrev);
		SetWindowPos(ghwnd, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOOWNERZORDER | SWP_NOZORDER | SWP_FRAMECHANGED);
		ShowCursor(TRUE);
	}
}

void initialize()
{
	//function prototype
	void uninitialize(void);
	void resize(int, int);

	//variable declaration
	PIXELFORMATDESCRIPTOR pfd;
	int iPixelFormatIndex;

	//code
	ZeroMemory(&pfd, sizeof(PIXELFORMATDESCRIPTOR));

	//Initialization code
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

	if (wglMakeCurrent(ghdc, ghrc) == FALSE)
	{
		wglDeleteContext(ghrc);
		ghrc = NULL;
		ReleaseDC(ghwnd, ghdc);
		ghdc = NULL;
	}

	glClearColor(0.0f, 0.0f, 1.0f, 0.0f);
	GLenum glew_error = glewInit();
	if (glew_error != GLEW_OK)
	{
		wglDeleteContext(ghrc);
		ghrc = NULL;
		ReleaseDC(ghwnd, ghdc);
		ghdc = NULL;
	}

	//******** VERTEX SHADER ******
	//create shader
	gVertexShaderObject = glCreateShader(GL_VERTEX_SHADER);

	//provide source code to shader
	const GLchar *vertexShaderSourceCode =
		"#version 430 core" \
		"\n" \
		"layout (location=0) in vec4 vPosition;"\
		"layout (location=1) in vec3 normal;"\
		"layout (location=2) in vec3 tangent;"\
		"layout (location=3) in vec2 texCoord;"\

		"out VS_OUT{"\
			"vec2 texcoord;"\
			"vec3 eyeDir;"\
			"vec3 lightDir;"\
			"vec3 normal;"\
		"}vs_out;"\
		
		"uniform mat4 mv_matrix;"\
		"uniform mat4 proj_matrix;"\
		"uniform vec3 light_pos=vec3(0.0,0.0,100.0);"\
		
		//"uniform vec3 light_pos;"\

		"void main(void)" \
		"{" \
			"vec4 P = mv_matrix*vPosition;"\
			//
			"vec3 V = P.xyz;"\
			"vec3 N = normalize(mat3(mv_matrix)*normal);"\
			"vec3 T = normalize(mat3(mv_matrix)*tangent);"\

			"vec3 B = cross(N,T);"\

			"vec3 L = light_pos - P.xyz;"\
			"vs_out.lightDir = normalize(vec3(dot(L,T),dot(L,B),dot(L,N)));"\
			
			"V = -P.xyz;"\
			"vs_out.eyeDir = normalize(vec3(dot(V,T),dot(V,B),dot(V,N)));"\

			"vs_out.texcoord = texCoord;"\

			"vs_out.normal = N;"\
			
			"gl_Position = proj_matrix * P;" \
		"}";

	glShaderSource(gVertexShaderObject, 1, (const GLchar **)&vertexShaderSourceCode, NULL);

	//compile shader
	glCompileShader(gVertexShaderObject);
	GLint iInfoLength = 0;
	GLint iShaderCompiledStatus = 0;
	char *szInfoLog = NULL;

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

	//**********FRAGMENT SHADER*********
	//create shader
	gFragmentShaderObject = glCreateShader(GL_FRAGMENT_SHADER);

	//provide source code to shader
	const GLchar *fragmentShaderSourceCode =
		"#version 430 core" \
		"\n" \
		"layout (binding=0) uniform sampler2D tex_color;"\
		"layout (binding=1) uniform sampler2D tex_normal;"\

		"in VS_OUT{"\
			"vec2 texcoord;"\
			"vec3 eyeDir;"\
			"vec3 lightDir;"\
			"vec3 normal;"\
		"}fs_in;"\

		"out vec4 FragColor;" \

		"void main (void)" \
		"{" \
			"vec3 V = normalize(fs_in.eyeDir);"\
			"vec3 L = normalize(fs_in.lightDir);"\
			"vec3 N = normalize(texture(tex_normal, fs_in.texcoord).rgb * 2.0 - vec3(1.0));"\
		
			"vec3 R = reflect(-L,N);"\
		
			"vec3 diffuse_albedo = texture(tex_color,fs_in.texcoord).rgb;"\

			"vec3 diffuse = max(dot(N, L), 0.0) * diffuse_albedo;"\
			"vec3 specular_albedo = vec3(1.0);"\
			"vec3 specular = max(pow(dot(R, V), 20.0), 0.0) * specular_albedo;"\
			
			"FragColor = vec4(diffuse+specular,1.0);" \

		//"FragColor = vec4(1.0);" \

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
				fprintf(gpFile, "Fragment Shader Compilation Log : %s\n", szInfoLog);
				free(szInfoLog);
				uninitialize();
				exit(0);
			}
		}
	}

	//********** SHADER PROGRAM *********
	//create
	gShaderProgramObject = glCreateProgram();

	//attach vertex shader to shader program
	glAttachShader(gShaderProgramObject, gVertexShaderObject);

	//attach fragment shader to shader program
	glAttachShader(gShaderProgramObject, gFragmentShaderObject);

	////pre-link binding of shader program object with vertex position attribute
	//glBindAttribLocation(gShaderProgramObject, 0, "vPosition");
	//glBindAttribLocation(gShaderProgramObject, 1, "normal");
	//glBindAttribLocation(gShaderProgramObject, 2, "tangent");
	//glBindAttribLocation(gShaderProgramObject, 3, "texCoord");

	//glBindAttribLocation(gShaderProgramObject, VDG_ATTRIBUTE_COLOR, "vColor");

	//Link Shader
	glLinkProgram(gShaderProgramObject);
	GLint iShaderProgramLinkStatus = 0;
	glGetProgramiv(gShaderProgramObject, GL_LINK_STATUS, &iShaderProgramLinkStatus);
	if (iShaderProgramLinkStatus == GL_FALSE)
	{
		glGetProgramiv(gShaderProgramObject, GL_INFO_LOG_LENGTH, &iInfoLength);
		if (iInfoLength > 0)
		{
			szInfoLog = (char *)malloc(iInfoLength);
			if (szInfoLog != NULL)
			{
				GLsizei written;
				glGetProgramInfoLog(gShaderProgramObject, iInfoLength, &written, szInfoLog);
				fprintf(gpFile, "Shader Program LinK Log %s\n");
				free(szInfoLog);
				uninitialize();
				exit(0);
			}
		}
	}


	//get MVP uniform locaion
	gMVMatrix = glGetUniformLocation(gShaderProgramObject, "mv_matrix");
	proj_matrix = glGetUniformLocation(gShaderProgramObject, "proj_matrix");
	light_posUniform = glGetUniformLocation(gShaderProgramObject, "light_pos");

	//textureColor = glGetUniformLocation(gShaderProgramObject, "tex_color");
	//textureNormal = glGetUniformLocation(gShaderProgramObject, "tex_normal");
	//**** vertices, colors, shader attribs, vbo, vao initializations ****//

	//
	//const GLfloat cubeVertices[] =
	//{
	//	//FRONT FACE
	//	 1.0f, 1.0f, 1.0f,
	//	-1.0f, 1.0f, 1.0f,
	//	-1.0f, -1.0f, 1.0f,
	//	 1.0f, -1.0f, 1.0f,

	//};

	////Start of Cube Vao
	//glGenVertexArrays(1, &gVao_Cube);
	//glBindVertexArray(gVao_Cube);

	////VBO of Cube Pos
	//glGenBuffers(1, &gVbo_Position_Cube);
	//glBindBuffer(GL_ARRAY_BUFFER, gVbo_Position_Cube);
	//glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);
	//glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	//glEnableVertexAttribArray(0);

	//glBindBuffer(GL_ARRAY_BUFFER, 0);

	//glBindVertexArray(0);


	//glShadeModel(GL_FLAT);
	//glClearDepth(1.0f);
	//glDepthFunc(GL_LEQUAL);
	//glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	//glBackFace(GL_CW);
	//glEnable(GL_DEPTH_TEST);
	//glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

//	glFrontFace(GL_CW);
//	glCullFace(GL_BACK);
//	glEnable(GL_CULL_FACE);

	glActiveTexture(GL_TEXTURE0);
	textures.color = load("E:/RealTimeRendering2018/Trial_NormalMapping/Trial_NormalMapping/ladybug_co.ktx",0);

	glActiveTexture(GL_TEXTURE1);
	textures.normals = load("E:/RealTimeRendering2018/Trial_NormalMapping/Trial_NormalMapping/ladybug_nm.ktx",0);
	//glBindTexture(GL_TEXTURE_2D, textureNormal);
	object.load("E:/RealTimeRendering2018/Trial_NormalMapping/Trial_NormalMapping/ladybug.sbm");

	//gPerspectiveProjectionMatrix = mat4::identity();  
	
	//resize(WIN_WIDTH, WIN_HEIGHT);

}

void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	static const GLfloat zeros[] = { 0.0f, 0.0f, 0.0f, 0.0f };
	static const GLfloat gray[] = { 0.1f, 0.1f, 0.1f, 0.0f };
	static const GLfloat ones[] = { 1.0f };
	static double last_time = 0.0;
	static double total_time = 0.0;
	double current_time = GetTickCount();
	//start using OpenGL program Object

	total_time += (current_time - last_time);	
	last_time = current_time;

	const float f = (float)total_time;

	glClearBufferfv(GL_COLOR, 0, gray);
	glClearBufferfv(GL_DEPTH, 0, ones);


	glViewport(0, 0, gWidth, gHeight);
	glEnable(GL_DEPTH_TEST);
	glUseProgram(gShaderProgramObject);

	//OpenL Drawing 
	//set modelview & modelviewprojection matrices to identity 
	mat4 modelviewMatrix = mat4::identity();
	mat4 modelviewProjectionMatrix = mat4::identity();
	gPerspectiveProjectionMatrix = perspective(50.0f, (GLfloat)gWidth / (GLfloat)gHeight, 0.1f, 1000.0f);

	modelviewMatrix = vmath::translate(0.0f, -0.2f, -5.5f) *
		vmath::rotate(14.5f, 1.0f, 0.0f, 0.0f) *
		vmath::rotate(-20.0f, 0.0f, 1.0f, 0.0f) *
		//vmath::rotate(t * 14.5f, 0.0f, 1.0f, 0.0f) *
		//vmath::rotate(0.0f, 1.0f, 0.0f, 0.0f) *
		vmath::mat4::identity();

	modelviewProjectionMatrix = gPerspectiveProjectionMatrix * modelviewMatrix;	//ORDER IS IMPORTANT
	//glUniformMatrix4fv(gMVPUniform, 1, GL_FALSE, modelviewProjectionMatrix);


	glUniformMatrix4fv(gMVMatrix, 1, GL_FALSE, modelviewMatrix);
	glUniformMatrix4fv(proj_matrix, 1, GL_FALSE, modelviewProjectionMatrix);
	glUniform3fv(light_posUniform, 1, vmath::vec3(40.0f * sinf(f), 30.0f + 20.0f * cosf(f), 40.0f));


	//glActiveTexture(GL_TEXTURE0);
	//glUniform1i(textureColor, 0);
	//glActiveTexture(GL_TEXTURE1);
	////glBindTexture(GL_TEXTURE_2D, textureNormal);
	//glUniform1i(textureNormal, 1);

	/*glUniform1i(tex_displacement, 0);
	glUniform1i(tex_color, 0);*/

	//*** bind vao ****

	//glBindVertexArray(gVao_Cube);
	//glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

	////glDrawArrays(GL_TRIANGLE_FAN, 4, 4);
	////glDrawArrays(GL_TRIANGLE_FAN, 8, 4);
	////glDrawArrays(GL_TRIANGLE_FAN, 12, 4);
	////glDrawArrays(GL_TRIANGLE_FAN, 16, 4);
	////glDrawArrays(GL_TRIANGLE_FAN, 20, 4);
	//
	//glBindVertexArray(0);				
	//
	object.render();

	glUseProgram(0);

	SwapBuffers(ghdc);

}

void resize(int width, int height)
{
	//code
	if (height == 0)
		height = 1;
	glViewport(0, 0, (GLsizei)width, (GLsizei)height);

	//gPerspectiveProjectionMatrix = perspective(45.0f, (GLfloat)width / (GLfloat)height, 0.1f, 100.0f);

	gWidth = width;
	gHeight = height;
}

void uninitialize(void)
{
	//UNINITIALIZATION CODE
	if (gbFullScreen == true)
	{
		dwStyle = GetWindowLong(ghwnd, GWL_STYLE);
		SetWindowLong(ghwnd, GWL_STYLE, dwStyle | WS_OVERLAPPEDWINDOW);
		SetWindowPlacement(ghwnd, &wpPrev);
		SetWindowPos(ghwnd, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOOWNERZORDER | SWP_NOZORDER | SWP_FRAMECHANGED);
		ShowCursor(TRUE);
	}

	//destroy vao
	if (gVao_Pyramid)
	{
		glDeleteVertexArrays(1, &gVao_Pyramid);
		gVao_Pyramid = 0;
	}

	//destroy vbo
	if (gVbo_Position_Pyramid)
	{
		glDeleteVertexArrays(1, &gVbo_Position_Pyramid);
		gVbo_Position_Pyramid = 0;
	}

	if (gVbo_Color_Pyramid)
	{
		glDeleteVertexArrays(1, &gVbo_Color_Pyramid);
		gVbo_Color_Pyramid = 0;
	}

	if (gVao_Cube)
	{
		glDeleteVertexArrays(1, &gVao_Cube);
		gVao_Cube = 0;
	}

	if (gVbo_Position_Cube)
	{
		glDeleteVertexArrays(1, &gVbo_Position_Cube);
		gVbo_Position_Cube = 0;
	}

	if (gVbo_Color_Cube)
	{
		glDeleteVertexArrays(1, &gVbo_Color_Cube);
		gVbo_Color_Cube = 0;
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
	gAnglePyramid = gAnglePyramid + 0.1f;
	if (gAnglePyramid >= 360.0f)
		gAnglePyramid = 0.0f;

	gAngleCube = gAngleCube + 0.1;;
	if (gAngleCube >= 360.0f)
		gAngleCube = 0.0f;
}














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
//
//
//
//
//
//
//
//









