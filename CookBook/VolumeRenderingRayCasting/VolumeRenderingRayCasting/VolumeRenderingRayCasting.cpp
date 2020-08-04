
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
#include"Grid.h"
#define WIN_WIDTH 1280
#define WIN_HEIGHT 960

#pragma comment(lib,"glew32.lib")
#pragma comment(lib,"opengl32.lib")


GLuint cubeVao;
GLuint cubeVbo;
GLuint cubeIndicesID;
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
GLuint gFragmentShaderObject;	
GLuint gShaderProgramObject;


GLuint gMVPUniform;
GLuint volumeSamplerUniform;
GLuint camUniform;
GLuint step_sizeUniform;


//Grid Object
CGrid *grid;

//modelview and projection matrices
glm::mat4 MV, P;

//camera transform variables
int state = 0, oldX = 0, oldY = 0;
float rX = 3, rY = 0, dist = -2.5;


//backroud color
glm::vec4 bg = glm::vec4(0.5, 0.5, 1, 1);
//glm::vec4 bg = glm::vec4(0.0, 0.0, 0.0, 1);

//volume data files 
const std::string volume_file = "E:/RealTimeRendering2018/Trial_Volume_Rendering_RayCasting/Trial_VolumeRendering/08_SHADERS_3D_PYRAMID_CUBE/Engine256.raw";

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


bool LoadVolume()
{
	std::ifstream infile(volume_file.c_str(), std::ios_base::binary);

	if (infile.good())
	{
		//read volume data file
		GLubyte *pData = new GLubyte[XDIM*YDIM*ZDIM];
		infile.read(reinterpret_cast<char*>(pData), XDIM*YDIM*ZDIM * sizeof(GLubyte));
		infile.close();

		


		//generate OpenGL texture
		glGenTextures(1, &textureID);
		glBindTexture(GL_TEXTURE_3D, textureID);

		// set the texture parameters
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

		//set the mipmap levels (base and max)
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_BASE_LEVEL, 0);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAX_LEVEL, 4);

		//allocate data with internal format and foramt as (GL_RED)		
		//glTexImage3D(GL_TEXTURE_3D,0,GL_LUMINANCE,XDIM,YDIM,ZDIM,0,GL_LUMINANCE,GL_UNSIGNED_BYTE,pData);
		glTexImage3D(GL_TEXTURE_3D, 0, GL_R8, XDIM, YDIM, ZDIM, 0, GL_RED, GL_UNSIGNED_BYTE, pData);
		//GL_CHECK_ERRORS

			//generate mipmaps
			glGenerateMipmap(GL_TEXTURE_3D);

		delete[] pData;
		return true;

	}
	else
	{
		return false;
	}
}

mat4 gPerspectiveProjectionMatrix;


