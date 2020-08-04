#include<windows.h>
#include<stdio.h>
#include<gl/glew.h>
#include<gl/GL.h>

#include"vmath.h"

using namespace vmath;

#define WIN_WIDTH 1920
#define WIN_HEIGHT 1080

#pragma comment(lib,"glew32.lib")
#pragma comment(lib,"opengl32.lib")
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

///////////////// Light Global

GLuint gVertexShaderObject;
GLuint gFragmentShaderObject;
GLuint gShaderProgramObject;
GLuint kaUniform;
GLuint kdUniform;
GLuint ksUniform;
GLuint LightPosUniform;
GLuint gLightPosUniform;
GLuint MaterialShininessUniform;

GLfloat Ka[] = { 1.0,1.0,0.65 };
GLfloat Kd[] = { 1.0,1.0,0.65 };
GLfloat Ks[] = { 1.0,1.0,0.65 };

GLuint gNumElements;
GLuint gNumVertices;
GLfloat gWidth;
GLfloat gHeight;

////////////////////// God rays global

GLuint gVertexShaderObjectGodRays;
GLuint gFragmentShaderObjectGodRays;
GLuint gShaderProgramObjectGodRays;

GLuint gTexture_sampler_uniformGodRays;
GLuint gLightPositionGodRays;
//quad
GLuint gVao_Square;
GLuint gVboPosSquare;
GLuint gVboTexSquare;

////////////////// Final Shader Global

GLuint gVertexShaderObjectTexture;
GLuint gFragmentShaderObjectTexture;
GLuint gShaderProgramObjectTexture;
GLuint gTexture_sampler_uniformTexture;
GLuint gTexture_sampler1_uniformTexture;

GLuint gVao_Smiley;
GLuint gVbo_Smiley_texture;
GLuint gVbo_Smiley_position;
GLuint gMVPTexture;

GLuint GodRaysFBO;
GLuint colorBuffer;
GLuint FinalcolorBuffer;
GLuint FinalFBO;
GLuint rboDepth;


GLuint finalFBO;
GLuint finalcolorBuffer;
GLuint godraysFBO;

//cube
GLuint gVao_Cube;
GLuint gVbo_CubePosition;
GLuint gVbo_Cube_Normal;

