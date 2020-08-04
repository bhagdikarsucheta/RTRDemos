

#include<windows.h>
#include<stdio.h>
#include<gl/glew.h>
#include<gl/GL.h>

#include"vmath.h"
#include"Sphere.h"

using namespace vmath;

#define WIN_WIDTH 800
#define WIN_HEIGHT 600

#pragma comment(lib,"glew32.lib")
#pragma comment(lib,"opengl32.lib")

#pragma comment(lib,"Sphere.lib")


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
	VDG_ATTRIBUTE_NORMAL,
	VDG_ATTRIBUTE_BIRD_POSITION,
	VDG_ATTRIBUTE_BIRD_VELOCITY
};


	enum
	{
		WORKGROUP_SIZE = 256,
		NUM_WORKGROUPS = 64,
		FLOCK_SIZE = (NUM_WORKGROUPS * WORKGROUP_SIZE)
	};

	GLuint gVertexShaderObjectFlocks;
	GLuint gFragmentShaderObjectFlocks;
	GLuint gShaderProgramObjectFlocks;
	GLuint gComputeShaderObjectFlocks;

	GLuint gComputeShaderProgramObjectFlocks;

	GLuint La_uniformFlocks;
	GLuint Ld_uniformFlocks;
	GLuint Ls_uniformFlocks;
	GLuint light_position_uniformFlocks;

	GLuint Ka_uniformFlocks;
	GLuint Kd_uniformFlocks;
	GLuint Ks_uniformFlocks;
	GLuint material_shininess_uniformFlocks;


mat4 gPerspectiveProjectionMatrix;
GLfloat myrotationArrBlue[4];
GLfloat myrotationArrGreen[4];
GLfloat myrotationArrRed[4];


int windowWidth;
int windowHeight;

GLfloat angleBlue;

//GLfloat lightAmbient[] = { 0.0f,0.0f,0.0f,1.0f };
//GLfloat lightDiffuse[] = { 0.0f,0.0f,1.0f,1.0f };
//GLfloat lightSpecular[] = { 0.0f,0.0f,1.0f,1.0f };
//GLfloat lightPosition[] = { 0.0f,0.0f,0.0f,1.0f };
//
//GLfloat material_ambient[] = { 0.0f,0.0f,0.0f,1.0f };
//GLfloat material_diffuse[] = { 1.0f,1.0f,1.0f,1.0f };
//GLfloat material_specular[] = { 1.0f,1.0f,1.0f,1.0f };
//GLfloat material_shininess = 50.0f;
bool gbLight;
bool gbFLight;

GLuint flock_render_vao[2];
GLuint flock_buffer[2];
GLuint geometry_buffer;
struct flock_member
{
	vmath::vec3 position;
	unsigned int : 32;
	vmath::vec3 velocity;
	unsigned int : 32;
};
GLuint gMVPUniformFlocks;
GLuint MVMatrixFlocks;
GLuint gGoal;
GLuint frame_index;


FILE *gpFile = NULL;

