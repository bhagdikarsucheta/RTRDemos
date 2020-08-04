#include<windows.h>
#include<stdio.h>
#include<GL/glew.h>
#include<GL/GL.h>

#include"vmath.h"
#include"01_TEXTURES_PYRAMID_CUBE.h"


using namespace vmath;

#define WIN_WIDTH 800
#define WIN_HEIGHT 600

#pragma comment(lib,"glew32.lib")
#pragma comment(lib,"opengl32.lib")


static int day = 0;
static int year = 0;
static int moon = 0;

//Prototype of WndProc() declared gloabally
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

//Global variable declarations
HWND ghwnd = NULL;
HDC ghdc = NULL;
HGLRC ghrc = NULL;
int MAXSIZE = 16;
int stack[16];
int top = -1;
mat4 insertModelMatrix[124];

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

GLuint gNumElements;
GLuint gNumVertices;

GLuint iResolutionUniform;
RECT rc;
GLuint sphere_Vao;
GLuint sphere_bo_position;
GLuint sphere_vbo_normal;
GLuint sphere_vbo_texture;
GLuint sphere_vbo_index;

GLuint gVao_Cube;
GLuint gVbo_CubePosition;

// Sphere Functions Start
GLushort *gsphereElements;
GLfloat  *gsphereVerts;
GLfloat  *gsphereNorms;
GLfloat  *gsphereTexCoords;


GLuint giNumElements = 0;
GLuint giMaxElements = 0;
GLuint giNumVertices = 0;
GLuint gTexture_sampler_uniform;


GLuint gVBOSphere = 0;
GLuint gVBOSpherePosition = 0;
GLuint gVBOSphereNormal = 0;
GLuint gVBOSphereTexture = 0;
GLuint gVBOSphereElement = 0;

void drawSphere();
void makeSphere(GLfloat fRadius, GLint iSlices, GLint iStacks);
void DeallocateSphere();
void allocateSphere(GLint iNumIndices);
GLuint getIndexCountSphere();
GLuint getVertexCountSphere();

void addTriangles(GLfloat **single_vertex, GLfloat **single_normal, GLfloat **single_texture);
void normalizeVector(GLfloat *gfVector);
bool IsFoundIdentical(GLfloat val1, GLfloat val2, GLfloat diff);

void prepareToDraw();
void releaseSphereMemory(GLfloat **Block);
void cleanupMeshDataSphere();

float sphere_vertices[1146];
float sphere_normals[1146];
float sphere_textures[764];
unsigned short sphere_elements[2280];

GLuint gVao_Sphere;
GLuint gVbo_Sphere_positions;
GLuint gVbo_Sphere_normal;
GLuint gVbo_Sphere_element;

GLuint model_matrix_uniform, view_matrix_uniform, projection_matrix_uniform;

GLuint L_KeyPressed_uniform;

GLuint La_uniform;
GLuint Ld_uniform;
GLuint Ls_uniform;
GLuint light_position_uniform;

GLuint Ka_uniform;
GLuint Kd_uniform;
GLuint Ks_uniform;
GLuint material_shininess_uniform;

mat4 gPerspectiveProjectionMatrix;


GLfloat lightAmbient[] = { 0.0f,0.0f,0.0f,1.0f };
GLfloat lightDiffuse[] = { 1.0f,1.0f,1.0f,1.0f };
GLfloat lightSpecular[] = { 1.0f,1.0f,1.0f,1.0f };
GLfloat lightPosition[] = { 100.0f,100.0f,100.0f,1.0f };

GLfloat material_ambient[] = { 0.0f,0.0f,0.0f,1.0f };
GLfloat material_diffuse[] = { 1.0f,1.0f,1.0f,1.0f };
GLfloat material_specular[] = { 1.0f,1.0f,1.0f,1.0f };
GLfloat material_shininess = 50.0f;
bool gbLight;
GLfloat gAngle = 0.0f;
GLfloat gAngle2 = 0.0f;
GLuint gTexture_Stone;
GLuint gTexture_Earth;
GLuint gTexture_Moon;

GLfloat animate = 1.0f;
GLuint animateUniform;


FILE *gpFile = NULL;

