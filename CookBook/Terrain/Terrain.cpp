
#include <Windows.h>
#include<stdio.h>
#include<gl/glew.h>
#include<gl/GL.h>
#include"glm/glm/glm.hpp"
#include"glm/glm/gtc/matrix_transform.hpp"
#include"glm/glm/gtc/type_ptr.hpp"
#include<math.h>
#include<iostream>
#include<fstream>
#include<algorithm>

#include"Common.h"
#include"SOIL2/SOIL2.h"
#include"resource.h"

#define WIN_WIDTH 1280
#define WIN_HEIGHT 960

#pragma comment(lib,"glew32.lib")
#pragma comment(lib,"opengl32.lib")
#pragma comment(lib,"soil2-debug.lib")
#pragma comment(lib, "SOIL_static_x86_d.lib")



//Prototype of WndProc() declared Globally
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);


//Global variable declarations
FILE *gpFile = NULL;

GLfloat gAnglePyramid = 0.0f;
GLfloat gAngleCube = 0.0f;

HWND ghwnd = NULL;
HDC ghdc = NULL;
HGLRC ghrc = NULL;

const float EPSILON = 0.0001f;


DWORD dwStyle;
WINDOWPLACEMENT wpPrev = { sizeof(WINDOWPLACEMENT) };

bool gbActiveWindow = false;
bool gbEscapeKeyIsPressed = false;
bool gbFullScreen = false;

GLuint gVertexShaderObject;
GLuint gGeometryShaderObject;
GLuint gFragmentShaderObject;
GLuint gShaderProgramObject;

GLuint gVao_Pyramid;
GLuint gVbo_Position_Pyramid;
GLuint gVbo_Color_Pyramid;

GLuint gVao_Cube;
GLuint gVbo_Position_Cube;
GLuint gVbo_Color_Cube;

GLuint gMVPUniform;
GLuint volumeSamplerUniform;
GLuint heightMapTextureUniform;
GLuint scaleUniform;
GLuint half_scaleUniform;
GLuint HALF_TERRAIN_SIZE_UNIFORM;

GLuint vaoID;
GLuint vboVerticesID;
GLuint vboIndicesID;

GLuint heightMapTextureID;

const int TERRAIN_WIDTH = 512;
const int TERRAIN_DEPTH = 512;
const int TERRAIN_HALF_WIDTH = TERRAIN_WIDTH >> 1;
const int TERRAIN_HALF_DEPTH = TERRAIN_DEPTH >> 1;

const int TOTAL = (TERRAIN_WIDTH*TERRAIN_DEPTH);
const int TOTAL_INDICES = TOTAL * 2 * 3;

const string filename = "heightmap512x512.png";

glm::vec3 vertices[TOTAL];
GLuint indices[TOTAL_INDICES];

glm::mat4 P = glm::mat4(1);
glm::mat4 MV = glm::mat4(1);

float scaleval = 100.0f;
float half_scale = scaleval / 2.0f;

//Grid Object

//camera transform variables
int state = 0, oldX = 0, oldY = 0;
float rX = 25, rY = -40, dist = -7;

//volume vertex array and buffer objects
GLuint volumeVBO;
GLuint volumeVAO;

//maximum number of slices
const int MAX_SLICES = 256;

//sliced vertices
glm::vec3 vTextureSlices[MAX_SLICES * 12];///6144

//backroud color
glm::vec4 bg = glm::vec4(0.5, 0.5, 1, 1);
//glm::vec4 bg = glm::vec4(0.0, 0.0, 0.0, 1);

//volume data files 
const std::string volume_file = "Engine256.raw";

//dimensions of volume data
const int XDIM = 256;
const int YDIM = 256;
const int ZDIM = 256;

//total number of slices current used
int num_slices = 256;

//OpenGL Volume texture id
GLuint textureID;

//flag to see if view is rotated
//volume is resliced if the view is rotated
bool bViewRotated = true;