//main()
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdLine, int nCmdShow)
{
	void display();
	void initialize();
	void uninitialize();
	void update();
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
	static bool gbIsFKeyPressed = false;
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
		windowWidth = LOWORD(lParam);
		windowHeight = HIWORD(lParam);
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

	//****COMPUTE SHADER******
	gComputeShaderObjectFlocks = glCreateShader(GL_COMPUTE_SHADER);

	//provide source code to shader
	const GLchar *computeShaderSourceCodeFlocks =
		"#version 430 core" \
		"\n" \


		"layout(local_size_x = 256) in;"\

		"uniform float closest_allowed_dist = 50.0;"\
		"uniform float rule1_weight = 0.18;"\
		"uniform float rule2_weight = 0.05;"\
		"uniform float rule3_weight = 0.17;"\
		"uniform float rule4_weight = 0.02;"\
		"uniform vec3 goal = vec3(0.0);"\
		"uniform float timestep = 0.4;"\

		"struct flock_member"\
		"{"\
		"vec3 position;"\
		"vec3 velocity;"\
		"};"\

		"layout(std430, binding = 0) readonly buffer members_in"\
		"{"\
		"flock_member member[];"\
		"} input_data;"\

		"layout(std430, binding = 1) buffer members_out"\
		"{"\
		"flock_member member[];"\
		"} output_data;"\

		"shared flock_member shared_member[gl_WorkGroupSize.x];"\

		"vec3 rule1(vec3 my_position, vec3 my_velocity, vec3 their_position, vec3 their_velocity)"\
		"{"\
		"vec3 d = my_position - their_position;"\
		"if (dot(d, d) < closest_allowed_dist)"\
		"	return d;"\
		"return vec3(0.0);"\
		"}"\

		"vec3 rule2(vec3 my_position, vec3 my_velocity, vec3 their_position, vec3 their_velocity)"\
		"{"\
		"	vec3 d = their_position - my_position;"\
		"	vec3 dv = their_velocity - my_velocity;"\
		"	return dv / (dot(d, d) + 100.0);"\
		"}"\

		"void main(void)"\
		"{"\

		"int i, j;"\
		"int global_id = int(gl_GlobalInvocationID.x);"\
		"int local_id = int(gl_LocalInvocationID.x);"\

		"flock_member me = input_data.member[global_id];"\
		"flock_member new_me;"\
		"vec3 accelleration = vec3(0.0);"\
		"vec3 flock_center = vec3(0.0);"

		"for (i = 0; i < gl_NumWorkGroups.x; i++)"\
		"{"\
		"	flock_member them ="\
		"	input_data.member[i * gl_WorkGroupSize.x +"\
		"	local_id];"\
		"	shared_member[local_id] = them;"\
		"	memoryBarrierShared();"\
		"	barrier();"\
		"	for (j = 0; j < gl_WorkGroupSize.x; j++)"\
		"	{"\
		"	them = shared_member[j];"\
		"	flock_center += them.position;"\
		"		if (i * gl_WorkGroupSize.x + j != global_id)"\
		"		{"\
		"		accelleration += rule1(me.position,"\
		"		me.velocity,"\
		"		them.position,"\
		"		them.velocity) * rule1_weight;"\
		"		accelleration += rule2(me.position,"\
		"		me.velocity,"\
		"		them.position,"\
		"		them.velocity) * rule2_weight;"\
		"		}"\
		"	}"\
		"barrier();"\
		"}"\

		"flock_center /= float(gl_NumWorkGroups.x * gl_WorkGroupSize.x);"\
		"new_me.position = me.position + me.velocity * timestep;"\
		"accelleration += normalize(goal - me.position) * rule3_weight;"\
		"accelleration += normalize(flock_center - me.position) * rule4_weight;"\
		"new_me.velocity = me.velocity + accelleration * timestep;"\
		"if (length(new_me.velocity) > 10.0)"\
		"	new_me.velocity = normalize(new_me.velocity) * 10.0;"\
		"new_me.velocity = mix(me.velocity, new_me.velocity, 0.4);"\
		"output_data.member[global_id] = new_me;"\
		"}";


	glShaderSource(gComputeShaderObjectFlocks, 1, (const GLchar**)&computeShaderSourceCodeFlocks, NULL);
	GLint iInfoLength = 0;
	GLint	iShaderCompiledStatus = 0;
	char *szInfoLog = NULL;

	//compile shader
	glCompileShader(gComputeShaderObjectFlocks);
	glGetShaderiv(gComputeShaderObjectFlocks, GL_COMPILE_STATUS, &iShaderCompiledStatus);
	if (iShaderCompiledStatus == GL_FALSE)
	{
		glGetShaderiv(gComputeShaderObjectFlocks, GL_INFO_LOG_LENGTH, &iShaderCompiledStatus);
		if (iInfoLength > 0)
		{
			szInfoLog = (char *)malloc(iInfoLength);
			if (szInfoLog != NULL)
			{
				GLsizei written;
				glGetShaderInfoLog(gComputeShaderObjectFlocks, iInfoLength, &written, szInfoLog);
				fprintf(gpFile, "Compute Shader Compilation Log : %s\n", szInfoLog);
				free(szInfoLog);
				uninitialize();
				exit(0);
			}
		}
	}

	gComputeShaderProgramObjectFlocks = glCreateProgram();

	glAttachShader(gComputeShaderProgramObjectFlocks, gComputeShaderObjectFlocks);



	//pre-link binding of shader program object with vertex position attribute

	//Link Shader
	glLinkProgram(gComputeShaderProgramObjectFlocks);
	GLint iShaderProgramLinkStatus = 0;
	glGetProgramiv(gComputeShaderProgramObjectFlocks, GL_LINK_STATUS, &iShaderProgramLinkStatus);
	if (iShaderProgramLinkStatus == GL_FALSE)
	{
		glGetProgramiv(gComputeShaderProgramObjectFlocks, GL_INFO_LOG_LENGTH, &iInfoLength);
		if (iInfoLength > 0)
		{
			szInfoLog = (char *)malloc(iInfoLength);
			if (szInfoLog != NULL)
			{
				GLsizei written;
				glGetProgramInfoLog(gComputeShaderProgramObjectFlocks, iInfoLength, &written, szInfoLog);
				fprintf(gpFile, "Compute Shader Program LinK Log %s\n");
				free(szInfoLog);
				uninitialize();
				exit(0);
			}
		}
	}
	gGoal = glGetUniformLocation(gComputeShaderObjectFlocks, "goal");



	//*****VERTEX SHADER*******
	//create shader 
	gVertexShaderObjectFlocks = glCreateShader(GL_VERTEX_SHADER);

	//provide source code to shader
	const GLchar * vertexShaderShaderCodeFlocks =
		"#version 430 core"	\
		"\n"\
		"in vec3 vPosition;"\
		"in vec3 vNormal;"\
		"uniform vec4 u_light_position;" \

		"uniform int u_lighting_enabled;" \
		"out vec3 transformed_normals;" \
		"out vec3 light_directionBlue;" \

		"out vec3 viewer_vector;" \

		"in vec3 bird_position;"\
		"in vec3 bird_velocity;"\
	

		"uniform mat4 u_mvp_matrix;"\
		"uniform mat4 u_mv_matrix;"\


		"out VS_OUT"\
		"{"\
		" vec3 color;"\
		"} vs_out;"


		"mat4 make_lookat(vec3 forward, vec3 up)"\
		"{"\
		"vec3 side = cross(forward, up);"\
		"vec3 u_frame = cross(side, forward);"\

		"return mat4(vec4(side, 0.0),"\
		"	vec4(u_frame, 0.0),"\
		"	vec4(forward, 0.0),"\
		"	vec4(0.0, 0.0, 0.0, 1.0));"\
		"}"\

		"vec3 choose_color(float f)"\
		"{"\
		"	float R = sin(f * 6.2831853);"\
		"	float G = sin((f + 0.9666) * 96.2831853);"\
		"	float B = sin((f + 0.9999) * 96.2831853);"\

		"	return vec3(R, G, B) * 0.45 + vec3(0.5);"\
		"}"\

		"void main(void)"\
		"{"
		"	mat4 lookat = make_lookat(normalize(bird_velocity), vec3(0.0, 1.0, 0.0));"\
		"	vec4 obj_coord = lookat * vec4(vPosition.xyz, 1.0);"\
		"	gl_Position = u_mvp_matrix * (obj_coord + vec4(bird_position, 0.0));"\

		"	vec3 N = mat3(lookat) * vNormal;"\
		"	vec3 C = choose_color(fract(float(gl_InstanceID / float(1237.0))));"\


		"	vs_out.color = mix(C * 0.9, C, smoothstep(0.0, 0.9, abs(N).z));"\
		"}";


	glShaderSource(gVertexShaderObjectFlocks, 1, (const GLchar**)&vertexShaderShaderCodeFlocks, NULL);

	//compile shader
	glCompileShader(gVertexShaderObjectFlocks);

	iInfoLength = 0;
	iShaderCompiledStatus = 0;
	szInfoLog = NULL;

	glGetShaderiv(gVertexShaderObjectFlocks, GL_COMPILE_STATUS, &iShaderCompiledStatus);
	if (iShaderCompiledStatus == GL_FALSE)
	{
		glGetShaderiv(gVertexShaderObjectFlocks, GL_INFO_LOG_LENGTH, &iInfoLength);
		if (iInfoLength > 0)
		{
			szInfoLog = (char *)malloc(iInfoLength);
			if (szInfoLog != NULL)
			{
				GLsizei written;
				glGetShaderInfoLog(gVertexShaderObjectFlocks, iInfoLength, &written, szInfoLog);
				fprintf(gpFile, "Vertex Shader Compilation Log:%s\n", szInfoLog);
				free(szInfoLog);
				uninitialize();
				exit(0);
			}
		}
	}


	//*****FRAGMENT SHADER****
	gFragmentShaderObjectFlocks = glCreateShader(GL_FRAGMENT_SHADER);

	//provide source code to shader
	const GLchar *fragmentShaderSourceCodeFlocks =
		"#version 430 core"	\
		"\n" \
		"in vec3 transformed_normals;" \
		"in vec3 light_directionBlue;" \
		"uniform mat4 u_mv_matrix;"\
		"in vec3 viewer_vector;" \

		"uniform vec3 u_La;" \
		"uniform vec3 u_Ld;" \
		"uniform vec3 u_Ls;" \
		"uniform vec3 u_Ka;" \
		"uniform vec3 u_Kd;" \
		"uniform vec3 u_Ks;" \


		"uniform float u_material_shininess;" \
		"uniform int u_lighting_enabled;" \

		"out vec4 Color;"\

		"in VS_OUT"\
		"{"\
		" vec3 color;"\
		"} fs_in; "\

		"void main(void)"\
		"{"\
		"vec3 phong_ads_color;" \

		/*	"vec3 normalized_transformed_normals=normalize(transformed_normals);" \
			"vec3 normalized_viewer_vector=normalize(viewer_vector);" \

			"vec3 normalized_light_directionBlue=normalize(light_directionBlue);" \
			"vec3 ambient = u_La * u_Ka;" \
			"float tn_dot_ld = max(dot(normalized_transformed_normals, normalized_light_directionBlue),0.0);" \
			"vec3 diffuse = u_Ld * u_Kd * tn_dot_ld;" \
			"vec3 reflection_vector = reflect(-normalized_light_directionBlue,normalized_transformed_normals);" \
			"vec3 specular = u_Ls * u_Ks * pow(max(dot(reflection_vector,normalized_viewer_vector),0.0),u_material_shininess);" \
			"phong_ads_color = ambient + diffuse + specular;"  \
			"Color =vec4(phong_ads_color,1.0);" \
		*/
		"Color = vec4(fs_in.color, 1.0);"\

		"}"; \



		glShaderSource(gFragmentShaderObjectFlocks, 1, (const GLchar**)&fragmentShaderSourceCodeFlocks, NULL);

	//compile shader
	glCompileShader(gFragmentShaderObjectFlocks);
	glGetShaderiv(gFragmentShaderObjectFlocks, GL_COMPILE_STATUS, &iShaderCompiledStatus);
	if (iShaderCompiledStatus == GL_FALSE)
	{
		glGetShaderiv(gFragmentShaderObjectFlocks, GL_INFO_LOG_LENGTH, &iShaderCompiledStatus);
		if (iInfoLength > 0)
		{
			szInfoLog = (char *)malloc(iInfoLength);
			if (szInfoLog != NULL)
			{
				GLsizei written;
				glGetShaderInfoLog(gFragmentShaderObjectFlocks, iInfoLength, &written, szInfoLog);
				fprintf(gpFile, "Fragment shader compilation Log %s\n", szInfoLog);
				free(szInfoLog);
				uninitialize();
				exit(0);
			}
		}
	}


	//*****SHADER PROGRAM******
	//create shader program
	gShaderProgramObjectFlocks = glCreateProgram();

	//attach vertex shader to shader program
	glAttachShader(gShaderProgramObjectFlocks, gVertexShaderObjectFlocks);

	//attach fragment shader to shader program
	glAttachShader(gShaderProgramObjectFlocks, gFragmentShaderObjectFlocks);

	//pre-link binding of shader program object with vertex position attribute
	glBindAttribLocation(gShaderProgramObjectFlocks, VDG_ATTRIBUTE_VERTEX, "vPosition");
	glBindAttribLocation(gShaderProgramObjectFlocks, VDG_ATTRIBUTE_NORMAL, "vNormal");
	glBindAttribLocation(gShaderProgramObjectFlocks, VDG_ATTRIBUTE_BIRD_POSITION, "bird_position");
	glBindAttribLocation(gShaderProgramObjectFlocks, VDG_ATTRIBUTE_BIRD_VELOCITY, "bird_velocity");

	//Link Shader
	glLinkProgram(gShaderProgramObjectFlocks);
	iShaderProgramLinkStatus = 0;
	glGetProgramiv(gShaderProgramObjectFlocks, GL_LINK_STATUS, &iShaderProgramLinkStatus);
	if (iShaderProgramLinkStatus == GL_FALSE)
	{
		glGetProgramiv(gShaderProgramObjectFlocks, GL_INFO_LOG_LENGTH, &iInfoLength);
		if (iInfoLength > 0)
		{
			szInfoLog = (char *)malloc(iInfoLength);
			if (szInfoLog != NULL)
			{
				GLsizei written;
				glGetProgramInfoLog(gShaderProgramObjectFlocks, iInfoLength, &written, szInfoLog);
				fprintf(gpFile, "Shader Program LinK Log %s\n");
				free(szInfoLog);
				uninitialize();
				exit(0);
			}
		}
	}


	//get MVP uniform locaion
	gMVPUniformFlocks = glGetUniformLocation(gShaderProgramObjectFlocks, "u_mvp_matrix");
	MVMatrixFlocks = glGetUniformLocation(gShaderProgramObjectFlocks, "u_mv_matrix");

	La_uniformFlocks = glGetUniformLocation(gShaderProgramObjectFlocks, "u_La");
	//diffuse color intensity of LIGHT
	Ld_uniformFlocks = glGetUniformLocation(gShaderProgramObjectFlocks, "u_Ld");
	//specular color intensity of LIGHT
	Ls_uniformFlocks = glGetUniformLocation(gShaderProgramObjectFlocks, "u_Ls");
	//position of light
	light_position_uniformFlocks = glGetUniformLocation(gShaderProgramObjectFlocks, "u_light_position");

	//ambient reflective color intensity of MATERIAL
	Ka_uniformFlocks = glGetUniformLocation(gShaderProgramObjectFlocks, "u_Ka");
	//diffuse reflective color intensity of MATERIAL
	Kd_uniformFlocks = glGetUniformLocation(gShaderProgramObjectFlocks, "u_Kd");

	//specular reflective color intensity of MATERIAL

	material_shininess_uniformFlocks = glGetUniformLocation(gShaderProgramObjectFlocks, "u_material_shininess");

	Ks_uniformFlocks = glGetUniformLocation(gShaderProgramObjectFlocks, "u_Ks");
	//vertices colors shader attribs 
	static const vmath::vec3 geometry[] =
	{		vmath::vec3(-0.0f, 1.0f, 0.0f),
			vmath::vec3(-1.0f, 0.5f, 0.0f),
			vmath::vec3(-1.0f, 0.5f,2.0f),
			vmath::vec3(0.0f, 0.0f, 0.0f),
			vmath::vec3(0.0f, 0.0f, 1.0f),
			vmath::vec3(1.0f, 1.5f, 0.0f),
			vmath::vec3(1.0f, 1.5f, 1.0f),
			vmath::vec3(2.0f, 0.0f, 0.0f),

		// Normals
		vmath::vec3(0.0f),
		vmath::vec3(0.0f),
		vmath::vec3(0.107f, -0.859f, 0.00f),
		vmath::vec3(0.932f, 0.554f, 0.00f),
		vmath::vec3(-0.59f, -0.395f, 0.00f),
		vmath::vec3(-0.932f, 0.554f, 0.00f),
		vmath::vec3(0.295f, -0.196f, 0.00f),
		vmath::vec3(0.124f, 0.992f, 0.00f),

	};

	glGenBuffers(2, flock_buffer);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, flock_buffer[0]);
	glBufferData(GL_SHADER_STORAGE_BUFFER, FLOCK_SIZE * sizeof(flock_member), NULL, GL_DYNAMIC_COPY);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, flock_buffer[1]);
	glBufferData(GL_SHADER_STORAGE_BUFFER, FLOCK_SIZE * sizeof(flock_member), NULL, GL_DYNAMIC_COPY);

	int i;

	glGenBuffers(1, &geometry_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, geometry_buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(geometry), geometry, GL_STATIC_DRAW);

	glGenVertexArrays(2, flock_render_vao);

	for (i = 0; i < 2; i++)
	{
		glBindVertexArray(flock_render_vao[i]);
		glBindBuffer(GL_ARRAY_BUFFER, geometry_buffer);
		glVertexAttribPointer(VDG_ATTRIBUTE_VERTEX, 3, GL_FLOAT, GL_FALSE, 0, NULL);
		glVertexAttribPointer(VDG_ATTRIBUTE_NORMAL, 3, GL_FLOAT, GL_FALSE, 0, (void *)(6 * sizeof(vmath::vec3)));

		glBindBuffer(GL_ARRAY_BUFFER, flock_buffer[i]);
		glVertexAttribPointer(VDG_ATTRIBUTE_BIRD_POSITION, 3, GL_FLOAT, GL_FALSE, sizeof(flock_member), NULL);
		glVertexAttribPointer(VDG_ATTRIBUTE_BIRD_VELOCITY, 3, GL_FLOAT, GL_FALSE, sizeof(flock_member), (void *)sizeof(vmath::vec4));
		glVertexAttribDivisor(VDG_ATTRIBUTE_BIRD_POSITION, 1);
		glVertexAttribDivisor(VDG_ATTRIBUTE_BIRD_VELOCITY, 1);

		glEnableVertexAttribArray(VDG_ATTRIBUTE_VERTEX);
		glEnableVertexAttribArray(VDG_ATTRIBUTE_NORMAL);
		glEnableVertexAttribArray(VDG_ATTRIBUTE_BIRD_POSITION);
		glEnableVertexAttribArray(VDG_ATTRIBUTE_BIRD_VELOCITY);
	}

	glBindBuffer(GL_ARRAY_BUFFER, flock_buffer[0]);
	flock_member * ptr = reinterpret_cast<flock_member *>(glMapBufferRange(GL_ARRAY_BUFFER, 0, FLOCK_SIZE * sizeof(flock_member), GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT));

	for (i = 0; i < FLOCK_SIZE;i++)
	{
		/*if (FLOCK_SIZE <= FLOCK_SIZE/2)
		{*/
		//ptr[i].position = (vmath::vec3::random() + vmath::vec3(1.6f)) + 300.0f*sin(50.0f)*cos(50.0f);
		//ptr[i].velocity = (vmath::vec3::random() - vmath::vec3(1.6f));

	/*	ptr[i].position = (vmath::vec3::random() + vmath::vec3(1.6f)) + 30.0f*sin(50.0f)*cos(50.0f);
		ptr[i].velocity = (vmath::vec3::random() - vmath::vec3(1.6f));*/

		ptr[i].position = (vmath::vec3::random() + vmath::vec3(1.6f)) + 300.0f;
		ptr[i].velocity = (vmath::vec3::random() - vmath::vec3(1.6f));

	//****	//Random Spread -
		//ptr[i].position = (vmath::vec3::random() + vmath::vec3(1.6f)) + 300.0f*sin(50.0f)*cos(50.0f);
		//ptr[i].velocity = (vmath::vec3::random() - vmath::vec3(1.6f));

		//}	
	}

	//for (i = FLOCK_SIZE; i <0; i--)
	//{
	//	/*if (FLOCK_SIZE <= 8192)
	//	{*/
	//	ptr[i].position = (vmath::vec3::random() + vmath::vec3(10.6f)) + 300.0f;
	//	ptr[i].velocity = (vmath::vec3::random() - vmath::vec3(10.6f));
	//	//}
	//}

	glUnmapBuffer(GL_ARRAY_BUFFER);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	ToggleFullScreen();
}