//main()
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdLine, int nCmdShow)
{
	void display();
	void initialize();
	void uninitialize();
	void spin();
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
		case 'D':
			day = (day + 6) % 360;
			break;
		case 'd':
			day = (day - 3) % 360;
			break;
		case 'Y':
			year = (year + 3) % 360;
			break;
		case 'y':
			year = (year - 3) % 360;
			break;

		case 'M':
			moon = (moon + 3) % 360;
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
		"#version 430 core"	\
		"\n" \
		"in vec4 vPosition;" \
		"in vec3 vNormal;" \
		"in vec2 vTexture0_Coord;" \
		"in vec4 vColor;"\
		"out vec4 out_Color;"\
		"uniform mat4 u_model_matrix;" \
		"uniform mat4 u_view_matrix;" \
		"uniform mat4 u_projection_matrix;" \
		"uniform vec4 u_light_position;" \
		"uniform int u_lighting_enabled;" \

		"out vec2 out_texture0_coord;" \

		"void main(void)" \
		"{" \
		"if(u_lighting_enabled == 1)" \
		"{" \

		"}" \
		"gl_Position =  vPosition;" \
		"out_texture0_coord = vTexture0_Coord;" \
		"out_Color = vColor;"\
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
		"uniform mat4 u_view_matrix;" \
		"vec4 textureFragColor;"\
		"uniform float animate;"\
		"uniform sampler2D u_texture0_sampler;" \

		"uniform vec3 iResolution;" \


		"float distance_from_sphere(in vec3 p, in vec3 c, float r)"\
		"{"\
		"return length(p - c) - r;"\

		/*	"vec3 d = abs(p) - dim;"\
			"return min(max(d.x,max(d.y,d.z)),0.0) + length(max(d,0.0));"	\*/
		"}"\

		"float map_to_the_world(in vec3 p)"\
		"{"\
		"float displacement = sin(5.0 * p.x) * sin(5.0 * p.y) * sin(5.0 * p.z)*0.25 ;"\
		"float sphere_0 = distance_from_sphere(p, vec3(0.0), 1.0);"\

		"return sphere_0+displacement;"\
		"}"\




		"vec3 calculate_normal(in vec3 p)"\
		"{"\
		"	const vec3 small_step = vec3(0.001, 0.0, 0.0);"\

		"	float gradient_x = map_to_the_world(p + small_step.xyy) - map_to_the_world(p - small_step.xyy);"\
		"	float gradient_y = map_to_the_world(p + small_step.yxy) - map_to_the_world(p - small_step.yxy);"\
		"	float gradient_z = map_to_the_world(p + small_step.yyx) - map_to_the_world(p - small_step.yyx);"\

		"	vec3 normal = vec3(gradient_x, gradient_y, gradient_z);"\

		"return normalize(normal);"\
		"}"\




		"vec3 ray_march(in vec3 ro, in vec3 rd)"\
		"{"\
		"float total_distance_traveled = 0.0;"\
		"const int NUMBER_OF_STEPS = 32;"\
		"const float MINIMUM_HIT_DISTANCE = 0.001;"\
		"const float MAXIMUM_TRACE_DISTANCE = 1000.0;"\

		"for (int i = 0; i < NUMBER_OF_STEPS; ++i)"\
		"{"\

		"vec3 current_position = ro + total_distance_traveled * rd;"\

		"float distance_to_closest = map_to_the_world(current_position);"\

		"if (distance_to_closest < MINIMUM_HIT_DISTANCE)"\
		"{"\
		"vec3 normal = calculate_normal(current_position);"\
		"vec3 light_position = vec3(2.0, -5.0, 3.0);"\
		"vec3 direction_to_light = normalize(current_position - light_position);"\

		"float diffuse_intensity = max(0.0, dot(normal, direction_to_light));"\

		"return vec3(1.0, 0.0, 0.0) * diffuse_intensity;"\
		"}"\

		"if (total_distance_traveled > MAXIMUM_TRACE_DISTANCE)"\
		"{"\
		"break;"\
		"}"\
		"total_distance_traveled += distance_to_closest;"\
		"}"\
		"return vec3(0.0);"\
		"}"\


		"void main(void)"\
		"{"\
		"	textureFragColor =  texture(u_texture0_sampler, out_texture0_coord);"
		"	vec2 uv = vec2(gl_FragCoord.x/1920.0, gl_FragCoord.y/1500.0)*2.0-1.0;" \
		"	vec3 camera_position = vec3(0.0,0.0,-5.0);"\
		"	vec3 ro = camera_position;"\
		"	vec3 rd = vec3(uv,1.0);"\
		"	vec3 shaded_color = ray_march(ro,rd);"\
		"	FragColor = vec4(shaded_color,1.0);"\
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

	glBindAttribLocation(gShaderProgramObject, VDG_ATTRIBUTE_NORMAL, "vNormal");

	glBindAttribLocation(gShaderProgramObject, VDG_ATTRIBUTE_TEXTURE0, "vTexture0_Coord");

	glBindAttribLocation(gShaderProgramObject, VDG_ATTRIBUTE_TEXTURE0, "vColor");

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

	// get uniform locations

	model_matrix_uniform = glGetUniformLocation(gShaderProgramObject, "u_model_matrix");
	view_matrix_uniform = glGetUniformLocation(gShaderProgramObject, "u_view_matrix");
	projection_matrix_uniform = glGetUniformLocation(gShaderProgramObject, "u_projection_matrix");
	iResolutionUniform = glGetUniformLocation(gShaderProgramObject, "iResolution");
	animateUniform = glGetUniformLocation(gShaderProgramObject, "animate");
	//L/I Key is pressed or not
	L_KeyPressed_uniform = glGetUniformLocation(gShaderProgramObject, "u_lighting_enabled");


	//ambient color intensity of LIGHT
	La_uniform = glGetUniformLocation(gShaderProgramObject, "u_La");
	//diffuse color intensity of LIGHT
	Ld_uniform = glGetUniformLocation(gShaderProgramObject, "u_Ld");
	//specular color intensity of LIGHT
	Ls_uniform = glGetUniformLocation(gShaderProgramObject, "u_Ls");
	//position of light
	light_position_uniform = glGetUniformLocation(gShaderProgramObject, "u_light_position");

	//ambient reflective color intensity of MATERIAL
	Ka_uniform = glGetUniformLocation(gShaderProgramObject, "u_Ka");
	//diffuse reflective color intensity of MATERIAL
	Kd_uniform = glGetUniformLocation(gShaderProgramObject, "u_Kd");

	//specular reflective color intensity of MATERIAL
	Ks_uniform = glGetUniformLocation(gShaderProgramObject, "u_Ks");

	//shininess of material (value is conventionally between 1 to 200)
	material_shininess_uniform = glGetUniformLocation(gShaderProgramObject, "u_material_shininess");

	//vertices colors shader attribs 
	gTexture_sampler_uniform = glGetUniformLocation(gShaderProgramObject, "u_texture0_sampler");


	fprintf(gpFile, "After all vertices and normals..\n");

	//makeSphere(0.5, 40, 40);


	const GLfloat squareVertices[] =
	{
		////front
		1.0f, 1.0f, 1.0f,
		-1.0f, 1.0f, 1.0f,
		-1.0f, -1.0f, 1.0f,
		1.0f, -1.0f, 1.0f,


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

	glVertexAttrib3f(VDG_ATTRIBUTE_COLOR, 0.0f, 0.0f, 1.0f);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0);


	glShadeModel(GL_SMOOTH);
	//DEPTH
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	//glEnable(GL_CULL_FACE);
	glClearColor(0.0f, 0.0f, 0.0, 0.0f);
	//LoadGLTextures(&gTexture_Stone, MAKEINTRESOURCE(IDBITMAP_STONE));
	//LoadGLTextures(&gTexture_Earth, MAKEINTRESOURCE(IDBITMAP_KUNDALI));
	//LoadGLTextures(&gTexture_Moon, MAKEINTRESOURCE(IDBITMAP_MOON));

	gPerspectiveProjectionMatrix = mat4::identity();
	gbLight = false;
	resize(WIN_WIDTH, WIN_HEIGHT);
	fprintf(gpFile, "Exitting Initialize..\n");
	GetClientRect(ghwnd, &rc);

	ToggleFullScreen();
}