//function to get the max (abs) dimension of the given vertex v
int FindAbsMax(glm::vec3 v)
{
	v = glm::abs(v);
	int max_dim = 0;
	float val = v.x;
	if (v.y > val)
	{
		val = v.y;
		max_dim = 1;
	}
	if (v.z > val)
	{
		val = v.z;
		max_dim = 2;
	}
	return max_dim;
}

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
	int x = 0, y = 0;
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

		case 0x57:
			if (state == 0)
			{
				dist += (y - oldX) / 50.0f;
			}
			else
			{
				rX += (y - oldX) / 5.0f;
				rY += (x - oldX) / 5.0f;
				bViewRotated = true;
			}
			oldX = x;
			oldY = y;
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

	//	num_slices = min(MAX_SLICES, max(num_slices, 3));
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
	GLubyte *SOIL_load_image(const char*, int*, int*, int*, int);
	int LoadGLTextures(GLuint *, TCHAR[]);

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

	//glClearColor(0.0f, 0.0f, 1.0f, 0.0f);
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
		"#version 330 core" \
		"\n" \
		"in vec3 vPosition;" \
		"void main(void)" \
		"{" \
		"gl_Position = vec4(vPosition.xyz,1);" \
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
				fprintf(gpFile, "Vertex Shader Compilation Log volume shader:%s\n", szInfoLog);
				free(szInfoLog);
				uninitialize();
				exit(0);
			}
		}
	}




	/////////////////************* Geometry Shader ***************////////////////

	gGeometryShaderObject = glCreateShader(GL_GEOMETRY_SHADER);

	//provide source code to shader
	const GLchar *geometryShaderSourceCode =
		"#version 330 core" \
		"\n" \
		"layout (triangles) in;"\
		"layout(triangle_strip, max_vertices = 3) out;"\
		"uniform mat4 u_mvp_matrix;"\

		"uniform ivec2 HALF_TERRAIN_SIZE;"\
		"uniform sampler2D heightMapTexture;"\
		"uniform float scale;"\
		"uniform float half_scale;"\
		"void main (void)" \
		"{" \
		"for(int i=0;i<gl_in.length();i++)"\
		"{"\
		"vec4 Position = gl_in[i].gl_Position;"\
		"float height = texture(heightMapTexture,Position.xz).r*scale - half_scale;"\
		"vec2 xz = (Position.xz*2.0-1)*HALF_TERRAIN_SIZE;"\
		"gl_Position = u_mvp_matrix*vec4(xz.x, height, xz.y, 1);"\
		"EmitVertex();"\
		"}"\
		"EndPrimitive();"\
		"}";

	glShaderSource(gGeometryShaderObject, 1, (const GLchar**)&geometryShaderSourceCode, NULL);

	//compile shader
	glCompileShader(gGeometryShaderObject);
	glGetShaderiv(gGeometryShaderObject, GL_COMPILE_STATUS, &iShaderCompiledStatus);
	if (iShaderCompiledStatus == GL_FALSE)
	{
		glGetShaderiv(gGeometryShaderObject, GL_INFO_LOG_LENGTH, &iShaderCompiledStatus);
		if (iInfoLength > 0)
		{
			szInfoLog = (char *)malloc(iInfoLength);
			if (szInfoLog != NULL)
			{
				GLsizei written;
				glGetShaderInfoLog(gGeometryShaderObject, iInfoLength, &written, szInfoLog);
				fprintf(gpFile, "Geometry Shader Compilation Log Volume shader: %s\n", szInfoLog);
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
		"#version 330 core" \
		"\n" \
		"out vec4 FragColor;" \
		"void main (void)" \
		"{" \
		"FragColor = vec4(1.0,1.0,1.0,1.0);" \
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
				fprintf(gpFile, "Fragment Shader Compilation Log Volume shader: %s\n", szInfoLog);
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
	glAttachShader(gShaderProgramObject, gGeometryShaderObject);

	//attach fragment shader to shader program
	glAttachShader(gShaderProgramObject, gFragmentShaderObject);

	//pre-link binding of shader program object with vertex position attribute
	glBindAttribLocation(gShaderProgramObject, VDG_ATTRIBUTE_VERTEX, "vPosition");


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
				fprintf(gpFile, "Shader Program LinK Log Volume Shader %s\n");
				free(szInfoLog);
				uninitialize();
				exit(0);
			}
		}
	}


	//get MVP uniform locaion
	gMVPUniform = glGetUniformLocation(gShaderProgramObject, "u_mvp_matrix");
	heightMapTextureUniform = glGetUniformLocation(gShaderProgramObject, "heightMapTexture");
	scaleUniform = glGetUniformLocation(gShaderProgramObject, "scale");
	half_scaleUniform = glGetUniformLocation(gShaderProgramObject, "half_scale");
	HALF_TERRAIN_SIZE_UNIFORM = glGetUniformLocation(gShaderProgramObject, "HALF_TERRAIN_SIZE");



	//**** vertices, colors, shader attribs, vbo, vao initializations ****//


	//fill indices array
	GLuint* id = &indices[0];
	int i = 0, j = 0;

	//setup vertices 
	int count = 0;


	for (j = 0; j < TERRAIN_DEPTH; j++) {
		for (i = 0; i < TERRAIN_WIDTH; i++) {
			/*
			vertices[count] = glm::vec3( ( (float(i)/(TERRAIN_WIDTH-1))*2.0f-1)*TERRAIN_HALF_WIDTH,
										 (pData[count]/255.0f)*scale-half_scale,
										 ( (float(j)/(TERRAIN_DEPTH-1))*2.0-1)*TERRAIN_HALF_DEPTH);
										 */
			vertices[count] = glm::vec3((float(i) / (TERRAIN_WIDTH - 1)),
				0,
				(float(j) / (TERRAIN_DEPTH - 1)));
			count++;
		}
	}

	for (i = 0; i < TERRAIN_DEPTH - 1; i++) {
		for (j = 0; j < TERRAIN_WIDTH - 1; j++) {
			int i0 = j + i * TERRAIN_WIDTH;
			int i1 = i0 + 1;
			int i2 = i0 + TERRAIN_WIDTH;
			int i3 = i2 + 1;
			*id++ = i0;
			*id++ = i2;
			*id++ = i1;
			*id++ = i1;
			*id++ = i2;
			*id++ = i3;
		}
	}


	//setup vao and vbo stuff
	glGenVertexArrays(1, &vaoID);
	glGenBuffers(1, &vboVerticesID);
	glGenBuffers(1, &vboIndicesID);

	glBindVertexArray(vaoID);

	glBindBuffer(GL_ARRAY_BUFFER, vboVerticesID);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices[0], GL_STATIC_DRAW);
	glVertexAttribPointer(VDG_ATTRIBUTE_VERTEX, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(VDG_ATTRIBUTE_VERTEX);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vboIndicesID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), &indices[0], GL_STATIC_DRAW);

	//load texture using SOIL
	int texture_width = 0, texture_height = 0, format = 0;
	GLubyte* pData = SOIL_load_image(filename.c_str(), &texture_width, &texture_height, &format, SOIL_LOAD_L);


	glGenTextures(1, &heightMapTextureID);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, heightMapTextureID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, texture_width, texture_height, 0, GL_RED, GL_UNSIGNED_BYTE, pData);

	free(pData);

	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);


}