//function to get the max (abs) dimension of the given vertex v



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
	int x=0, y=0;
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
		case 0x52:
			/*if (state == 0)
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
			oldY = y;*/
			if (rX <= 100)
			{
				rX = rX + 0.1f;
			}
			if (rY <= 50)
			{
				rY = rY + 0.1f;
			}

			/*if (dist < -0.6)
			{
				dist = dist + 0.05f;
			}*/
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

	grid = new CGrid(20, 20);

	//******** VERTEX SHADER ******
	//create shader
	gVertexShaderObject = glCreateShader(GL_VERTEX_SHADER);

	//provide source code to shader
	const GLchar *vertexShaderSourceCode =
		"#version 330 core" \
		"\n" \
		"in vec4 vPosition;" \
		"uniform mat4 u_mvp_matrix;" \
		"smooth out vec3 vUV;" \
		"void main(void)" \
		"{" \
		"gl_Position = u_mvp_matrix * vPosition;" \
		"vUV = vPosition.xyz+vec3(0.5);" \
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

	//**********FRAGMENT SHADER*********
	//create shader
	gFragmentShaderObject = glCreateShader(GL_FRAGMENT_SHADER);

	//provide source code to shader
	const GLchar *fragmentShaderSourceCode =
		"#version 330 core" \
		"\n" \
		"smooth in vec3 vUV;" \
		"out vec4 FragColor;" \
		"uniform sampler3D volume;"\
		"uniform vec3 camPos;"\
		"uniform vec3 step_size;"\
		"const int MAX_SAMPLES = 300;"\
		"const vec3 texMin = vec3(0);"\
		"const vec3 texMax = vec3(1);"\
		"void main (void)" \
		"{" \
			"vec3 dataPos = vUV;"\
			"vec3 geomDir = normalize((vUV-vec3(0.5)) - camPos);"\
			"vec3 dirStep = geomDir * step_size;"\
			"bool stop = false;"\
			"for (int i = 0; i < MAX_SAMPLES; i++) {"\
				"dataPos = dataPos + dirStep;"\
				"stop = dot(sign(dataPos-texMin),sign(texMax-dataPos)) < 3.0;"\
				"if (stop) break;"\
					
				"float sample = texture(volume, dataPos).r;"\
				"float prev_alpha = sample - (sample * FragColor.a);"\
				"FragColor.rgb = prev_alpha * vec3(sample) + FragColor.rgb;"\
				"FragColor.a += prev_alpha;"\

				"if(FragColor.a > 0.99) break;"\

					//"FragColor = vec4(1,1,1,1);"\
		
			"}"\
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
	camUniform = glGetUniformLocation(gShaderProgramObject, "camPos");
	volumeSamplerUniform = glGetUniformLocation(gShaderProgramObject, "volume");
	step_sizeUniform = glGetUniformLocation(gShaderProgramObject, "step_size");


	//load volume data
	if (LoadVolume())
	{
		fprintf(gpFile, "Volume data loaded successfully");
	}
	else
	{
		fprintf(gpFile, "Volume data Cannot be loaded");
	}

	glClearColor(bg.r, bg.g, bg.b, bg.a);



	glGenVertexArrays(1, &cubeVao);
	glGenBuffers(1, &cubeVbo);
	glGenBuffers(1, &cubeIndicesID);

	glm::vec3 vertices[8] = { glm::vec3(-0.5f,-0.5f,-0.5f),
							glm::vec3(0.5f,-0.5f,-0.5f),
							glm::vec3(0.5f, 0.5f,-0.5f),
							glm::vec3(-0.5f, 0.5f,-0.5f),
							glm::vec3(-0.5f,-0.5f, 0.5f),
							glm::vec3(0.5f,-0.5f, 0.5f),
							glm::vec3(0.5f, 0.5f, 0.5f),
							glm::vec3(-0.5f, 0.5f, 0.5f) };

	//unit cube indices
	GLushort cubeIndices[36] = { 0,5,4,
							  5,0,1,
							  3,7,6,
							  3,6,2,
							  7,4,6,
							  6,4,5,
							  2,1,3,
							  3,1,0,
							  3,0,7,
							  7,0,4,
							  6,5,2,
							  2,5,1 }; 
	glBindVertexArray(cubeVao);
	glBindBuffer(GL_ARRAY_BUFFER, cubeVbo);
	//pass cube vertices to buffer object memory
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &(vertices[0].x), GL_STATIC_DRAW);

	//enable vertex attributre array for position
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

	//pass indices to element array  buffer
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cubeIndicesID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cubeIndices), &cubeIndices[0], GL_STATIC_DRAW);

	glBindVertexArray(0);
	///////////////////////////////////////////Load texture slicing shader //////////////////////////

	//gPerspectiveProjectionMatrix = mat4::identity();
	glEnable(GL_DEPTH_TEST);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	///////////////////////////////////////////Load texture slicing shader //////////////////////////

	//get the current view direction vector

	

	//gPerspectiveProjectionMatrix = mat4::identity();

	//resize(WIN_WIDTH, WIN_HEIGHT);

}

void display(void)
{

	//set up camera
	glm::mat4 Tr = glm::translate(glm::mat4(1.0f), glm::vec3(0.0, 0.0, dist));
	glm::mat4 Rx = glm::rotate(Tr, rX, glm::vec3(1.0f, 0.0f, 0.0f));
	glm::mat4 MV = glm::rotate(Rx, rY, glm::vec3(0.0f, 0.0f, 1.0f));

	//get the view direction 
	glm::vec3 camPos;
	camPos = glm::vec3(glm::inverse(MV)*glm::vec4(0, 0, 0, 1));

	//clear the color and depth buffers
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//get combined mvp matrix
	glm::mat4 MVP = P * MV;

	//Render the grid Object
	grid->Render(glm::value_ptr(MVP));

	
	//enable alpha blending
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glUseProgram(gShaderProgramObject);

	//bind bertex array object
	glBindVertexArray(cubeVao);
	//volume shader
		glUniform3fv(camUniform, 1, &(camPos.x));
		glUniformMatrix4fv(gMVPUniform, 1, GL_FALSE, glm::value_ptr(MVP));
		glUniform1i(volumeSamplerUniform, 0);
		glUniform3f(step_sizeUniform, 1.0f / XDIM, 1.0f / YDIM, 1.0f / ZDIM);

		//draw the triangles
			//glDrawArrays(GL_TRIANGLES, 0, sizeof(vTextureSlices) / sizeof(vTextureSlices[0]));
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, 0);
		//unbind the shader 
	//glBindVertexArray(0);

	glUseProgram(0);

	glDisable(GL_BLEND);
	

	SwapBuffers(ghdc);

}