int LoadGLTextures(GLuint *texture, TCHAR imageResourceId[])
{
	fprintf(gpFile, "In LoadGLTextures..\n");

	//variable declarations
	HBITMAP hBitmap;
	BITMAP bmp;
	int iStatus = FALSE;
	//code
	glGenTextures(1, texture);	//1 image
	hBitmap = (HBITMAP)LoadImage(GetModuleHandle(NULL), imageResourceId, IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION);
	if (hBitmap)
	{
		iStatus = TRUE;
		GetObject(hBitmap, sizeof(bmp), &bmp);
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		glBindTexture(GL_TEXTURE_2D, *texture);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, bmp.bmWidth, bmp.bmHeight, 0, GL_BGR, GL_UNSIGNED_BYTE, bmp.bmBits);

		//create mipmaps
		glGenerateMipmap(GL_TEXTURE_2D);
		DeleteObject(hBitmap);
	}
	fprintf(gpFile, "Exiting LoadGLTextures..\n");

	return(iStatus);
}

void allocateSphere(GLint iNumIndices)
{
	void cleanupMeshDataSphere();

	cleanupMeshDataSphere();

	giMaxElements = iNumIndices;
	giNumElements = 0;
	giNumVertices = 0;

	int iNumberOfIndices = iNumIndices / 3;

	gsphereElements = (GLushort*)malloc(iNumberOfIndices * 3 * sizeof(gsphereElements));
	gsphereVerts = (GLfloat*)malloc(iNumberOfIndices * 3 * sizeof(gsphereVerts));
	gsphereNorms = (GLfloat*)malloc(iNumberOfIndices * 3 * sizeof(gsphereNorms));
	gsphereTexCoords = (GLfloat*)malloc(iNumberOfIndices * 2 * sizeof(gsphereTexCoords));


}