void display(void)
{

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glm::mat4 T = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, dist));
	glm::mat4 Rx = glm::rotate(T, rX, glm::vec3(1.0f, 0.0f, 0.0f));
	glm::mat4 Ry = glm::rotate(Rx, rY, glm::vec3(0.0f, 1.0f, 0.0));
	glm::mat4 MV = Ry;
	glm::mat4 MVP = P * MV;

	glUseProgram(gShaderProgramObject);
	glUniform1i(heightMapTextureUniform, 0);
	glUniform2i(HALF_TERRAIN_SIZE_UNIFORM, TERRAIN_WIDTH >> 1, TERRAIN_DEPTH >> 1);
	glUniform1f(scaleUniform, scaleval);
	glUniform1f(half_scaleUniform, half_scale);


	glUniformMatrix4fv(gMVPUniform, 1, GL_FALSE, glm::value_ptr(MVP));
	glDrawElements(GL_TRIANGLES, TOTAL_INDICES, GL_UNSIGNED_INT, 0);
	glUseProgram(0);


	SwapBuffers(ghdc);

}

void resize(int width, int height)
{
	//code
	if (height == 0)
		height = 1;
	glViewport(0, 0, (GLsizei)width, (GLsizei)height);

	P = glm::perspective(45.0f, (GLfloat)width / (GLfloat)height, 0.1f, 1000.0f);
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