void resize(int width, int height)
{
	//code
	if (height == 0)
		height = 1;
	glViewport(0, 0, (GLsizei)width, (GLsizei)height);

	P = glm::perspective(55.0f, (GLfloat)width / (GLfloat)height, 0.1f, 1000.0f);
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
	if (cubeVao)
	{
		glDeleteVertexArrays(1, &cubeVao);
		cubeVao = 0;
	}
	glDeleteTextures(1, &textureID);
	//destroy vbo
	if (cubeVbo)
	{
		glDeleteVertexArrays(1, &cubeVbo);
		cubeVbo = 0;
	}

	if (cubeIndicesID)
	{
		glDeleteVertexArrays(1, &cubeIndicesID);
		cubeIndicesID = 0;
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
//#include<Windows.h>
//#include<stdio.h>
//
//#include<gl\glew.h>
//#include<gl\GL.h>
//
//#include"vmath.h" // vermillion math library
//
//#pragma comment (lib,"glew32.lib")
//#pragma comment (lib,"opengl32.lib")
//
//#define WIN_WIDTH 800
//#define WIN_HEIGHT 600
//
//#define chessImageWidth 64
//#define chessImageHeight 64
//
//using namespace vmath;
//
//GLfloat boardVertices1[] =
//{
//	2.0f, 2.0f, 0.0f,
//	-2.0f, 2.0f, 0.0f,
//	-2.0f, -2.0f, 0.0f,
//	2.0f, -2.0f, 0.0f
//};
//;
//
//using namespace std;
//
//LRESULT CALLBACK WndProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam);
//
//enum
//{
//	SAM_ATTRIBUTE_VERTEX = 0,
//	SAM_ATTRIBUTE_COLOR,
//	SAM_ATTRIBUTE_NORMAL,
//	SAM_ATTRIBUTE_TEXTURE0,
//};
//
//FILE *gp_File = NULL;
//
//HWND g_hwnd = NULL;
//HDC  g_hdc = NULL;
//HGLRC g_hrc = NULL;
//
//DWORD dwStyle;
//WINDOWPLACEMENT wpPrev = { sizeof(wpPrev) };
//
//bool g_bActiveWindow = false;
//bool g_bEscapePressed = false;
//bool g_bFullscreen = false;
//
//GLuint g_VertexShaderObject;
//GLuint g_FragmentShaderObject;
//GLuint g_ShaderProgramObject;
//
//GLuint g_VertexArrayObject_Board;
//GLuint g_VertexBufferObject_Position;
//GLuint g_VertexBufferObject_Texture;
//GLuint g_Uniform_ModelViewProjection;
//
//
//vmath::mat4 g_PerspectiveProjectionMatrix;
//
//int g_iCurrentWidth, g_iCurrentHeight;
//
//GLubyte chessImage[chessImageWidth][chessImageHeight][4];
//GLuint g_Texture_Chess_Image;
//
//GLuint g_uniform_TextureSampler;
//GLuint g_uniform_Time;
//GLuint g_uniform_Resolution;
//
//
//int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdLine, int iCmdShow)
//{
//	void Initialize(void);
//	void Display(void);
//	void Update(void);
//	void UnInitialize(void);
//
//	WNDCLASSEX wndclass;
//	MSG msg;
//	HWND hwnd = NULL;
//	TCHAR szClassName[] = TEXT("SSM_OpenGLProgramablePipeline");
//	bool bDone = false;
//
//	wndclass.cbSize = sizeof(wndclass);
//	wndclass.cbClsExtra = 0;
//	wndclass.cbWndExtra = 0;
//	wndclass.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
//	wndclass.hInstance = hInstance;
//	wndclass.lpfnWndProc = WndProc;
//	wndclass.lpszMenuName = NULL;
//	wndclass.lpszClassName = szClassName;
//	wndclass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
//	wndclass.hIcon = LoadIcon(hInstance, IDI_APPLICATION);
//	wndclass.hIconSm = LoadIcon(hInstance, IDI_APPLICATION);
//	wndclass.hCursor = LoadCursor(hInstance, IDC_ARROW);
//
//	if (!RegisterClassEx(&wndclass))
//	{
//		MessageBox(NULL, TEXT("could not Register Class"), TEXT("Failure..!!"), MB_OK);
//		exit(EXIT_FAILURE);
//	}
//
//	hwnd = CreateWindowEx(WS_EX_APPWINDOW, szClassName, TEXT("SSM : Texture Checker Board"),
//		WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_VISIBLE,
//		200, 200, WIN_WIDTH, WIN_HEIGHT, (HWND)NULL, (HMENU)NULL,
//		hInstance, (LPVOID)NULL);
//
//	if (hwnd == NULL)
//	{
//		MessageBox(NULL, TEXT("could not Create Window"), TEXT("Failure..!!"), MB_OK);
//		exit(EXIT_FAILURE);
//	}
//
//	g_hwnd = hwnd;
//
//	ShowWindow(hwnd, iCmdShow);
//	SetForegroundWindow(hwnd);
//	SetFocus(hwnd);
//
//	if (fopen_s(&gp_File, "SAM_OGL_SunTrial_proglog.txt", "w+") != 0)
//	{
//		MessageBox(NULL, TEXT("could not Open File"), TEXT("Failure..!!"), MB_OK);
//		exit(EXIT_FAILURE);
//	}
//	else
//	{
//		fprintf_s(gp_File, "File opened Succesfully....!!!\n");
//	}
//	Initialize();
//
//	while (bDone == false)
//	{
//		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
//		{
//			if (msg.wParam == WM_QUIT)
//			{
//				bDone = true;
//			}

//			else
//			{
//				TranslateMessage(&msg);
//				DispatchMessage(&msg);
//			}
//
//		}
//		else
//		{
//			if (g_bActiveWindow == true)
//			{
//				if (g_bEscapePressed == true)
//				{
//					bDone = true;
//				}
//				Update();
//				Display();
//			}
//
//		}
//	}
//
//	UnInitialize();
//
//	return (int)msg.wParam;
//
//}
//
//
//LRESULT CALLBACK WndProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
//{
//
//	void Resize(int iWidth, int iHeight);
//	void ToggleFullScreen(void);
//	void UnInitialize(void);
//
//	static WORD xMouse = NULL;
//	static WORD yMouse = NULL;
//
//	switch (iMsg)
//	{
//	case WM_ACTIVATE:
//		if (HIWORD(wParam) == 0)
//			g_bActiveWindow = true;
//		else
//			g_bActiveWindow = false;
//		break;
//	case WM_KEYDOWN:
//		switch (LOWORD(wParam))
//		{
//		case VK_ESCAPE:
//			g_bEscapePressed = true;
//			break;
//		case 0x46:
//			if (g_bFullscreen == false)
//			{
//				ToggleFullScreen();
//				g_bFullscreen = true;
//			}
//			else
//			{
//				ToggleFullScreen();
//				g_bFullscreen = false;
//			}
//			break;
//		default:
//			break;
//		}
//		break;
//	case WM_SIZE:
//		g_iCurrentWidth = LOWORD(lParam);
//		g_iCurrentHeight = HIWORD(lParam);
//		Resize(LOWORD(lParam), HIWORD(lParam));
//		break;
//	case WM_CLOSE:
//		UnInitialize();
//		break;
//	case WM_DESTROY:
//		PostQuitMessage(EXIT_SUCCESS);
//		break;
//	default:
//		break;
//	}
//
//	return(DefWindowProc(hwnd, iMsg, wParam, lParam));
//}
//
//
//void Initialize(void)
//{
//	fprintf_s(gp_File, "EXEC : Inside Initialize - Start\n");
//	void Resize(int iWidth, int iHeight);
//	void UnInitialize(void);
//	//	void LoadGLTexture(void);
//
//
//	PIXELFORMATDESCRIPTOR pfd;
//	int iPixelFormatIndex;
//
//	SecureZeroMemory(&pfd, sizeof(pfd));
//
//	pfd.nSize = sizeof(pfd);
//	pfd.nVersion = 1;
//	pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_DOUBLEBUFFER | PFD_SUPPORT_OPENGL;
//	pfd.iPixelType = PFD_TYPE_RGBA;
//	pfd.cColorBits = 32;
//	pfd.cRedBits = 8;
//	pfd.cGreenBits = 8;
//	pfd.cBlueBits = 8;
//	pfd.cDepthBits = 32;
//
//	g_hdc = GetDC(g_hwnd);
//
//	iPixelFormatIndex = ChoosePixelFormat(g_hdc, &pfd);
//	if (iPixelFormatIndex == 0)
//	{
//		ReleaseDC(g_hwnd, g_hdc);
//		g_hdc = NULL;
//	}
//
//	if (SetPixelFormat(g_hdc, iPixelFormatIndex, &pfd) == false)
//	{
//		ReleaseDC(g_hwnd, g_hdc);
//		g_hdc = NULL;
//	}
//
//	g_hrc = wglCreateContext(g_hdc);
//	if (g_hrc == NULL)
//	{
//		ReleaseDC(g_hwnd, g_hdc);
//		g_hdc = NULL;
//	}
//
//	if (wglMakeCurrent(g_hdc, g_hrc) == false)
//	{
//		wglDeleteContext(g_hrc);
//		g_hrc = NULL;
//		ReleaseDC(g_hwnd, g_hdc);
//		g_hdc = NULL;
//	}
//
//	// Sam : Position For GLEW initialization Code.
//	GLenum glew_error = glewInit();
//	if (glew_error != GLEW_OK)
//	{
//		wglDeleteContext(g_hrc);
//		g_hrc = NULL;
//		ReleaseDC(g_hwnd, g_hdc);
//		g_hdc = NULL;
//	}
//
//	// Shader Information Start
//	fprintf_s(gp_File, "SHADER_INFO : OpenGl Version is : %s \n", glGetString(GL_VERSION));
//	fprintf_s(gp_File, "SHADER_INFO : GLSL Version is : %s \n", glGetString(GL_SHADING_LANGUAGE_VERSION));
//
//	// Shader Information End
//	// Sam :  All Shaders Code Start
//
//	//***** Vertex Shader *****
//	//Create Shader
//	g_VertexShaderObject = glCreateShader(GL_VERTEX_SHADER);
//
//	// give source code to shader
//	const GLchar *vertexShaderSourceCode = "#version 450 core"	\
//		"\n" \
//		"in vec4 vPosition;"	\
//	//////////////////////////////////////////////////////////////////////////////////////////// Here remove all releated to sampler 
//		
//		"uniform mat4 u_mvp_matrix;"	\
//		"void main (void)"	\
//		"{"	\
//		"gl_Position = u_mvp_matrix * vPosition;"	\
//		
//		"}";
//	glShaderSource(g_VertexShaderObject, 1, (const GLchar**)&vertexShaderSourceCode, NULL);
//
//	// Compile Source Code
//	glCompileShader(g_VertexShaderObject);
//	GLint iInfoLogLength = 0;
//	GLint iShaderCompileStatus = 0;
//	char *szInfoLog = NULL;
//	glGetShaderiv(g_VertexShaderObject, GL_COMPILE_STATUS, &iShaderCompileStatus);
//	if (iShaderCompileStatus == GL_FALSE)
//	{
//		glGetShaderiv(g_VertexShaderObject, GL_INFO_LOG_LENGTH, &iInfoLogLength);
//		if (iInfoLogLength > 0)
//		{
//			szInfoLog = (char*)malloc(iInfoLogLength);
//			if (szInfoLog != NULL)
//			{
//				GLsizei written;
//				glGetShaderInfoLog(g_VertexShaderObject, GL_INFO_LOG_LENGTH, &written, szInfoLog);
//				fprintf_s(gp_File, "Error : Vertex Shader Compilation Log : %s \n", szInfoLog);
//				free(szInfoLog);
//				UnInitialize();
//				exit(EXIT_FAILURE);
//			}
//
//		}
//
//	}
//
//	//***** Fragment Shader *****
//	g_FragmentShaderObject = glCreateShader(GL_FRAGMENT_SHADER);
//
//	const GLchar *fragmentShaderSourceCode =
//		"#version 450 core"	\
//		"\n"	\
//		"out vec4 FragColor;"	\
//		"uniform vec3 iResolution;" \
//		"uniform sampler2D u_texture0_sampler;" \
//		"uniform float time;" \
//		"mat2 makem2(in float theta){float c = cos(theta); float s = sin(theta);return mat2(c,-s,s,c);}"\
//		"float noise(in vec2 x){return texture(u_texture0_sampler,x*0.01).x;}" \
//
//		"float fbm(in vec2 p)" \
//		"{" \
//		"float z=2;" \
//		"float rz = 0;" \
//		"vec2 bp = p;" \
//		"for(float i=1;i< 6;i++)" \
//		"{" \
//		"rz += abs((noise(p)-0.5)*2.0)/z;"\
//		"z = z*2;" \
//		"p = p*2;" \
//		"}" \
//		"return rz;" \
//		"}" \
//
//		"float dualfbm(in vec2 p)" \
//		"{" \
//		"vec2 p2 = p*0.7;" \
//		"vec2 basis = vec2(fbm(p2-(time*0.1) * 1.6),fbm(p2+(time*0.1)*1.7));" \
//		"basis = (basis - 0.5) * 0.2;" \
//		"p+= basis;" \
//		"return fbm(p*makem2((time*0.1)*0.2));" \
//		"}"\
//
//		"float circ(vec2 p)" \
//		"{" \
//		"float r = length(p);" \
//		"r = log(sqrt(r));" \
//		"return sin(mod(r*0.9 , 4.2831853)+3.14)*0.1+0.2;"\
//		"}" \
//
//		"void main(void)" \
//		"{"\
//		"vec2 p = gl_FragCoord.xy / iResolution.xy-0.3;" \
//		"p.x *= iResolution.x / iResolution.y;" \
//		"p *= 2.0;" \
//		"float rz = dualfbm(p);" \
//		"p /= exp(mod((time*0.2)*10.0,3.14159));"\
//
//		"rz *= pow(abs((0.1-circ(p))),0.9);"\
//		"vec3 col = vec3(0.120,0.1,0.0)/rz;" \
//		"col = pow(abs(col),vec3(1.0));" \
//		"FragColor = vec4(col,1.0);" \
//		"}";
//	glShaderSource(g_FragmentShaderObject, 1, (const GLchar **)&fragmentShaderSourceCode, NULL);
//
//	// Compile Source Code
//	glCompileShader(g_FragmentShaderObject);
//	iInfoLogLength = 0;
//	iShaderCompileStatus = 0;
//	szInfoLog = NULL;
//	glGetShaderiv(g_FragmentShaderObject, GL_COMPILE_STATUS, &iShaderCompileStatus);
//	if (iShaderCompileStatus == GL_FALSE)
//	{
//		glGetShaderiv(g_FragmentShaderObject, GL_INFO_LOG_LENGTH, &iInfoLogLength);
//		if (iInfoLogLength>0)
//		{
//			szInfoLog = (char*)malloc(iInfoLogLength);
//			if (szInfoLog != NULL)
//			{
//				GLsizei written;
//				glGetShaderInfoLog(g_FragmentShaderObject, GL_INFO_LOG_LENGTH, &written, szInfoLog);
//				fprintf_s(gp_File, "Error : Fragment Shader Compilation Log : %s \n", szInfoLog);
//				free(szInfoLog);
//				UnInitialize();
//				exit(EXIT_FAILURE);
//			}
//		}
//	}
//
//	//***** Shader Program *****
//	// Create
//	g_ShaderProgramObject = glCreateProgram();
//	// Attach Vertex Shader
//	glAttachShader(g_ShaderProgramObject, g_VertexShaderObject);
//	// Attach Fragment Shader
//	glAttachShader(g_ShaderProgramObject, g_FragmentShaderObject);
//	// pre-link Program object with Vertex Sahder position attribute
//	glBindAttribLocation(g_ShaderProgramObject, SAM_ATTRIBUTE_VERTEX, "vPosition");
//	//glBindAttribLocation(g_ShaderProgramObject, SAM_ATTRIBUTE_TEXTURE0, "vTexture0_Coord");
//	// link Shader 
//	glLinkProgram(g_ShaderProgramObject);
//
//	GLint iShaderProgramLinkStatus = 0;
//	glGetProgramiv(g_ShaderProgramObject, GL_LINK_STATUS, &iShaderProgramLinkStatus);
//	szInfoLog = NULL;
//	iInfoLogLength = 0;
//	if (iShaderProgramLinkStatus == GL_FALSE)
//	{
//		glGetProgramiv(g_ShaderProgramObject, GL_INFO_LOG_LENGTH, &iInfoLogLength);
//		if (iInfoLogLength > 0)
//		{
//			szInfoLog = (char*)malloc(iInfoLogLength);
//			if (szInfoLog != NULL)
//			{
//				GLsizei written;
//				glGetShaderInfoLog(g_ShaderProgramObject, GL_INFO_LOG_LENGTH, &written, szInfoLog);
//				fprintf_s(gp_File, "Error : Shader Program Link Log : %s \n", szInfoLog);
//				free(szInfoLog);
//				UnInitialize();
//				exit(EXIT_FAILURE);
//			}
//		}
//	}
//
//	g_Uniform_ModelViewProjection = glGetUniformLocation(g_ShaderProgramObject, "u_mvp_matrix");
//	g_uniform_TextureSampler = glGetUniformLocation(g_ShaderProgramObject, "u_texture0_sampler");
//
//	g_uniform_Resolution = glGetUniformLocation(g_ShaderProgramObject, "iResolution");
//	g_uniform_Time = glGetUniformLocation(g_ShaderProgramObject, "time");
//
//
//	// **** Verttices, Colors, Shader Attribs, Vbo, Vao Initializations ****
//
//	/*const GLfloat boardTexCoords[] =
//	{
//		0.0f, 0.0f,
//		1.0f, 0.0f,
//		1.0f, 1.0f,
//		0.0f, 1.0f
//	};*/
//	// vao creation and binding
//	glGenBuffers(1, &g_VertexArrayObject_Board);
//	glBindVertexArray(g_VertexArrayObject_Board);
//
//	//vbo Position creation and binding
//	glGenBuffers(1, &g_VertexBufferObject_Position);
//	glBindBuffer(GL_ARRAY_BUFFER, g_VertexBufferObject_Position);
//	glBufferData(GL_ARRAY_BUFFER, sizeof(boardVertices1), NULL, GL_DYNAMIC_DRAW);
//
//	glVertexAttribPointer(SAM_ATTRIBUTE_VERTEX, 3, GL_FLOAT, GL_FALSE, 0, NULL);
//	glEnableVertexAttribArray(SAM_ATTRIBUTE_VERTEX);
//
//	glBindBuffer(GL_ARRAY_BUFFER, 0);
//
//
//	////////////////////////////////////////////////////////////////////////////////////////////// No need of Texcoords as we are creating direct tecture in GPU memory and renedering it on one quad vertices
//	
//	////vbo Texture creation and binding
//	//glGenBuffers(1, &g_VertexBufferObject_Texture);
//	//glBindBuffer(GL_ARRAY_BUFFER, g_VertexBufferObject_Texture);
//
//	//glBufferData(GL_ARRAY_BUFFER, sizeof(boardTexCoords), boardTexCoords, GL_STATIC_DRAW);
//
//	//glVertexAttribPointer(SAM_ATTRIBUTE_TEXTURE0, 2, GL_FLOAT, GL_FALSE, 0, NULL);
//	//glEnableVertexAttribArray(SAM_ATTRIBUTE_TEXTURE0);
//
//	//glBindBuffer(GL_ARRAY_BUFFER, 0);
//	glBindVertexArray(0);
//	// Sam :  All Shaders Code End
//
//	//LoadGLTexture();
//
//	glShadeModel(GL_SMOOTH);
//
//	//glEnable(GL_TEXTURE_2D);
//	glClearDepth(1.0f);
//	glEnable(GL_DEPTH_TEST);
//	glDepthFunc(GL_LEQUAL);
//	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
//	glEnable(GL_CULL_FACE);
//
//	glClearColor(0.4f, 0.7f, 0.5f, 0.0f);
//
//	g_PerspectiveProjectionMatrix = vmath::mat4::identity();
//
//	Resize(WIN_WIDTH, WIN_HEIGHT);
//
//	fprintf_s(gp_File, "EXEC : Inside Initialize - End \n");
//}
//
//
///*void LoadGLTexture(void)
//{
//int i, j, c;
//
//for (i = 0; i < chessImageHeight; i++)
//{
//for (j = 0; j < chessImageWidth; j++)
//{
//c = (((i & 0x8) == 0) ^ ((j & 0x8) == 0)) * 255;
//chessImage[i][j][0] = (GLubyte)c;
//chessImage[i][j][1] = (GLubyte)c;
//chessImage[i][j][2] = (GLubyte)c;
//chessImage[i][j][3] = (GLubyte)155;
//}
//}
//
//
//
//glGenTextures(1, &g_Texture_Chess_Image);
//
//glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
//glBindTexture(GL_TEXTURE_2D, g_Texture_Chess_Image);
//
//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
//
//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, chessImageWidth, chessImageHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, chessImage);
//
//glGenerateMipmap(GL_TEXTURE_2D);
//
//glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
//
//
//}
//*/
//
//
//void ToggleFullScreen(void)
//{
//	fprintf_s(gp_File, "EXEC : Inside Toggle Fullscreen - Start\n");
//	MONITORINFO mi = { sizeof(mi) };
//
//	if (g_bFullscreen == false)
//	{
//		dwStyle = GetWindowLong(g_hwnd, GWL_STYLE);
//		if (dwStyle & WS_OVERLAPPEDWINDOW)
//		{
//			if (GetWindowPlacement(g_hwnd, &wpPrev) && GetMonitorInfo(MonitorFromWindow(g_hwnd, MONITORINFOF_PRIMARY), &mi))
//			{
//				SetWindowLong(g_hwnd, GWL_STYLE, dwStyle & ~WS_OVERLAPPEDWINDOW);
//				SetWindowPos(g_hwnd, HWND_TOP, mi.rcMonitor.left, mi.rcMonitor.top, (mi.rcMonitor.right - mi.rcMonitor.left), (mi.rcMonitor.bottom - mi.rcMonitor.top), SWP_NOZORDER | SWP_FRAMECHANGED);
//			}
//			ShowCursor(FALSE);
//		}
//	}
//	else
//	{
//		SetWindowLong(g_hwnd, GWL_STYLE, dwStyle | WS_OVERLAPPEDWINDOW);
//		SetWindowPlacement(g_hwnd, &wpPrev);
//		SetWindowPos(g_hwnd, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_FRAMECHANGED | SWP_NOZORDER | SWP_NOOWNERZORDER);
//		ShowCursor(TRUE);
//	}
//	fprintf_s(gp_File, "EXEC : Inside Toggle Fullscreen - End \n");
//}
//
//void Resize(int iWidth, int iHeight)
//{
//	//fprintf_s(gp_File, "EXEC : Inside Resize - Start \n " );
//	if (iHeight == 0)
//		iHeight = 1;
//
//	glViewport(0, 0, (GLsizei)iWidth, (GLsizei)iHeight);
//
//	//glFrustum(left,right,bottom,top,Near-ZAxis,Far-ZAxis)
//	g_PerspectiveProjectionMatrix = vmath::perspective(45.0f, (float)iWidth / (float)iHeight, 0.1f, 100.0f);
//
//	//fprintf_s(gp_File, "EXEC : Inside Resize - End \n");
//}
//
//void Display(void)
//{
//	//int iResolutionInfo[3] = { g_iCurrentWidth,g_iCurrentWidth,(int)0 };
//	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//
//	glUseProgram(g_ShaderProgramObject);
//
//	vmath::mat4 modelViewMatrix = vmath::mat4::identity();
//	//modelViewMatrix = vmath::translate(0.0f, 0.0f, -6.0f);
//	mat4 viewmatrix = mat4::identity();
//	vmath::mat4 modelViewProjectionMatrix = g_PerspectiveProjectionMatrix * modelViewMatrix;
//
////	glUniformMatrix4fv(g_Uniform_ModelViewProjection, 1, GL_FALSE, modelViewProjectionMatrix);
//
//	//glUniform3iv(g_uniform_Resolution, 1, iResolutionInfo);
//	///////////////////////////////////////////////////////////////////////////////////// Resolution changed it to just like WIN_WIDTH & WIN_HEIGHT
//	glUniform3f(g_uniform_Resolution, 1920, 1080.0, 0.0f);
//	glUniform1f(g_uniform_Time, 1000.0f);
//
//	//glActiveTexture(GL_TEXTURE0);
//	//glBindTexture(GL_TEXTURE_2D, g_Texture_Chess_Image);
//
////	glUniform1i(g_uniform_TextureSampler, 0);
//
//	glBindVertexArray(g_VertexArrayObject_Board);
//
//	/////////////////////////////////////////////////////////////////////////////////////  here we need to multiply it only by  setting view matrix and send this as MVP in glsl code
//	glUniformMatrix4fv(g_Uniform_ModelViewProjection, 1, GL_FALSE, viewmatrix);
//
//	glBindBuffer(GL_ARRAY_BUFFER, g_VertexBufferObject_Position);
//
//	glBufferData(GL_ARRAY_BUFFER, sizeof(boardVertices1), boardVertices1, GL_DYNAMIC_DRAW);
//
//	glBindBuffer(GL_ARRAY_BUFFER, 0);
//
//	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
//
//
//
//	glBindVertexArray(0);
//
//	glUseProgram(0);
//
//	SwapBuffers(g_hdc);
//}
//
//void Update(void)
//{
//
//}
//
//void UnInitialize(void)
//{
//	fprintf_s(gp_File, "EXEC : Inside UnInitilize - Start\n");
//	if (g_bFullscreen == true)
//	{
//		SetWindowLong(g_hwnd, GWL_STYLE, dwStyle | WS_OVERLAPPEDWINDOW);
//		SetWindowPlacement(g_hwnd, &wpPrev);
//		SetWindowPos(g_hwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_FRAMECHANGED | SWP_NOZORDER | SWP_NOOWNERZORDER);
//		ShowCursor(TRUE);
//	}
//
//	if (g_VertexArrayObject_Board)
//	{
//		glDeleteVertexArrays(1, &g_VertexArrayObject_Board);
//		g_VertexArrayObject_Board = 0;
//	}
//
//	if (g_VertexBufferObject_Position)
//	{
//		glDeleteBuffers(1, &g_VertexBufferObject_Position);
//		g_VertexBufferObject_Position = 0;
//	}
//
//	if (g_VertexBufferObject_Texture)
//	{
//		glDeleteBuffers(1, &g_VertexBufferObject_Texture);
//		g_VertexBufferObject_Texture = 0;
//	}
//
//	glDetachShader(g_ShaderProgramObject, g_VertexShaderObject);
//	glDetachShader(g_ShaderProgramObject, g_FragmentShaderObject);
//
//	if (g_VertexShaderObject)
//	{
//		glDeleteShader(g_VertexShaderObject);
//		g_VertexShaderObject = 0;
//	}
//
//	if (g_FragmentShaderObject)
//	{
//		glDeleteShader(g_FragmentShaderObject);
//		g_FragmentShaderObject = 0;
//	}
//
//	if (g_ShaderProgramObject)
//	{
//		glDeleteProgram(g_ShaderProgramObject);
//		g_ShaderProgramObject = 0;
//	}
//
//	glUseProgram(0);
//
//	wglMakeCurrent(NULL, NULL);
//
//	if (g_hrc)
//	{
//		wglDeleteContext(g_hrc);
//		g_hrc = NULL;
//	}
//
//	if (g_hdc)
//	{
//		ReleaseDC(g_hwnd, g_hdc);
//		g_hdc = NULL;
//	}
//
//	if (gp_File)
//	{
//		fprintf_s(gp_File, "EXEC : Inside UnInitilize - End \n");
//		fprintf_s(gp_File, "Log file is closed....!!");
//		fclose(gp_File);
//		gp_File = NULL;
//	}
//
//
//
//}
//
//
//