void addTriangles(GLfloat **single_vertex, GLfloat **single_normal, GLfloat **single_texture)
{
	void normalizeVector(GLfloat *gfVector);
	bool IsFoundIdentical(GLfloat val1, GLfloat val2, GLfloat diff);

	const float diff = 0.00001f;
	int i, j;

	normalizeVector(single_normal[0]);
	normalizeVector(single_normal[1]);
	normalizeVector(single_normal[2]);

	for (i = 0; i < 3; i++)
	{
		for (j = 0; j < giNumVertices; j++)
		{
			if (IsFoundIdentical(gsphereVerts[j * 3], single_vertex[i][0], diff) &&
				IsFoundIdentical(gsphereVerts[(j * 3) + 1], single_vertex[i][1], diff) &&
				IsFoundIdentical(gsphereVerts[(j * 3) + 2], single_vertex[i][2], diff) &&

				IsFoundIdentical(gsphereNorms[j * 3], single_normal[i][0], diff) &&
				IsFoundIdentical(gsphereNorms[(j * 3) + 1], single_normal[i][1], diff) &&
				IsFoundIdentical(gsphereNorms[(j * 3) + 2], single_normal[i][2], diff) &&

				IsFoundIdentical(gsphereTexCoords[j * 2], single_texture[i][0], diff) &&
				IsFoundIdentical(gsphereTexCoords[(j * 2) + 1], single_texture[i][1], diff))
			{
				gsphereElements[giNumElements] = (short)j;
				giNumElements++;
				break;
			}
		}


		if ((j == giNumVertices) && (giNumVertices < giMaxElements) && (giNumElements < giMaxElements))
		{
			gsphereVerts[giNumVertices * 3] = single_vertex[i][0];
			gsphereVerts[(giNumVertices * 3) + 1] = single_vertex[i][1];
			gsphereVerts[(giNumVertices * 3) + 2] = single_vertex[i][2];

			gsphereNorms[giNumVertices * 3] = single_normal[i][0];
			gsphereNorms[(giNumVertices * 3) + 1] = single_normal[i][1];
			gsphereNorms[(giNumVertices * 3) + 2] = single_normal[i][2];

			gsphereTexCoords[giNumVertices * 2] = single_texture[i][0];
			gsphereTexCoords[(giNumVertices * 2) + 1] = single_texture[i][1];

			gsphereElements[giNumElements] = (short)giNumVertices;
			giNumElements++;
			giNumVertices++;
		}
	}
}