void display()
{
	//double T = 2.0f;

	float t = (float)GetTickCount();
	static const float black[] = { 0.0f, 0.0f, 0.0f, 1.0f };
	static const float one = 1.0f;

	glUseProgram(gComputeShaderProgramObjectFlocks);

	/*vmath::vec3 goal = vmath::vec3(t,t,1.0f);*/
	vmath::vec3 goal = vmath::vec3(sinf(t * 1.34f),
		cosf(t * 1.29f),
		sinf(t * 1.12f) * cosf(t * 1.5f));

	goal = goal * vmath::vec3(35.0f, 25.0f, 60.0f);

	glUniform3fv(gGoal, 1, goal);

	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, flock_buffer[frame_index]);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, flock_buffer[frame_index ^ 1]);

	glDispatchCompute(NUM_WORKGROUPS, 1, 1);

	//glViewport(0, 0, windowWidth, windowHeight);
	glClearBufferfv(GL_COLOR, 0, black);
	glClearBufferfv(GL_DEPTH, 0, &one);

	glUseProgram(gShaderProgramObjectFlocks);

	vmath::mat4 mv_matrix = vmath::lookat(vmath::vec3(0.0f, 0.0f, -100.0f),
		vmath::vec3(0.0f, 0.0f, 0.0f),
		vmath::vec3(0.0f, 1.0f, 0.0f));
	vmath::mat4 modelviewMatrix;
	modelviewMatrix = translate(0.0f, 0.0f, -1.2f);

	vmath::mat4 proj_matrix = vmath::perspective(60.0f,
		(float)windowWidth / (float)windowHeight,
		0.1f,
		3000.0f);
	vmath::mat4 mvp = proj_matrix * mv_matrix;

	glUniformMatrix4fv(gMVPUniformFlocks, 1, GL_FALSE, mvp);

	glUniformMatrix4fv(MVMatrixFlocks, 1, GL_FALSE, modelviewMatrix);

	glBindVertexArray(flock_render_vao[frame_index]);

	glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 8, FLOCK_SIZE);

	frame_index ^= 1;
	
	SwapBuffers(ghdc);

}

