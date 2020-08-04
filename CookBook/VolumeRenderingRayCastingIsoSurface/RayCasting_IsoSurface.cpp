
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

//volume data files 
const std::string volume_file = "Engine256.raw";

//dimensions of volume data
 int XDIM = 256;
 int YDIM = 256;
 int ZDIM = 256;

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
		TEXT("OpenGL Programmable PipeLine Using Native Windowing: Volume Rendering RayCastingisourface"),
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

		case 0x52:
		
			if (rX <= 100)
			{
				rX = rX + 0.1f;
			}
			if (rY <= 50)
			{
				rY = rY +0.1f;
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
		"const float DELTA =0.01;"\
		"const float isoValue = 40/255.0;"\

		"vec3 Bisection(vec3 left,vec3 right,float iso)"\
		"{"\
			"for(int i=0;i<4;i++)"\
			"{"\
				"vec3 midpoint = (right + left) * 0.5;"\
				"float cM = texture(volume,midpoint).x;"\
				"if(cM < iso){"\
					"left = midpoint;}"\
				"else{"\
					"right = midpoint;}"\
			"}"\
			"return vec3(right + left)*0.5;"\
		"}"\

	

		"vec3 GetGradient(vec3 uvw)"\
		"{"\
			"vec3 s1,s2;"\

			"s1.x = texture(volume,uvw-vec3(DELTA,0.0,0.0)).x;"\
			"s2.x = texture(volume,uvw+vec3(DELTA,0.0,0.0)).x;"\

			"s1.y = texture(volume,uvw-vec3(0.0,DELTA,0.0)).x;"\
			"s2.y = texture(volume,uvw+vec3(0.0,DELTA,0.0)).x;"\
			
			"s1.z = texture(volume,uvw-vec3(0.0,0.0,DELTA)).x;"\
			"s2.z = texture(volume,uvw+vec3(0.0,0.0,DELTA)).x;"\
			
			"return normalize((s1-s2)/2.0);"\

		"}"\

		"vec4 PhongLighting(vec3 L,vec3 N,vec3 V,float specPower,vec3 diffuseColor)"\
		"{"\
			"float diffuse = max(dot(L,N),0.0);"\
			"vec3 halfVec = normalize(L+V);"\
			"float specular = pow(max(0.00001,dot(halfVec,N)),specPower);"\
			"return vec4((diffuse*diffuseColor + specular),1.0);"\
		"}"\

		"void main (void)" \
		"{" \
			"vec3 dataPos = vUV;"\
			"vec3 geomDir = normalize((vUV-vec3(0.5)) - camPos);"\
			"vec3 dirStep = geomDir * step_size;"\
			"bool stop = false;"\
			"for (int i = 0; i < MAX_SAMPLES; i++) {"\
				"dataPos = dataPos + dirStep;"\
				"stop = dot(sign(dataPos-texMin),sign(texMax-dataPos)) < 3.0;"\
				"if (stop)"\
					"break;"\

				"float sample = texture(volume, dataPos).r;"\
				"float sample2 = texture(volume,dataPos+dirStep).r;"\
				"if((sample-isoValue) < 0 && (sample2-isoValue) >= 0.0){"\

					
						"vec3 xN = dataPos;"\
						"vec3 xF = dataPos + dirStep;"\
						"vec3 tc = Bisection(xN,xF,isoValue);"\

						"vec3 N = GetGradient(tc);"\
						"vec3 V = -geomDir;"\

						"vec3 L = V;"\

						"FragColor = PhongLighting(L,N,V,250,vec3(0.5));"\
							"break;"\
			
				"}"\
	
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

	glEnable(GL_DEPTH_TEST);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	///////////////////////////////////////////Load texture slicing shader //////////////////////////

	//get the current view direction vector



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

		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, 0);
		
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