void makeSphere(GLfloat fRadius, GLint iSlices, GLint iStacks)
{
	void allocateSphere(GLint iNumIndices);
	void prepareToDraw();
	void releaseSphereMemory(GLfloat **Block);

	GLfloat drho = (GLfloat)M_PI / (GLfloat)iStacks;
	GLfloat dtheta = 2.0f * (GLfloat)M_PI / (GLfloat)iSlices;
	GLfloat ds = 1.0f / (GLfloat)(iSlices);
	GLfloat dt = 1.0f / (GLfloat)(iStacks);
	GLfloat t = 1.0f;
	GLfloat s = 0.0f;
	int i = 0, j = 0;

	allocateSphere(iSlices * iStacks * 6);

	for (int i = 0; i < iStacks; i++)
	{
		GLfloat rho = (GLfloat)(i * drho);
		GLfloat srho = (sinf(rho));
		GLfloat crho = (cosf(rho));
		GLfloat srhodrho = (sinf(rho + drho));
		GLfloat crhodrho = (cosf(rho + drho));

		s = 0.0;

		GLfloat **vertex = (GLfloat **)malloc(sizeof(GLfloat *) * 4);
		for (int i = 0; i < 4; i++) // vertex[4][3]
			vertex[i] = (GLfloat *)malloc(sizeof(GLfloat) * 3);

		GLfloat **normal = (GLfloat **)malloc(sizeof(GLfloat *) * 4);
		for (int i = 0; i < 4; i++)// normal[4][3]
			normal[i] = (GLfloat *)malloc(sizeof(GLfloat) * 3);

		GLfloat **texture = (GLfloat **)malloc(sizeof(GLfloat *) * 4);
		for (int i = 0; i < 4; i++)// texture[4][2]
			texture[i] = (GLfloat *)malloc(sizeof(GLfloat) * 2);


		for (j = 0; j < iSlices; j++)
		{
			GLfloat theta = (j == iSlices) ? 0.0 : j * dtheta;
			GLfloat stheta = (GLfloat)(-sin(theta));
			GLfloat ctheta = (GLfloat)(cos(theta));

			GLfloat x = stheta * srho;
			GLfloat y = ctheta * srho;
			GLfloat z = crho;

			texture[0][0] = s;
			texture[0][1] = t;
			normal[0][0] = x;
			normal[0][1] = y;
			normal[0][2] = z;
			vertex[0][0] = x * fRadius;
			vertex[0][1] = y * fRadius;
			vertex[0][2] = z * fRadius;

			x = stheta * srhodrho;
			y = ctheta * srhodrho;
			z = crhodrho;

			texture[1][0] = s;
			texture[1][1] = t - dt;
			normal[1][0] = x;
			normal[1][1] = y;
			normal[1][2] = z;
			vertex[1][0] = x * fRadius;
			vertex[1][1] = y * fRadius;
			vertex[1][2] = z * fRadius;

			theta = ((j + 1) == iSlices) ? 0.0 : (j + 1) * dtheta;
			stheta = (float)(-sin(theta));
			ctheta = (float)(cos(theta));

			x = stheta * srho;
			y = ctheta * srho;
			z = crho;

			s += ds;
			texture[2][0] = s;
			texture[2][1] = t;
			normal[2][0] = x;
			normal[2][1] = y;
			normal[2][2] = z;
			vertex[2][0] = x * fRadius;
			vertex[2][1] = y * fRadius;
			vertex[2][2] = z * fRadius;

			x = stheta * srhodrho;
			y = ctheta * srhodrho;
			z = crhodrho;

			texture[3][0] = s;
			texture[3][1] = t - dt;
			normal[3][0] = x;
			normal[3][1] = y;
			normal[3][2] = z;
			vertex[3][0] = x * fRadius;
			vertex[3][1] = y * fRadius;
			vertex[3][2] = z * fRadius;

			addTriangles(vertex, normal, texture);


			vertex[0][0] = vertex[1][0];
			vertex[0][1] = vertex[1][1];
			vertex[0][2] = vertex[1][2];
			normal[0][0] = normal[1][0];
			normal[0][1] = normal[1][1];
			normal[0][2] = normal[1][2];
			texture[0][0] = texture[1][0];
			texture[0][1] = texture[1][1];

			vertex[1][0] = vertex[3][0];
			vertex[1][1] = vertex[3][1];
			vertex[1][2] = vertex[3][2];
			normal[1][0] = normal[3][0];
			normal[1][1] = normal[3][1];
			normal[1][2] = normal[3][2];
			texture[1][0] = texture[3][0];
			texture[1][1] = texture[3][1];


			addTriangles(vertex, normal, texture);
		}
		t -= dt;

	}

	prepareToDraw();
}