GLuint sphere_Vao;
GLuint sphere_bo_position;
GLuint sphere_vbo_normal;
GLuint sphere_vbo_texture;
GLuint sphere_vbo_index;


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
GLuint gmodel_matrix_uniform, gview_matrix_uniform, gprojection_matrix_uniform;

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
GLfloat material_shininess = 128.0f;
bool gbLight;
GLfloat gAngle = 0.0f;
GLfloat gAngle2 = 0.0f;
GLuint gTexture_Stone;
GLuint gTexture_Earth;
GLuint gTexture_Moon;


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

	/////////////////////////////////////////////////////////// 1. Light Shader //////////////////////////////////



	////// scene render shader //////////////////////////////////////////////////////////
	gVertexShaderObject = glCreateShader(GL_VERTEX_SHADER);

	// vertex shader source code 
	const GLchar *vertexShaderSourceCode = (GLchar *)
		/*"#version 450 core"	\
		"\n" \
		"in vec4 vPosition;" \
		"in vec3 vNormal;" \
		"in vec2 vTexture0_Coord;" \

		"uniform mat4 u_model_matrix;" \
		"uniform mat4 u_view_matrix;" \
		"uniform mat4 u_projection_matrix;" \
		"uniform int u_lighting_enabled;" \

		"out vec3 out_world;" \
		"out vec3 out_eye;" \
		"out vec3 out_normal;" \

		"void main(void)" \
		"{" \
		"vec4 worldCoordinates = u_model_matrix*vPosition;" \
		"vec4 eyeCoordinates = u_view_matrix * worldCoordinates;" \
		"vec4 clipSpace = u_projection_matrix*eyeCoordinates;" \

		"out_world = worldCoordinates.xyz;"\
		"out_eye = eyeCoordinates.xyz;"\
		"out_normal = mat3(u_view_matrix*u_model_matrix)*vNormal;"\

		"gl_Position = clipSpace;" \

		"}";*/



		"#version 430 core"	\
		"\n" \
		"in vec4 vPosition;" \
		"in vec3 vNormal;" \
		"in vec2 vTexture0_Coord;" \

		"uniform mat4 u_model_matrix;" \
		"uniform mat4 u_view_matrix;" \
		"uniform mat4 u_projection_matrix;" \
		"uniform vec4 u_light_position;" \
		"uniform int u_lighting_enabled;" \

		"out vec3 transformed_normals;" \
		"out vec3 light_direction;" \
		"out vec3 viewer_vector;" \
		"out vec2 out_texture0_coord;" \

		"void main(void)" \
		"{" \

		"vec4 eye_coordinates = u_view_matrix*u_model_matrix*vPosition;" \
		"transformed_normals = mat3(u_view_matrix * u_model_matrix) * vNormal;" \
		"light_direction = vec3(u_light_position) - eye_coordinates.xyz;" \

		"viewer_vector = -eye_coordinates.xyz;" \

		"gl_Position = u_projection_matrix * u_view_matrix * u_model_matrix * vPosition;" \
		//	"out_texture0_coord = vTexture0_Coord;" \

		"}";


	// attach source code to vertex shader
	glShaderSource(gVertexShaderObject, 1, (const GLchar **)&vertexShaderSourceCode, NULL);

	// compile vertex shader source code
	glCompileShader(gVertexShaderObject);

	// compilation errors 
	int iShaderCompiledStatus = 0;
	int iInfoLength = 0;
	char *szInfoLog = NULL;

	glGetShaderiv(gVertexShaderObject, GL_COMPILE_STATUS, &iShaderCompiledStatus);
	if (iShaderCompiledStatus == GL_FALSE)
	{
		glGetShaderiv(gVertexShaderObject, GL_INFO_LOG_LENGTH, &iInfoLength);
		if (iInfoLength > 0)
		{
			szInfoLog = (GLchar *)malloc(iInfoLength);
			if (szInfoLog != NULL)
			{
				GLsizei written;
				glGetShaderInfoLog(gVertexShaderObject, GL_INFO_LOG_LENGTH, &written, szInfoLog);

				fprintf(gpFile, "Vertex Shader Compiler Info Log: %s", szInfoLog);
				free(szInfoLog);
				uninitialize();
				DestroyWindow(ghwnd);
			}
		}
	}

	// create fragment shader object
	gFragmentShaderObject = glCreateShader(GL_FRAGMENT_SHADER);

	// fragment shader source code
	const GLchar *fragmentShaderSourceCode = (GLchar *)
		/*"#version 450 core" \
		"\n" \

		"in vec3 out_world;" \
		"in vec3 out_eye;" \
		"in vec3 out_normal;" \


		"uniform vec4 u_light_position;" \

		"uniform vec3 u_Ka;" \
		"uniform vec3 u_Kd;" \
		"uniform vec3 u_Ks;" \
		"uniform float u_material_shininess;" \
		"out vec4 FragColor;" \

		"void main(void)" \
		"{" \

			"vec3 Normals = out_normal;"\
			"vec3 Light = normalize(u_light_position.xyz-out_world);"\

			"vec3 reflect_vector = reflect(-Light,Normals);"\

			"float Dot_Light_Normals = dot(Normals, Light);"\
			"float diffuse = max(Dot_Light_Normals,0.0);"\


			"float specular = max(pow(dot(normalize(-out_eye),reflect_vector),u_material_shininess),0.0);"\

			"FragColor = vec4(u_Ka +  (u_Kd * diffuse * vec3(1.0, 1.0, 0.65 ) + u_Ks * specular * vec3(1.0, 1.0, 0.65)), 1.0);" \



		"}";	*/

		"#version 430 core" \
		"\n" \
		"in vec3 transformed_normals;" \
		"in vec3 light_direction;" \
		"in vec3 viewer_vector;" \
		"in vec2 out_texture0_coord;" \
		"out vec4 FragColor;" \

		"uniform vec3 u_La;" \
		"uniform vec3 u_Ld;" \
		"uniform vec3 u_Ls;" \
		"uniform vec3 u_Ka;" \
		"uniform vec3 u_Kd;" \
		"uniform vec3 u_Ks;" \
		"uniform float u_material_shininess;" \
		"uniform int u_lighting_enabled;" \
		"uniform sampler2D u_texture0_sampler;" \

		"void main(void)" \
		"{" \
		"vec3 phong_ads_color;" \

		"vec3 normalized_transformed_normals=normalize(transformed_normals);" \
		"vec3 normalized_light_direction=normalize(light_direction);" \
		"vec3 normalized_viewer_vector=normalize(viewer_vector);" \
		"vec3 ambient = u_La * u_Ka;" \
		"float tn_dot_ld = max(dot(normalized_transformed_normals, normalized_light_direction),0.0);" \
		"vec3 diffuse = u_Ld * u_Kd * tn_dot_ld;" \
		"vec3 reflection_vector = reflect(-normalized_light_direction,normalized_transformed_normals);" \
		"vec3 specular = u_Ls * u_Ks * pow(max(dot(reflection_vector,normalized_viewer_vector),0.0),u_material_shininess);" \
		"phong_ads_color = ambient + diffuse + specular;" \

		//"textureFragColor =  texture(u_texture0_sampler, out_texture0_coord);"

		"FragColor =  vec4(phong_ads_color,1.0f);" \


		"}";

	// attach source code to fragment shader
	glShaderSource(gFragmentShaderObject, 1, (const GLchar **)&fragmentShaderSourceCode, NULL);

	// compile fragment shader source code
	glCompileShader(gFragmentShaderObject);

	// compile errors
	iShaderCompiledStatus = 0;
	iInfoLength = 0;
	szInfoLog = NULL;

	glGetShaderiv(gFragmentShaderObject, GL_COMPILE_STATUS, &iShaderCompiledStatus);
	if (iShaderCompiledStatus == GL_FALSE)
	{
		glGetShaderiv(gFragmentShaderObject, GL_INFO_LOG_LENGTH, &iInfoLength);
		if (iInfoLength > 0)
		{
			szInfoLog = (GLchar *)malloc(iInfoLength);
			if (szInfoLog != NULL)
			{
				GLsizei written;
				glGetShaderInfoLog(gFragmentShaderObject, GL_INFO_LOG_LENGTH, &written, szInfoLog);

				fprintf(gpFile, "Fragment Shader Compiler Info Log: %s", szInfoLog);
				free(szInfoLog);
				uninitialize();
				DestroyWindow(ghwnd);
			}
		}
	}

	// create shader program object 
	gShaderProgramObject = glCreateProgram();

	// attach vertex shader to shader program
	glAttachShader(gShaderProgramObject, gVertexShaderObject);

	// attach fragment shader to shader program
	glAttachShader(gShaderProgramObject, gFragmentShaderObject);

	// pre-linking binding to vertex attribute
	glBindAttribLocation(gShaderProgramObject, VDG_ATTRIBUTE_VERTEX, "vPosition");
	glBindAttribLocation(gShaderProgramObject, VDG_ATTRIBUTE_NORMAL, "vNormal");

	// link the shader program
	glLinkProgram(gShaderProgramObject);

	// linking errors
	int iProgramLinkStatus = 0;

	glGetProgramiv(gShaderProgramObject, GL_LINK_STATUS, &iProgramLinkStatus);
	if (iProgramLinkStatus == GL_FALSE)
	{
		glGetProgramiv(gShaderProgramObject, GL_INFO_LOG_LENGTH, &iInfoLength);
		if (iInfoLength > 0)
		{
			szInfoLog = (GLchar *)malloc(iInfoLength);
			if (szInfoLog != NULL)
			{
				GLsizei written;
				glGetProgramInfoLog(gShaderProgramObject, GL_INFO_LOG_LENGTH, &written, szInfoLog);

				fprintf(gpFile, ("Shader Program Linking Info Log: %s"), szInfoLog);
				free(szInfoLog);
				uninitialize();
				DestroyWindow(ghwnd);
			}
		}
	}

	// post-linking retrieving uniform locations
	model_matrix_uniform = glGetUniformLocation(gShaderProgramObject, "u_model_matrix");
	view_matrix_uniform = glGetUniformLocation(gShaderProgramObject, "u_view_matrix");
	projection_matrix_uniform = glGetUniformLocation(gShaderProgramObject, "u_projection_matrix");
	//ambient color intensity of LIGHT
	La_uniform = glGetUniformLocation(gShaderProgramObject, "u_La");
	//diffuse color intensity of LIGHT
	Ld_uniform = glGetUniformLocation(gShaderProgramObject, "u_Ld");
	//specular color intensity of LIGHT
	Ls_uniform = glGetUniformLocation(gShaderProgramObject, "u_Ls");
	//position of light
	LightPosUniform = glGetUniformLocation(gShaderProgramObject, "u_light_position");

	//ambient reflective color intensity of MATERIAL
	kaUniform = glGetUniformLocation(gShaderProgramObject, "u_Ka");
	//diffuse reflective color intensity of MATERIAL
	kdUniform = glGetUniformLocation(gShaderProgramObject, "u_Kd");

	//specular reflective color intensity of MATERIAL
	ksUniform = glGetUniformLocation(gShaderProgramObject, "u_Ks");

	//shininess of material (value is conventionally between 1 to 200)
	MaterialShininessUniform = glGetUniformLocation(gShaderProgramObject, "u_material_shininess");

	/////////////////////////////////////////////////////////////////////////////////////////////////////////



	///////////////////////////////////////////////// CUBE


	const GLfloat cubeVertices[] =
	{
		1.0f, 1.0f, -1.0f,
		-1.0f, 1.0f, -1.0f,
		-1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,

		//bottom
		1.0f, -1.0f, 1.0f,
		-1.0f, -1.0f, 1.0f,
		-1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,

		//front
		1.0f, 1.0f, 1.0f,
		-1.0f, 1.0f, 1.0f,
		-1.0f, -1.0f, 1.0f,
		1.0f, -1.0f, 1.0f,

		//back 
		1.0f, 1.0f, -1.0f,
		-1.0f, 1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,

		//right
		1.0f, 1.0f, -1.0f,
		1.0f, 1.0f, 1.0f,
		1.0f, -1.0f, 1.0f,
		1.0f, -1.0f, -1.0f,

		//left
		-1.0f, 1.0f, 1.0f,
		-1.0f, 1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f, 1.0f,
		/////////////////////////////////////////////////

		//0.5f, 1.0f, -1.0f,
		//-0.5f, 1.0f, -1.0f,
		//-0.5f, 1.0f, 1.0f,
		//0.5f, 1.0f, 1.0f,

		////bottom
		//1.5f, -1.0f, 1.0f,
		//-1.5f, -1.0f, 1.0f,
		//-1.5f, -1.0f, -1.0f,
		//1.5f, -1.0f, -1.0f,

		////front
		//0.5f, 1.0f, 1.0f,
		//-0.5f, 1.0f, 1.0f,
		//-1.5f, -1.0f, 1.0f,
		//1.5f, -1.0f, 1.0f,

		////back 
		//0.5f, 1.0f, -1.0f,
		//-0.5f, 1.0f, -1.0f,
		//-1.5f, -1.0f, -1.0f,
		//1.5f, -1.0f, -1.0f,

		////right
		//0.5f, 1.0f, -1.0f,
		//0.5f, 1.0f, 1.0f,
		//1.5f, -1.0f, 1.0f,
		//1.5f, -1.0f, -1.0f,

		////left
		//-0.5f, 1.0f, 1.0f,
		//-0.5f, 1.0f, -1.0f,
		//-1.5f, -1.0f, -1.0f,
		//-1.5f, -1.0f, 1.0f,


		////////////////////////////////////////

		// 1.5f, 2.4f, -1.0f,
		//-1.5f, 2.4f, -1.0f,
		//-1.5f, 2.4f, 1.0f,
		// 1.5f, 2.4f, 1.0f,

		////bottom
		// 0.8f, -1.0f, 1.0f,
		//-0.8f, -1.0f, 1.0f,
		//-0.8f, -1.0f, -1.0f,
		// 0.8f, -1.0f, -1.0f,

		////front
		//1.5f,  2.4f, 1.0f,
	 //   -1.5f, 2.4f, 1.0f,
		//-0.8f, -1.0f, 1.0f,
		//0.8f,  -1.0f, 1.0f,

		////back 
		//1.5f,  2.4f, -1.0f,
		//-1.5f, 2.4f, -1.0f,
		//-0.8f,-1.0f, -1.0f,
		//0.8f, -1.0f, -1.0f,

		////right
		//1.5f, 2.4f, -1.0f,
		//1.5f, 2.4f, 1.0f,
		//0.8f, -1.0f, 1.0f,
		//0.8f, -1.0f, -1.0f,

		////left
		//-1.5f, 2.4f, 1.0f,
		//-1.5f, 2.4f, -1.0f,
		//-0.8f, -1.0f, -1.0f,
		//-0.8f, -1.0f, 1.0f,

	};

	const GLfloat cubeNormals[] =
	{
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,

		0.0f,-1.0f, 0.0f,
		0.0f,-1.0f, 0.0f,
		0.0f,-1.0f, 0.0f,
		0.0f,-1.0f, 0.0f,

		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,


		0.0f, 0.0f, -1.0f,
		0.0f, 0.0f, -1.0f,
		0.0f, 0.0f, -1.0f,
		0.0f, 0.0f, -1.0f,

		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,

		-1.0f, 0.0f, 0.0f,
		-1.0f, 0.0f, 0.0f,
		-1.0f, 0.0f, 0.0f,
		-1.0f, 0.0f, 0.0f,

	};
	fprintf(gpFile, "After all vertices and normals..\n");

	glGenVertexArrays(1, &gVao_Cube);
	glBindVertexArray(gVao_Cube);

	// position vbo
	glGenBuffers(1, &gVbo_CubePosition);
	glBindBuffer(GL_ARRAY_BUFFER, gVbo_CubePosition);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);

	glVertexAttribPointer(VDG_ATTRIBUTE_VERTEX, 3, GL_FLOAT, GL_FALSE, 0, NULL);

	glEnableVertexAttribArray(VDG_ATTRIBUTE_VERTEX);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// normal vbo
	glGenBuffers(1, &gVbo_Cube_Normal);
	glBindBuffer(GL_ARRAY_BUFFER, gVbo_Cube_Normal);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cubeNormals), cubeNormals, GL_STATIC_DRAW);

	glVertexAttribPointer(VDG_ATTRIBUTE_NORMAL, 3, GL_FLOAT, GL_FALSE, 0, NULL);

	glEnableVertexAttribArray(VDG_ATTRIBUTE_NORMAL);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0);
	fprintf(gpFile, "After all vertices and normals..\n");

	/////////////////////////////////////GodRays Pass//////////////////////////////////////////////////


	// create vertex shader object
	gVertexShaderObjectGodRays = glCreateShader(GL_VERTEX_SHADER);

	//provide source code to shader
	const GLchar * vertexShaderShaderCodeGodRays =
		"#version 450 core"	\
		"\n" \
		"in vec4 vPosition;" \
		"in vec2 vTexture0_Coord;" \
		"out vec2 out_TexCoords;" \

		"void main(void)" \
		"{" \
		"out_TexCoords = vTexture0_Coord;"\
		"gl_Position = vPosition;" \

		"}";

	// attach source code to vertex shader
	glShaderSource(gVertexShaderObjectGodRays, 1, (const GLchar **)&vertexShaderShaderCodeGodRays, NULL);

	// compile vertex shader source code
	glCompileShader(gVertexShaderObjectGodRays);

	// compilation errors 
	iShaderCompiledStatus = 0;
	iInfoLength = 0;
	szInfoLog = NULL;

	glGetShaderiv(gVertexShaderObjectGodRays, GL_COMPILE_STATUS, &iShaderCompiledStatus);
	if (iShaderCompiledStatus == GL_FALSE)
	{
		glGetShaderiv(gVertexShaderObjectGodRays, GL_INFO_LOG_LENGTH, &iInfoLength);
		if (iInfoLength > 0)
		{
			szInfoLog = (GLchar *)malloc(iInfoLength);
			if (szInfoLog != NULL)
			{
				GLsizei written;
				glGetShaderInfoLog(gVertexShaderObjectGodRays, GL_INFO_LOG_LENGTH, &written, szInfoLog);

				fprintf(gpFile, "God rays Vertex Shader Compiler Info Log: %s", szInfoLog);
				free(szInfoLog);
				uninitialize();
				DestroyWindow(ghwnd);
			}
		}
	}

	// create fragment shader object
	gFragmentShaderObjectGodRays = glCreateShader(GL_FRAGMENT_SHADER);

	//provide source code to shader
	const GLchar *fragmentShaderSourceCodeGodRays =
		"#version 450 core" \
		"\n" \
		"in vec2 out_TexCoords;" \
		"uniform sampler2D u_texture_sampler2D;" \
		"uniform vec4  u_light_position;" \
		"out vec4 FragColor;" \
		"void main(void)" \
		"{" \
		"vec2 deltaTexCoords = vec2(out_TexCoords-u_light_position.xy);"\
		"vec3 finalColor = vec3(texture(u_texture_sampler2D,out_TexCoords));"\
		"vec2 texCoo = out_TexCoords.st;"\
		"float exposure = 1.5f;"\
		"float decay = 1.0;"\
		"float density = 1.0;"\
		"float weight = 0.005;"\
		"float illuminationDecay = 1.0;"\
		"deltaTexCoords *= 1.0/100.0*density;"\

		"for(int i=0;i<100;i++)"\
		"{"\
			"texCoo -= deltaTexCoords;"\
			"vec4 sampled = texture2D(u_texture_sampler2D,texCoo);"\
			"sampled *= illuminationDecay*weight;"\
			"finalColor += vec3(sampled);"\
			"illuminationDecay *= decay;"\
		"}"\
		"finalColor *= exposure;"\
		"FragColor = vec4(finalColor,1.0);"\
		"}";

	// attach source code to fragment shader
	glShaderSource(gFragmentShaderObjectGodRays, 1, (const GLchar**)&fragmentShaderSourceCodeGodRays, NULL);

	//compile shader
	glCompileShader(gFragmentShaderObjectGodRays);
	glGetShaderiv(gFragmentShaderObjectGodRays, GL_COMPILE_STATUS, &iShaderCompiledStatus);
	if (iShaderCompiledStatus == GL_FALSE)
	{
		glGetShaderiv(gFragmentShaderObjectGodRays, GL_INFO_LOG_LENGTH, &iShaderCompiledStatus);
		if (iInfoLength > 0)
		{
			szInfoLog = (char *)malloc(iInfoLength);
			if (szInfoLog != NULL)
			{
				GLsizei written;
				glGetShaderInfoLog(gFragmentShaderObjectGodRays, iInfoLength, &written, szInfoLog);
				fprintf(gpFile, "God Rays Fragment shader compilation Log %s\n", szInfoLog);
				free(szInfoLog);
				uninitialize();
				exit(0);
			}
		}
	}
	// create shader program object 
	gShaderProgramObjectGodRays = glCreateProgram();

	// attach vertex shader to shader program
	glAttachShader(gShaderProgramObjectGodRays, gVertexShaderObjectGodRays);

	// attach fragment shader to shader program
	glAttachShader(gShaderProgramObjectGodRays, gFragmentShaderObjectGodRays);

	// pre-linking binding to vertex attribute
	glBindAttribLocation(gShaderProgramObjectGodRays, VDG_ATTRIBUTE_VERTEX, "vPosition");
	glBindAttribLocation(gShaderProgramObjectGodRays, VDG_ATTRIBUTE_TEXTURE0, "vTexture0_Coord");

	// link the shader program
	glLinkProgram(gShaderProgramObjectGodRays);

	// linking errors
	iProgramLinkStatus = 0;
	iInfoLength = 0;
	szInfoLog = NULL;

	glGetProgramiv(gShaderProgramObjectGodRays, GL_LINK_STATUS, &iProgramLinkStatus);
	if (iProgramLinkStatus == GL_FALSE)
	{
		glGetProgramiv(gShaderProgramObjectGodRays, GL_INFO_LOG_LENGTH, &iInfoLength);
		if (iInfoLength > 0)
		{
			szInfoLog = (GLchar *)malloc(iInfoLength);
			if (szInfoLog != NULL)
			{
				GLsizei written;
				glGetProgramInfoLog(gShaderProgramObjectGodRays, GL_INFO_LOG_LENGTH, &written, szInfoLog);

				fprintf(gpFile, ("God Rays Shader Program Linking Info Log: %s"), szInfoLog);
				free(szInfoLog);
				uninitialize();
				DestroyWindow(ghwnd);
			}
		}
	}

	// post-linking retrieving uniform locations
	gLightPositionGodRays = glGetUniformLocation(gShaderProgramObjectGodRays, "u_light_position");
	gTexture_sampler_uniformGodRays = glGetUniformLocation(gShaderProgramObjectGodRays, "u_texture_sampler2D");


	const GLfloat Coords[] =
	{
		-1.0f,-1.0f,0.0f,
		-1.0f,1.0f,0.0f,
		 1.0f,1.0f,0.0f,
		 1.0f,-1.0f,0.0f
	};

	const GLfloat smileyTexCorrds[] =
	{
		0.0f,0.0f,
		0.0f,1.0f,
		1.0f,1.0f,
		1.0f,0.0f
	};

	//VAO SMILEY
	glGenVertexArrays(1, &gVao_Square);
	glBindVertexArray(gVao_Square);

	glGenBuffers(1, &gVboPosSquare);
	glBindBuffer(GL_ARRAY_BUFFER, gVboPosSquare);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Coords), Coords, GL_STATIC_DRAW);
	glVertexAttribPointer(VDG_ATTRIBUTE_VERTEX, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(VDG_ATTRIBUTE_VERTEX);

	glBindBuffer(GL_ARRAY_BUFFER, 0);


	glGenBuffers(1, &gVboTexSquare);
	glBindBuffer(GL_ARRAY_BUFFER, gVboTexSquare);
	glBufferData(GL_ARRAY_BUFFER, sizeof(smileyTexCorrds), smileyTexCorrds, GL_STATIC_DRAW);
	glVertexAttribPointer(VDG_ATTRIBUTE_TEXTURE0, 2, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(VDG_ATTRIBUTE_TEXTURE0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0);

	///////////////////////////////////////////////////Texture //////////////////////



		//*****VERTEX SHADER*******
	gVertexShaderObjectTexture = glCreateShader(GL_VERTEX_SHADER);

	//provide source code to shader
	const GLchar * vertexShaderCodeTexture =
		"#version 450 core" \
		"\n" \
		"in vec4 vPosition;" \
		"in vec2 vTexture0_coord;" \
		"out vec2 out_texture0_coord;" \
		"void main(void)" \
		"{" \
		"gl_Position =  vPosition;" \
		"out_texture0_coord = vTexture0_coord;" \
		"}";

	glShaderSource(gVertexShaderObjectTexture, 1, (const GLchar**)&vertexShaderCodeTexture, NULL);

	//compile shader
	glCompileShader(gVertexShaderObjectTexture);

	iInfoLength = 0;
	iShaderCompiledStatus = 0;
	szInfoLog = NULL;

	glGetShaderiv(gVertexShaderObjectTexture, GL_COMPILE_STATUS, &iShaderCompiledStatus);
	if (iShaderCompiledStatus == GL_FALSE)
	{
		glGetShaderiv(gVertexShaderObjectTexture, GL_INFO_LOG_LENGTH, &iInfoLength);
		if (iInfoLength > 0)
		{
			szInfoLog = (char *)malloc(iInfoLength);
			if (szInfoLog != NULL)
			{
				GLsizei written;
				glGetShaderInfoLog(gVertexShaderObjectTexture, iInfoLength, &written, szInfoLog);
				fprintf(gpFile, "Vertex Shader Compilation Log:%s\n", szInfoLog);
				free(szInfoLog);
				uninitialize();
				exit(0);
			}
		}
	}



	//*****FRAGMENT SHADER****
	gFragmentShaderObjectTexture = glCreateShader(GL_FRAGMENT_SHADER);

	//provide source code to shader
	const GLchar *fragmentShaderSourceCodeTexture =
		"#version 430 core" \
		"\n" \
		"in vec2 out_texture0_coord;" \
		"out vec4 FragColor;" \
		"uniform sampler2D u_texture0_sampler;" \
		"uniform sampler2D u_texture1_sampler;" \

		"void main(void)" \
		"{" \
		"vec4 final = texture(u_texture0_sampler,out_texture0_coord);" \
		"final += texture(u_texture1_sampler,out_texture0_coord);" \
		"FragColor = final;"\
		"}";


	glShaderSource(gFragmentShaderObjectTexture, 1, (const GLchar**)&fragmentShaderSourceCodeTexture, NULL);

	//compile shader
	glCompileShader(gFragmentShaderObjectTexture);
	glGetShaderiv(gFragmentShaderObjectTexture, GL_COMPILE_STATUS, &iShaderCompiledStatus);
	if (iShaderCompiledStatus == GL_FALSE)
	{
		glGetShaderiv(gFragmentShaderObjectTexture, GL_INFO_LOG_LENGTH, &iShaderCompiledStatus);
		if (iInfoLength > 0)
		{
			szInfoLog = (char *)malloc(iInfoLength);
			if (szInfoLog != NULL)
			{
				GLsizei written;
				glGetShaderInfoLog(gFragmentShaderObjectTexture, iInfoLength, &written, szInfoLog);
				fprintf(gpFile, "Fragment shader compilation Log %s\n", szInfoLog);
				free(szInfoLog);
				uninitialize();
				exit(0);
			}
		}
	}


	//*****SHADER PROGRAM******
	//create shader program
	gShaderProgramObjectTexture = glCreateProgram();

	//attach vertex shader to shader program
	glAttachShader(gShaderProgramObjectTexture, gVertexShaderObjectTexture);

	//attach fragment shader to shader program
	glAttachShader(gShaderProgramObjectTexture, gFragmentShaderObjectTexture);

	glBindAttribLocation(gShaderProgramObjectTexture, VDG_ATTRIBUTE_VERTEX, "vPosition");

	glBindAttribLocation(gShaderProgramObjectTexture, VDG_ATTRIBUTE_TEXTURE0, "vTexture0_coord");

	//link shader
	glLinkProgram(gShaderProgramObjectTexture);

	iProgramLinkStatus = 0;
	glGetProgramiv(gShaderProgramObjectTexture, GL_LINK_STATUS, &iShaderCompiledStatus);
	if (iShaderCompiledStatus == GL_FALSE)
	{
		glGetProgramiv(gShaderProgramObjectTexture, GL_INFO_LOG_LENGTH, &iInfoLength);
		if (iInfoLength > 0)
		{
			szInfoLog = (char*)malloc(iInfoLength);
			if (szInfoLog != NULL)
			{
				GLsizei written;
				glGetProgramInfoLog(gShaderProgramObjectTexture, iInfoLength, &written, szInfoLog);
				fprintf(gpFile, "Shader Program Link Status %s\n", szInfoLog);
				free(szInfoLog);
				uninitialize();
				exit(0);
			}
		}
	}

	// post-linking retrieving uniform locations
	gTexture_sampler_uniformTexture = glGetUniformLocation(gShaderProgramObjectTexture, "u_texture0_sampler");

	// create vao
	glGenVertexArrays(1, &gVao_Square);
	glBindVertexArray(gVao_Square);

	glGenBuffers(1, &gVboPosSquare);
	glBindBuffer(GL_ARRAY_BUFFER, gVboPosSquare);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Coords), Coords, GL_STATIC_DRAW);
	glVertexAttribPointer(VDG_ATTRIBUTE_VERTEX, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(VDG_ATTRIBUTE_VERTEX);

	glBindBuffer(GL_ARRAY_BUFFER, 0);


	glGenBuffers(1, &gVboTexSquare);
	glBindBuffer(GL_ARRAY_BUFFER, gVboTexSquare);
	glBufferData(GL_ARRAY_BUFFER, sizeof(smileyTexCorrds), smileyTexCorrds, GL_STATIC_DRAW);
	glVertexAttribPointer(VDG_ATTRIBUTE_TEXTURE0, 2, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(VDG_ATTRIBUTE_TEXTURE0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0);




	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////



	///////////////////////////////////// FrameBuffer 1.GodRays //////////////////////////////////////////////////


	glGenFramebuffers(1, &GodRaysFBO);

	glGenTextures(1, &colorBuffer);

	//normal color
	glBindTexture(GL_TEXTURE_2D, colorBuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, WIN_WIDTH, WIN_HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);


	glGenRenderbuffers(1, &rboDepth);
	glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, WIN_WIDTH, WIN_HEIGHT);

	glBindFramebuffer(GL_FRAMEBUFFER, GodRaysFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorBuffer, 0);

	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);

	// tell opengl which color attachments we will use for this framebuffer
	GLuint attachments[] = { GL_COLOR_ATTACHMENT0 };
	glDrawBuffers(2, attachments);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		fprintf_s(gpFile, "God Rays Framebuffer not complete");
	glBindFramebuffer(GL_FRAMEBUFFER, 0);


	/////////////////////////////////// Final framebuffer ///////////////////////////////////////////////////////////////////


	glGenFramebuffers(1, &FinalFBO);

	glGenTextures(1, &FinalcolorBuffer);

	//normal color
	glBindTexture(GL_TEXTURE_2D, FinalcolorBuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, WIN_WIDTH, WIN_HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);


	glBindFramebuffer(GL_FRAMEBUFFER, FinalFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, FinalcolorBuffer, 0);

	glDrawBuffers(2, attachments);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		fprintf_s(gpFile, "Texture Framebuffer not complete");
	glBindFramebuffer(GL_FRAMEBUFFER, 0);



	/*	glGenFramebuffers(1, &godraysFBO);

		glGenTextures(1, &colorBuffer);

		// normal color
		glBindTexture(GL_TEXTURE_2D, colorBuffer);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, WIN_WIDTH, WIN_HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		glGenRenderbuffers(1, &rboDepth);
		glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, WIN_WIDTH, WIN_HEIGHT);

		glBindFramebuffer(GL_FRAMEBUFFER, godraysFBO);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorBuffer, 0);

		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);

		// tell opengl which color attachments we will use for this framebuffer
		GLuint attachments[] = { GL_COLOR_ATTACHMENT0 };
		glDrawBuffers(2, attachments);

		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			fprintf_s(gpFile, "\nRTR: Framebuffer not complete...");
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		///////////////////////////////////////////////////////////////////////////////////////////////


		////// final framebuffer object /////////////////////////////////////////////////////////////

		glGenFramebuffers(1, &finalFBO);

		glGenTextures(1, &finalcolorBuffer);

		// normal color
		glBindTexture(GL_TEXTURE_2D, finalcolorBuffer);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, WIN_WIDTH, WIN_HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		glBindFramebuffer(GL_FRAMEBUFFER, finalFBO);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, finalcolorBuffer, 0);

		// tell opengl which color attachments we will use for this framebuffer
		glDrawBuffers(1, attachments);

		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			fprintf_s(gpFile, "\nRTR: Framebuffer not complete...");
		glBindFramebuffer(GL_FRAMEBUFFER, 0);*/

		//makeSphere(0.5, 40, 40);



	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	glClearColor(0.1f, 0.1f, 0.1f, 0.0f);
	gPerspectiveProjectionMatrix = mat4::identity();
	resize(WIN_WIDTH, WIN_HEIGHT);
	fprintf(gpFile, "Exitting Initialize..\n");
	ToggleFullScreen();
}