void resize(int width, int height)
{
	if (height == 0)
		height = 1;
	glViewport(0, 0, (GLsizei)width, (GLsizei)height);
	//	gPerspectiveProjectionMatrix = perspective(45.0f, (GLfloat)width / (GLfloat)height, 0.1f, 100.0f);
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
	

	
	//detach vertex shader from shader program object
	glDetachShader(gShaderProgramObjectFlocks, gVertexShaderObjectFlocks);

	//detach fragment shader from shader program object
	glDetachShader(gShaderProgramObjectFlocks, gFragmentShaderObjectFlocks);

	//delete vertex shader object
	glDeleteShader(gVertexShaderObjectFlocks);
	gVertexShaderObjectFlocks = 0;

	//delete fragment shader object
	glDeleteShader(gFragmentShaderObjectFlocks);
	gFragmentShaderObjectFlocks = 0;

	//delete shader program object
	glDeleteProgram(gShaderProgramObjectFlocks);
	gShaderProgramObjectFlocks = 0;

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
	/*angleBlue = angleBlue + 0.1f;
	if (angleBlue > 360.0f)
		angleBlue = 0.0f;

	angleGreen = angleGreen + 0.1f;
	if (angleGreen > 360.0f)
		angleGreen = 0.0f;

	angleRed = angleRed + 0.1;
	if (angleRed > 360.0f)
		angleRed = 0.0f;*/
}