void prepareToDraw()
{
	void cleanupMeshDataSphere();

	glGenVertexArrays(1, &gVBOSphere);
	glBindVertexArray(gVBOSphere);


	glGenBuffers(1, &gVBOSpherePosition);
	glBindBuffer(GL_ARRAY_BUFFER, gVBOSpherePosition);
	glBufferData(GL_ARRAY_BUFFER, (giMaxElements * 3 * sizeof(gsphereVerts) / 3), gsphereVerts, GL_STATIC_DRAW);
	glVertexAttribPointer(VDG_ATTRIBUTE_VERTEX, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(VDG_ATTRIBUTE_VERTEX);
	glBindBuffer(GL_ARRAY_BUFFER, 0);


	glGenBuffers(1, &gVBOSphereNormal);
	glBindBuffer(GL_ARRAY_BUFFER, gVBOSphereNormal);
	glBufferData(GL_ARRAY_BUFFER, (giMaxElements * 3 * sizeof(gsphereNorms) / 3), gsphereNorms, GL_STATIC_DRAW);
	glVertexAttribPointer(VDG_ATTRIBUTE_NORMAL, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(VDG_ATTRIBUTE_NORMAL);
	glBindBuffer(GL_ARRAY_BUFFER, 0);


	glGenBuffers(1, &gVBOSphereTexture);
	glBindBuffer(GL_ARRAY_BUFFER, gVBOSphereTexture);
	glBufferData(GL_ARRAY_BUFFER, (giMaxElements * 2 * sizeof(gsphereTexCoords) / 3), gsphereTexCoords, GL_STATIC_DRAW);
	glVertexAttribPointer(VDG_ATTRIBUTE_TEXTURE0, 2, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(VDG_ATTRIBUTE_TEXTURE0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenBuffers(1, &gVBOSphereElement);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gVBOSphereElement);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, (giMaxElements * 3 * sizeof(gsphereElements) / 3), gsphereElements, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	glBindVertexArray(0);

	cleanupMeshDataSphere();
}

void normalizeVector(GLfloat *gfVector)
{

	GLfloat fSquaredVectorLength = (gfVector[0] * gfVector[0]) + (gfVector[1] * gfVector[1]) + (gfVector[2] * gfVector[2]);
	GLfloat fSsquareRootOfSquaredVectorLength = sqrtf(fSquaredVectorLength);

	gfVector[0] = gfVector[0] * 1.0f / fSsquareRootOfSquaredVectorLength;
	gfVector[1] = gfVector[1] * 1.0f / fSsquareRootOfSquaredVectorLength;
	gfVector[2] = gfVector[2] * 1.0f / fSsquareRootOfSquaredVectorLength;
}

bool IsFoundIdentical(GLfloat val1, GLfloat val2, GLfloat diff)
{
	if (fabs(val1 - val2) < diff)
		return(true);
	else
		return(false);
}


void drawSphere()
{
	glBindVertexArray(gVBOSphere);

	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gVBOSphereElement);
	glDrawElements(GL_TRIANGLES, giNumElements, GL_UNSIGNED_SHORT, 0);

	glBindVertexArray(0);
}

GLuint getIndexCountSphere()
{
	return(giNumElements);
}

GLuint getVertexCountSphere()
{
	return GLuint(giNumElements);
}

void releaseSphereMemory(GLfloat **Block)
{
	for (int idx = 0; idx < 4; idx++)
	{
		free(Block[idx]);
		Block[idx] = NULL;
	}
	free(Block);
	Block = NULL;

}


void cleanupMeshDataSphere()
{
	if (gsphereElements != NULL)
	{
		free(gsphereElements);
		gsphereElements = NULL;
	}

	if (gsphereVerts != NULL)
	{
		free(gsphereVerts);
		gsphereVerts = NULL;
	}

	if (gsphereNorms != NULL)
	{
		free(gsphereNorms);
		gsphereNorms = NULL;
	}

	if (gsphereTexCoords != NULL)
	{
		free(gsphereTexCoords);
		gsphereTexCoords = NULL;
	}
}



void display()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	GetClientRect(ghwnd, &rc);

	//start using OpenGL program object
	glUseProgram(gShaderProgramObject);

	//GLfloat lightDiffuse[] = { 1.0f,1.0f,0.0f,1.0f };

	//if (gbLight == true)
	//{

	//	//set u_lighting_enabled uniform
	//	glUniform1i(L_KeyPressed_uniform, 1);

	//	////setting lights properties
	//	glUniform3fv(La_uniform, 1, lightAmbient);
	//	glUniform3fv(Ld_uniform, 1, lightDiffuse);
	//	glUniform3fv(Ls_uniform, 1, lightSpecular);
	//	glUniform4fv(light_position_uniform, 1, lightPosition);

	//	//setting material's properties
	//	glUniform3fv(Ka_uniform, 1, material_ambient);
	//	glUniform3fv(Kd_uniform, 1, material_diffuse);
	//	glUniform3fv(Ks_uniform, 1, material_specular);
	//	glUniform1f(material_shininess_uniform, material_shininess);
	//}
	//else
	//{
	//	//set 'u_lighting_enabled' uniform
	//	glUniform1i(L_KeyPressed_uniform, 0);
	//}


	////OpenGL Drawing
	//mat4 modelMatrix = mat4::identity();
	//mat4 viewMatrix = mat4::identity();
	//mat4 rotationmatrix = mat4::identity();
	//mat4 rotationMatrixSphere = mat4::identity();

	//modelMatrix = translate(0.0f, 0.0f, -3.0f);
//	rotationMatrixSphere = vmath::rotate((GLfloat)90.0f, 1.0f, 0.0f, 0.0f);
	//rotationmatrix = vmath::rotate((GLfloat)gAngle, 0.0f, 0.0f, 1.0f);
//	modelMatrix = modelMatrix * rotationMatrixSphere*rotationmatrix;

//	glUniformMatrix4fv(model_matrix_uniform, 1, GL_FALSE, modelMatrix);
////	glUniformMatrix4fv(view_matrix_uniform, 1, GL_FALSE, viewMatrix);
//	glUniform3f(iResolutionUniform, 1920, 1080, 0.0);
//
//	//glUniformMatrix4fv(projection_matrix_uniform, 1, GL_FALSE, gPerspectiveProjectionMatrix);
//
//	glActiveTexture(GL_TEXTURE0); // 0th texture ( corresponds to VDG_ATTRIBUTE_TEXTURE0 )
//	glBindTexture(GL_TEXTURE_2D, gTexture_Stone);
//	glUniform1i(gTexture_sampler_uniform, 0); // 0th sampler enable ( as we have only 1 texture sampler in fragment shader )
//	
//	drawSphere();
//	

	glBindVertexArray(gVao_Cube);
	glUniform1f(animateUniform, animate);

	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

	glBindVertexArray(0);

	glUseProgram(0);
	SwapBuffers(ghdc);

	animate += 0.01f;

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
	if (gVao_Sphere)
	{
		glDeleteVertexArrays(1, &gVao_Sphere);
		gVao_Sphere = 0;
	}

	if (gVbo_Sphere_positions)
	{
		glDeleteBuffers(1, &gVbo_Sphere_positions);
		gVbo_Sphere_positions = 0;
	}

	if (gVbo_Sphere_normal)
	{
		glDeleteBuffers(1, &gVbo_Sphere_normal);
		gVbo_Sphere_normal = 0;
	}

	if (gVbo_Sphere_element)
	{
		glDeleteBuffers(1, &gVbo_Sphere_element);
		gVbo_Sphere_element = 0;
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


