vec4 vec4Transform(vec4& vSrcVector, mat4 matSrcMatrix)
{
	vec4 vDstVector;
	vDstVector[0] = matSrcMatrix[0][0] * vSrcVector[0] +
		matSrcMatrix[1][0] * vSrcVector[1] +
		matSrcMatrix[2][0] * vSrcVector[2] +
		matSrcMatrix[3][0] * vSrcVector[3];

	vDstVector[1] = matSrcMatrix[0][1] * vSrcVector[0] +
		matSrcMatrix[1][1] * vSrcVector[1] +
		matSrcMatrix[2][1] * vSrcVector[2] +
		matSrcMatrix[3][1] * vSrcVector[3];

	vDstVector[2] = matSrcMatrix[0][2] * vSrcVector[0] +
		matSrcMatrix[1][2] * vSrcVector[1] +
		matSrcMatrix[2][2] * vSrcVector[2] +
		matSrcMatrix[3][2] * vSrcVector[3];

	vDstVector[3] = matSrcMatrix[0][3] * vSrcVector[0] +
		matSrcMatrix[1][3] * vSrcVector[1] +
		matSrcMatrix[2][3] * vSrcVector[2] +
		matSrcMatrix[3][3] * vSrcVector[3];
	return vDstVector;
}




void display()
{

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// intialize above matrices to identity
	mat4 modelMatrix = mat4::identity();
	mat4 viewMatrix = mat4::identity();
	mat4 projectionMatrix = mat4::identity();


	vec4 light_position = vec4(
		 0,
		 0,
		-5, 0);

	modelMatrix = translate(0.0f, 0.0f, -5.0f);

	projectionMatrix = gPerspectiveProjectionMatrix;


	glBindFramebuffer(GL_FRAMEBUFFER, GodRaysFBO);
	glViewport(0, 0, gWidth, gHeight);

	/////////////////////////////////////////////// Light  

	glUseProgram(gShaderProgramObject);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Setup all the matrices
	glUniformMatrix4fv(model_matrix_uniform, 1, GL_FALSE, modelMatrix);
	glUniformMatrix4fv(view_matrix_uniform, 1, GL_FALSE, viewMatrix);
	glUniformMatrix4fv(projection_matrix_uniform, 1, GL_FALSE, projectionMatrix);
	glUniform3fv(LightPosUniform, 1, light_position);

	vec4 v = vec4Transform(light_position, modelMatrix);
	v = vec4Transform(v, projectionMatrix);

	v = vec4(v[0] / v[3], v[1] / v[3], v[2] / v[3], v[3] / v[3]);

	v = v + vec4(1.0, 1.0, 0.0, 0.0);

	v = vec4(v[0] * 0.5, v[1] * 0.5, v[2] * 0.5, v[3] * 0.5);

	light_position[0] = v[0];
	light_position[1] = v[1];
	////
	//modelMatrix *= translate(light_position);

	/*light_position[2] -= 200.0;
	float lightX = (0.0*light_position[0]);
	float lightY = (0.1*light_position[1]);
	float lightW = (light_position[2]);

	lightX /= lightW;
	lightY /= lightW;

	lightX = (lightX + 1.0f)*0.5f;
	lightY = (lightY + 1.0f)*0.5f;*/

	modelMatrix *= scale(0.11f, 0.11f, 0.11f);
	glUniformMatrix4fv(model_matrix_uniform, 1, GL_FALSE, modelMatrix);
	glUniform3fv(kaUniform, 1, vec3(1.0f, 1.0f, 1.0f));
	glUniform3fv(kdUniform, 1, vec3(1.0f, 1.0f, 1.0f));
	glUniform3fv(ksUniform, 1, vec3(1.0f, 1.0f, 1.0f));
	glUniform3fv(La_uniform, 1, vec3(1.0f, 1.0f, 1.0f));
	glUniform3fv(Ld_uniform, 1, vec3(1.0f, 1.0f, 1.0f));
	glUniform3fv(Ls_uniform, 1, vec3(1.0f, 1.0f, 1.0f));

	glUniform1f(MaterialShininessUniform, 128.0f);


	glBindVertexArray(gVao_Cube);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
	glDrawArrays(GL_TRIANGLE_FAN, 4, 4);
	glDrawArrays(GL_TRIANGLE_FAN, 8, 4);
	glDrawArrays(GL_TRIANGLE_FAN, 12, 4);
	glDrawArrays(GL_TRIANGLE_FAN, 16, 4);
	glDrawArrays(GL_TRIANGLE_FAN, 20, 4);

	glBindVertexArray(0);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// unuse program
	glUseProgram(0);


	/////////////////////////////////////////////// God Rays 

	glBindFramebuffer(GL_FRAMEBUFFER, FinalFBO);
	glUseProgram(gShaderProgramObjectGodRays);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, colorBuffer);

	//light_position[2] -= 200.0;
	//float lightX = (0.974313*light_position[0]);
	//float lightY = (1.732113*light_position[1]);
	//float lightW = (-1 * light_position[2]);

	//lightY /= lightW;

	//lightX = (lightX + 1.0f)*0.5f;
	//lightY = (lightY + 1.0f)*0.5f;

	glUniform4fv(gLightPositionGodRays, 1, vec4(light_position[0], light_position[1], 0.0, 0.0));

	glBindVertexArray(gVao_Square);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

	glUseProgram(0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	/////////////////////////////////////////////Texture 

	glUseProgram(gShaderProgramObjectTexture);


	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, FinalcolorBuffer);
	glUniform1i(gTexture_sampler_uniformTexture, 1);

	glBindVertexArray(gVao_Square);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

	glUseProgram(0);


	//////////////////////////////////////////////

	SwapBuffers(ghdc);

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


void resize(int width, int height)
{
	if (height == 0)
		height = 1;

	gWidth = width;
	gHeight = height;

	glViewport(0, 0, (GLsizei)width, (GLsizei)height);
	gPerspectiveProjectionMatrix = perspective(60.0f, (GLfloat)width / (GLfloat)height, 0.1f, 1000.0f);
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


























