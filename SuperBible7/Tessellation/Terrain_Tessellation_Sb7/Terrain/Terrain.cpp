



///////////////////////////////////////////////////// Added FrameBuffer for Terrain ///////////////////////////////////



#include<Windows.h>
#include<stdio.h>
#include<GL/glew.h>
#include<gl/GL.h>

#include"vmath.h"
#include"ktx_loading_header.h"
#include"02_TEXTURE_SMILEY.h"

using namespace vmath;

#define WIN_WIDTH 1920
#define WIN_HEIGHT 1080

#pragma comment(lib,"opengl32.lib")
#pragma comment(lib,"glew32.lib")

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
GLuint gTessellationControlShaderObject;
GLuint gTessellationEvaluationShaderObject;
GLuint gFragmentShaderObject;
GLuint gShaderProgramObject;

GLuint gVao;
GLuint gVbo;
GLuint gVbo_Smiley_position;
GLuint gVbo_Smiley_texture;

GLuint gTexture_sampler_uniform;

GLuint gMVPUniform;
GLuint mv_matrix;
GLuint projection_matrix;
GLuint gTextureSmiley;

bool enable_displacement;
mat4 gPerspectiveProjectionMatrix;
float dmap_depth = 10.5f;
GLuint enable_Fog;
bool enable_fog;

float gWidth;
float gHeight;


//Terrain FrameBuffer
GLuint TerrainFBO, TerrainColorBuffer, TerrainDepthBuffer, textureUniformTerrain;

GLuint  texture;
GLuint tex_src;
GLuint tex_src2;

GLuint tex_displacement;
GLuint tex_color;

double current_time;

FILE *gpFile = NULL;



//////////////////Lights (Lamp) global


//cube
GLuint gVao_CubeLamp;
GLuint gVbo_CubeLampPosition;
GLuint gVbo_CubeLampNormal;

GLuint gVertexShaderObjectLightLamp;
GLuint gFragmentShaderObjectLightLamp;
GLuint gShaderProgramObjectLightLamp;

GLuint model_matrix_uniformLightsLamp, view_matrix_uniformLightsLamp, projection_matrix_uniformLightsLamp;


GLuint La_uniform;
GLuint Ld_uniform;
GLuint Ls_uniform;
GLuint LightPosUniform;
GLuint kaUniform;
GLuint kdUniform;
GLuint ksUniform;
GLuint gLightPosUniform;
GLuint MaterialShininessUniform;

//////////////////// Framebuffer global Lamp
GLuint GodRaysFBO;
GLuint colorBuffer;
GLuint FinalcolorBuffer;
GLuint FinalFBO;
GLuint rboDepth;

////////////////////// God rays global

GLuint gVertexShaderObjectGodRays;
GLuint gFragmentShaderObjectGodRays;
GLuint gShaderProgramObjectGodRays;

GLuint gTexture_sampler_uniformGodRays;
GLuint gLightPositionGodRays;
GLuint gTexture_sampler_uniformTerrain;
//quad
GLuint gVao_SquareGodRays;
GLuint gVboPosSquareGodRays;
GLuint gVboTexSquareGodRays;
//quad 2
GLuint gVao_QuadGodRays;
GLuint gVboPosQuadGodRays;
GLuint gVboTexQuadGodRays;

////////////////// Final Shader Global

GLuint gVertexShaderObjectTexture;
GLuint gFragmentShaderObjectTexture;
GLuint gShaderProgramObjectTexture;


////////////////// Final Shader Global

GLuint gVertexShaderObjectTextureTerrain;
GLuint gFragmentShaderObjectTextureTerrain;
GLuint gShaderProgramObjectTextureTerrain;
GLuint gTexture_sampler_uniformTexture;
GLuint gTexture_sampler1_uniformTexture;
//quad 2
GLuint gVao_QuadTerrain;
GLuint gVboPosQuadTerrain;
GLuint gVboTexQuadTerrain;


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
		TEXT("OpenGL Programmable PipeLine Native Windowing : Textures Smiley"),
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

	int LoadGLTextures(GLuint *texture, TCHAR imageResourceID[]);

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

	glClearColor(0.0f, 0.25f, 0.0, 0.0f);
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
		"#version 450 core" \
		"\n" \

		"out VS_OUT"\
		"{"\
		" vec2 tc;"\
		"}vs_out;"\

		//"out vec2 vtc;"\
		
		"void main(void)" \
		"{" \
		"  const vec4 vertices[] = vec4[](vec4(-0.5, 0.0, -0.5, 1.0),"\
		"vec4(0.5, 0.0, -0.5, 1.0),"\
		"vec4(-0.5, 0.0, 0.5, 1.0),"\
		"vec4(0.5, 0.0, 0.5, 1.0)); "\
		"int x = gl_InstanceID & 63;"\
		"int y = gl_InstanceID >> 6;"\
		"vec2 offs = vec2(x,y);"\
		"vs_out.tc = (vertices[gl_VertexID].xz + offs + vec2(0.5)) / 64.0;"\
		"gl_Position = vertices[gl_VertexID] + vec4(float(x-32),0.0,float(y-32),0.0);"\

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


	//create shader
	gTessellationControlShaderObject = glCreateShader(GL_TESS_CONTROL_SHADER);

	//provide source code to shader
	const GLchar *tessellationControlShaderSourceCode =
		"#version 450 core" \
		"\n" \
		"layout(vertices=4)out;" \

		"in VS_OUT"\
		"{"\
		"vec2 tc;"\
		"} tcs_in[];"\

		"out TCS_OUT"\
		"{"\
		"vec2 tc;"\
		"} tcs_out[];"\

		"uniform mat4 u_mvp_matrix;"\


		"void main(void)"\
		"{"\
		"if (gl_InvocationID == 0)"\
		"{"\
		"vec4 p0 = u_mvp_matrix * gl_in[0].gl_Position;"\
		"vec4 p1 = u_mvp_matrix * gl_in[1].gl_Position;"\
		"vec4 p2 = u_mvp_matrix * gl_in[2].gl_Position;"\
		"vec4 p3 = u_mvp_matrix * gl_in[3].gl_Position;"\
		"p0 /= p0.w;"\
		"p1 /= p1.w;"\
		"p2 /= p2.w;"\
		"p3 /= p3.w;"\
		"if (p0.z <= 0.0 ||"\
		"p1.z <= 0.0 ||"\
		"p2.z <= 0.0 ||"\
		"p3.z <= 0.0)"\
		"{"\
		"gl_TessLevelOuter[0] = 0.0;"\
		"gl_TessLevelOuter[1] = 0.0;"\
		"gl_TessLevelOuter[2] = 0.0;"\
		"gl_TessLevelOuter[3] = 0.0;"\
		"}"\
		"else"\
		"{"\
		"float l0 = length(p2.xy - p0.xy) * 16.0 + 1.0;"\
		"float l1 = length(p3.xy - p2.xy) * 16.0 + 1.0;"\
		"float l2 = length(p3.xy - p1.xy) * 16.0 + 1.0;"\
		"float l3 = length(p1.xy - p0.xy) * 16.0 + 1.0;"\
		"gl_TessLevelOuter[0] = l0;"\
		"gl_TessLevelOuter[1] = l1;"\
		"gl_TessLevelOuter[2] = l2;"\
		"gl_TessLevelOuter[3] = l3;"\
		"gl_TessLevelInner[0] = min(l1, l3);"\
		"gl_TessLevelInner[1] = min(l0, l2);"\
		"}"\
		"}"\
		"gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;"\
		"tcs_out[gl_InvocationID].tc = tcs_in[gl_InvocationID].tc;"\
		"}"; \

		glShaderSource(gTessellationControlShaderObject, 1, (const GLchar**)&tessellationControlShaderSourceCode, NULL);
	//compile shader
	glCompileShader(gTessellationControlShaderObject);
	glGetShaderiv(gTessellationControlShaderObject, GL_COMPILE_STATUS, &iShaderCompiledStatus);
	if (iShaderCompiledStatus == GL_FALSE)
	{
		glGetShaderiv(gTessellationControlShaderObject, GL_INFO_LOG_LENGTH, &iShaderCompiledStatus);
		if (iInfoLength > 0)
		{
			szInfoLog = (char *)malloc(iInfoLength);
			if (szInfoLog != NULL)
			{
				GLsizei written;
				glGetShaderInfoLog(gTessellationControlShaderObject, iInfoLength, &written, szInfoLog);
				fprintf(gpFile, "Tessellation control Shader Compilation Log : %s\n", szInfoLog);
				free(szInfoLog);
				uninitialize();
				exit(0);
			}
		}
	}




	///*******TESSELLATION EVALUATION SHADER ***
	gTessellationEvaluationShaderObject = glCreateShader(GL_TESS_EVALUATION_SHADER);
	const char* tessellationEvaluationShaderSourceCode =
		"#version 450 core" \
		"\n" \
		"layout(quads, fractional_odd_spacing) in;"\

		"uniform sampler2D tex_displacement;"\
		"uniform float dmap_depth;"\

		"uniform mat4 mv_matrix;"\
		"uniform mat4 proj_matrix;"\

		"in TCS_OUT"\
		"{"\
		"vec2 tc;"\
		"} tes_in[];"\

		"out TES_OUT"\
		"{"
		"vec2 tc;"\
		"vec3 world_coord;"\
		"vec3 eye_coord;"\
		"} tes_out;"\

		"void main(void)"\
		"{"\
		"vec2 tc1 = mix(tes_in[0].tc, tes_in[1].tc, gl_TessCoord.x);"\
		"vec2 tc2 = mix(tes_in[2].tc, tes_in[3].tc, gl_TessCoord.x);"\
		"vec2 tc = mix(tc2, tc1, gl_TessCoord.y);"\

		"vec4 p1 = mix(gl_in[0].gl_Position, gl_in[1].gl_Position, gl_TessCoord.x);"\
		"vec4 p2 = mix(gl_in[2].gl_Position, gl_in[3].gl_Position, gl_TessCoord.x);"\
		"vec4 p = mix(p2, p1, gl_TessCoord.y);"\
		"p.y += texture(tex_displacement, tc).r * 1.1;"\

		"vec4 P_eye = mv_matrix * p;"\

		"tes_out.tc = tc;"\
		"tes_out.world_coord = p.xyz;"\
		"tes_out.eye_coord = P_eye.xyz;"\

		"gl_Position = proj_matrix * P_eye;"\
		"}";



	glShaderSource(gTessellationEvaluationShaderObject, 1, (const GLchar**)&tessellationEvaluationShaderSourceCode, NULL);

	//compile shader
	glCompileShader(gTessellationEvaluationShaderObject);
	glGetShaderiv(gTessellationEvaluationShaderObject, GL_COMPILE_STATUS, &iShaderCompiledStatus);
	if (iShaderCompiledStatus == GL_FALSE)
	{
		glGetShaderiv(gTessellationEvaluationShaderObject, GL_INFO_LOG_LENGTH, &iShaderCompiledStatus);
		if (iInfoLength > 0)
		{
			szInfoLog = (char *)malloc(iInfoLength);
			if (szInfoLog != NULL)
			{
				GLsizei written;
				glGetShaderInfoLog(gTessellationEvaluationShaderObject, iInfoLength, &written, szInfoLog);
				fprintf(gpFile, "Tessellation Evaluation Shader Compilation Log : %s\n", szInfoLog);
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
		"#version 450 core" \
		"\n" \

		"out vec4 color;"\

		"layout (binding = 1) uniform sampler2D tex_color;" \

		"uniform bool enable_fog = true;"\
		"uniform vec4 fog_color = vec4(0.7, 0.8, 0.9, 0.0);" \
		" " \
		"in TES_OUT {" \
		"	vec2 tc;" \
		"	vec3 world_coord;" \
		"	vec3 eye_coord;" \
		"} fs_in;" \
		"vec4 fog(vec4 c)"\
		"{"\
		"	float z = length(fs_in.eye_coord);" \
		"	" \
		"	float de = 0.025 * smoothstep(0.0, 6.0, 10.0 - fs_in.world_coord.y);" \
		"	float di = 0.045 * smoothstep(0.0, 40.0, 20.0 - fs_in.world_coord.y);" \
		"	" \
		"	float extinction = exp(-z * de);" \
		"	float inscattering = exp(-z * di);" \
		"	" \
		"	return c * extinction + fog_color * (1.0 - inscattering);" \
		"}"\

		"void main(void)"\
		"{"\

			"vec4 landscape = texture(tex_color, fs_in.tc*0.5);"\

			//"vec4 landscape = vec4(1,1,1,1);"\

			"if (enable_fog)"\
			"{"
				"color = fog(landscape);"\
			"}"\
			"else"\
			"{"\
				"color = landscape;"\
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

	//attach tessellationControl shader to shader program
	glAttachShader(gShaderProgramObject, gTessellationControlShaderObject);

	//attach tessellationevaluation shader to shader program
	glAttachShader(gShaderProgramObject, gTessellationEvaluationShaderObject);

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

	gMVPUniform = glGetUniformLocation(gShaderProgramObject, "u_mvp_matrix");

	//gTexture_sampler_uniform = glGetUniformLocation(gShaderProgramObject, "u_texture0_sampler");

	mv_matrix = glGetUniformLocation(gShaderProgramObject, "mv_matrix");

	projection_matrix = glGetUniformLocation(gShaderProgramObject, "proj_matrix");

	tex_displacement = glGetUniformLocation(gShaderProgramObject, "tex_displacement");

	tex_color = glGetUniformLocation(gShaderProgramObject, "tex_color");

	dmap_depth = glGetUniformLocation(gShaderProgramObject, "dmap_depth");



	//VAO SMILEY
	const float vertices[] = {
		-8.0f, -8.0f,
		-0.9f, 8.0f,
		 0.9f,-8.0f,
		 8.0f, 8.0f
	};

	// create vao
	glGenVertexArrays(1, &gVao);
	glBindVertexArray(gVao);

	// vertex positions
	glGenBuffers(1, &gVbo);
	glBindBuffer(GL_ARRAY_BUFFER, gVbo);
	glBufferData(GL_ARRAY_BUFFER, 8 * sizeof(float), vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(VDG_ATTRIBUTE_VERTEX, 2, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(VDG_ATTRIBUTE_VERTEX);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0);








	/////////////////////////////////////////////////////////////////// Lamp (God Rays) //////////////////////////////////////////



	/////////////////////////////////////////////////////////// 1. Light Shader //////////////////////////////////



	gVertexShaderObjectLightLamp = glCreateShader(GL_VERTEX_SHADER);

	const GLchar *vertexShaderSourceCodeLightLamp =
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

		"}";


	glShaderSource(gVertexShaderObjectLightLamp, 1, (const GLchar **)&vertexShaderSourceCodeLightLamp, NULL);
	glCompileShader(gVertexShaderObjectLightLamp);

	iShaderCompiledStatus = 0;
	iInfoLength = 0;
	szInfoLog = NULL;

	glGetShaderiv(gVertexShaderObjectLightLamp, GL_COMPILE_STATUS, &iShaderCompiledStatus);
	if (iShaderCompiledStatus == GL_FALSE)
	{
		glGetShaderiv(gVertexShaderObjectLightLamp, GL_INFO_LOG_LENGTH, &iInfoLength);
		if (iInfoLength > 0)
		{
			szInfoLog = (GLchar *)malloc(iInfoLength);
			if (szInfoLog != NULL)
			{
				GLsizei written;
				glGetShaderInfoLog(gVertexShaderObjectLightLamp, GL_INFO_LOG_LENGTH, &written, szInfoLog);

				fprintf(gpFile, "Vertex Shader Compiler Info Log: %s", szInfoLog);
				free(szInfoLog);
				uninitialize();
				DestroyWindow(ghwnd);
			}
		}
	}

	gFragmentShaderObjectLightLamp = glCreateShader(GL_FRAGMENT_SHADER);

	// fragment shader source code
	const GLchar *fragmentShaderSourceCodeLightLamp =
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

		"#version 450 core" \
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

		//"FragColor =  vec4(phong_ads_color,1.0f);" \

		"FragColor =  vec4(1.0f);" \

		"}";

	glShaderSource(gFragmentShaderObjectLightLamp, 1, (const GLchar **)&fragmentShaderSourceCodeLightLamp, NULL);

	glCompileShader(gFragmentShaderObjectLightLamp);

	iShaderCompiledStatus = 0;
	iInfoLength = 0;
	szInfoLog = NULL;

	glGetShaderiv(gFragmentShaderObjectLightLamp, GL_COMPILE_STATUS, &iShaderCompiledStatus);
	if (iShaderCompiledStatus == GL_FALSE)
	{
		glGetShaderiv(gFragmentShaderObjectLightLamp, GL_INFO_LOG_LENGTH, &iInfoLength);
		if (iInfoLength > 0)
		{
			szInfoLog = (GLchar *)malloc(iInfoLength);
			if (szInfoLog != NULL)
			{
				GLsizei written;
				glGetShaderInfoLog(gFragmentShaderObjectLightLamp, GL_INFO_LOG_LENGTH, &written, szInfoLog);

				fprintf(gpFile, "Fragment Shader Compiler Info Log: %s", szInfoLog);
				free(szInfoLog);
				uninitialize();
				DestroyWindow(ghwnd);
			}
		}
	}

	gShaderProgramObjectLightLamp = glCreateProgram();

	glAttachShader(gShaderProgramObjectLightLamp, gVertexShaderObjectLightLamp);

	glAttachShader(gShaderProgramObjectLightLamp, gFragmentShaderObjectLightLamp);

	glBindAttribLocation(gShaderProgramObjectLightLamp, VDG_ATTRIBUTE_VERTEX, "vPosition");
	glBindAttribLocation(gShaderProgramObjectLightLamp, VDG_ATTRIBUTE_NORMAL, "vNormal");

	glLinkProgram(gShaderProgramObjectLightLamp);

	int iProgramLinkStatus = 0;

	glGetProgramiv(gShaderProgramObjectLightLamp, GL_LINK_STATUS, &iProgramLinkStatus);
	if (iProgramLinkStatus == GL_FALSE)
	{
		glGetProgramiv(gShaderProgramObjectLightLamp, GL_INFO_LOG_LENGTH, &iInfoLength);
		if (iInfoLength > 0)
		{
			szInfoLog = (GLchar *)malloc(iInfoLength);
			if (szInfoLog != NULL)
			{
				GLsizei written;
				glGetProgramInfoLog(gShaderProgramObjectLightLamp, GL_INFO_LOG_LENGTH, &written, szInfoLog);

				fprintf(gpFile, ("Shader Program Linking Info Log: %s"), szInfoLog);
				free(szInfoLog);
				uninitialize();
				DestroyWindow(ghwnd);
			}
		}
	}

	model_matrix_uniformLightsLamp = glGetUniformLocation(gShaderProgramObjectLightLamp, "u_model_matrix");
	view_matrix_uniformLightsLamp = glGetUniformLocation(gShaderProgramObjectLightLamp, "u_view_matrix");
	projection_matrix_uniformLightsLamp = glGetUniformLocation(gShaderProgramObjectLightLamp, "u_projection_matrix");
	La_uniform = glGetUniformLocation(gShaderProgramObjectLightLamp, "u_La");
	Ld_uniform = glGetUniformLocation(gShaderProgramObjectLightLamp, "u_Ld");
	Ls_uniform = glGetUniformLocation(gShaderProgramObjectLightLamp, "u_Ls");
	LightPosUniform = glGetUniformLocation(gShaderProgramObjectLightLamp, "u_light_position");

	kaUniform = glGetUniformLocation(gShaderProgramObjectLightLamp, "u_Ka");
	kdUniform = glGetUniformLocation(gShaderProgramObjectLightLamp, "u_Kd");

	ksUniform = glGetUniformLocation(gShaderProgramObjectLightLamp, "u_Ks");

	MaterialShininessUniform = glGetUniformLocation(gShaderProgramObjectLightLamp, "u_material_shininess");

	/////////////////////////////////////////////////////////////////////////////////////////////////////////



	///////////////////////////////////////////////// CUBE


	const GLfloat cubeVertices[] =
	{

		0.5f, 1.0f, -1.0f,
		-0.5f, 1.0f, -1.0f,
		-1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,

		//bottom
		1.0f, -1.0f, 1.0f,
		-1.0f, -1.0f, 1.0f,
		-1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,

		//front
		0.5f, 1.0f, 1.0f,
		-0.5f, 1.0f, 1.0f,
		-1.0f, -1.0f, 1.0f,
		1.0f, -1.0f, 1.0f,

		//back 
		0.5f, 1.0f, -1.0f,
		-0.5f, 1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,

		//right
		0.5f, 1.0f, -1.0f,
		0.5f, 1.0f, 1.0f,
		1.0f, -1.0f, 1.0f,
		1.0f, -1.0f, -1.0f,

		//left
		-0.5f, 1.0f, 1.0f,
		-0.5f, 1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f, 1.0f,

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

	glGenVertexArrays(1, &gVao_CubeLamp);
	glBindVertexArray(gVao_CubeLamp);

	// position vbo
	glGenBuffers(1, &gVbo_CubeLampPosition);
	glBindBuffer(GL_ARRAY_BUFFER, gVbo_CubeLampPosition);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);

	glVertexAttribPointer(VDG_ATTRIBUTE_VERTEX, 3, GL_FLOAT, GL_FALSE, 0, NULL);

	glEnableVertexAttribArray(VDG_ATTRIBUTE_VERTEX);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// normal vbo
	glGenBuffers(1, &gVbo_CubeLampNormal);
	glBindBuffer(GL_ARRAY_BUFFER, gVbo_CubeLampNormal);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cubeNormals), cubeNormals, GL_STATIC_DRAW);

	glVertexAttribPointer(VDG_ATTRIBUTE_NORMAL, 3, GL_FLOAT, GL_FALSE, 0, NULL);

	glEnableVertexAttribArray(VDG_ATTRIBUTE_NORMAL);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0);
	fprintf(gpFile, "After all vertices and normals..\n");

	/////////////////////////////////////GodRays Pass//////////////////////////////////////////////////


	gVertexShaderObjectGodRays = glCreateShader(GL_VERTEX_SHADER);

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

	glShaderSource(gVertexShaderObjectGodRays, 1, (const GLchar **)&vertexShaderShaderCodeGodRays, NULL);

	glCompileShader(gVertexShaderObjectGodRays);

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

	gFragmentShaderObjectGodRays = glCreateShader(GL_FRAGMENT_SHADER);

	const GLchar *fragmentShaderSourceCodeGodRays =
		"#version 450 core" \
		"\n" \
		"                                                                                                                     " \
		"in vec2 out_TexCoords;                                                                                               " \
		"                                                                                                                     " \
		"uniform sampler2D u_texture_sampler2D;                                                                                           " \
		"uniform vec4  u_light_position;                                                                                      " \
		"                                                                                                                     " \
		"out vec4 FragColor;                                                                                                  " \
		"void main(void)                                                                                                      " \
		"{                                                                                                                    " \
		"vec2 deltaTexCoords = vec2(out_TexCoords-u_light_position.xy);"\
		"vec3 finalColor = vec3(texture(u_texture_sampler2D,out_TexCoords));"\
		"vec2 texCoo = out_TexCoords.st;"\
		"float exposure = 1.9;"\
		"float decay = 1.0;"\
		"float density = 0.480;"\
		"float weight = 0.025;"\
		"float illuminationDecay = 1.0;"\
		"deltaTexCoords *= 1.0/150.0*density;"\

		"for(int i=0;i<150;i++)"\
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

	gLightPositionGodRays = glGetUniformLocation(gShaderProgramObjectGodRays, "u_light_position");
	gTexture_sampler_uniformGodRays = glGetUniformLocation(gShaderProgramObjectGodRays, "u_texture_sampler2D");


	const GLfloat Coords[] =
	{
		-1.0f,-1.0f,0.0f,
		-1.0f,1.0f,0.0f,
		 1.0f,1.0f,0.0f,
		 1.0f,-1.0f,0.0f
	};

	const GLfloat TexCorrds[] =
	{
		0.0f,0.0f,
		0.0f,1.0f,
		1.0f,1.0f,
		1.0f,0.0f
	};

	//VAO SMILEY
	glGenVertexArrays(1, &gVao_SquareGodRays);
	glBindVertexArray(gVao_SquareGodRays);

	glGenBuffers(1, &gVboPosSquareGodRays);
	glBindBuffer(GL_ARRAY_BUFFER, gVboPosSquareGodRays);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Coords), Coords, GL_STATIC_DRAW);
	glVertexAttribPointer(VDG_ATTRIBUTE_VERTEX, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(VDG_ATTRIBUTE_VERTEX);

	glBindBuffer(GL_ARRAY_BUFFER, 0);


	glGenBuffers(1, &gVboTexSquareGodRays);
	glBindBuffer(GL_ARRAY_BUFFER, gVboTexSquareGodRays);
	glBufferData(GL_ARRAY_BUFFER, sizeof(TexCorrds), TexCorrds, GL_STATIC_DRAW);
	glVertexAttribPointer(VDG_ATTRIBUTE_TEXTURE0, 2, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(VDG_ATTRIBUTE_TEXTURE0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0);
	//makeSphere(0.5, 40, 40);

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
		"#version 450 core" \
		"\n" \
		"in vec2 out_texture0_coord;" \
		"out vec4 FragColor;" \
		"uniform sampler2D u_texture0_sampler;" \
		"uniform sampler2D u_texture1_sampler;" \
		"uniform sampler2D u_texture2_sampler;"\

		"void main(void)" \
		"{" \
		"vec4 final = texture(u_texture0_sampler,out_texture0_coord);" \
		//"final += texture(u_texture1_sampler,out_texture0_coord);" \
		//"final += texture(u_texture2_sampler,out_texture0_coord);" \

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
	textureUniformTerrain = glGetUniformLocation(gShaderProgramObjectTexture, "u_texture2_sampler");




	glGenVertexArrays(1, &gVao_SquareGodRays);
	glBindVertexArray(gVao_SquareGodRays);

	glGenBuffers(1, &gVboPosSquareGodRays);
	glBindBuffer(GL_ARRAY_BUFFER, gVboPosSquareGodRays);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Coords), Coords, GL_STATIC_DRAW);
	glVertexAttribPointer(VDG_ATTRIBUTE_VERTEX, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(VDG_ATTRIBUTE_VERTEX);

	glBindBuffer(GL_ARRAY_BUFFER, 0);


	glGenBuffers(1, &gVboTexSquareGodRays);
	glBindBuffer(GL_ARRAY_BUFFER, gVboTexSquareGodRays);
	glBufferData(GL_ARRAY_BUFFER, sizeof(TexCorrds), TexCorrds, GL_STATIC_DRAW);
	glVertexAttribPointer(VDG_ATTRIBUTE_TEXTURE0, 2, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(VDG_ATTRIBUTE_TEXTURE0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0);







	///////////////////////////////////////////////////Texture Terrain//////////////////////



		//*****VERTEX SHADER*******
	gVertexShaderObjectTextureTerrain = glCreateShader(GL_VERTEX_SHADER);

	//provide source code to shader
	const GLchar * vertexShaderCodeTextureTerrain =
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

	glShaderSource(gVertexShaderObjectTextureTerrain, 1, (const GLchar**)&vertexShaderCodeTextureTerrain, NULL);

	//compile shader
	glCompileShader(gVertexShaderObjectTextureTerrain);

	iInfoLength = 0;
	iShaderCompiledStatus = 0;
	szInfoLog = NULL;

	glGetShaderiv(gVertexShaderObjectTextureTerrain, GL_COMPILE_STATUS, &iShaderCompiledStatus);
	if (iShaderCompiledStatus == GL_FALSE)
	{
		glGetShaderiv(gVertexShaderObjectTextureTerrain, GL_INFO_LOG_LENGTH, &iInfoLength);
		if (iInfoLength > 0)
		{
			szInfoLog = (char *)malloc(iInfoLength);
			if (szInfoLog != NULL)
			{
				GLsizei written;
				glGetShaderInfoLog(gVertexShaderObjectTextureTerrain, iInfoLength, &written, szInfoLog);
				fprintf(gpFile, "Vertex Shader Compilation Log:%s\n", szInfoLog);
				free(szInfoLog);
				uninitialize();
				exit(0);
			}
		}
	}





	//*****FRAGMENT SHADER****
	gFragmentShaderObjectTextureTerrain = glCreateShader(GL_FRAGMENT_SHADER);

	//provide source code to shader
	const GLchar *fragmentShaderSourceCodeTextureTerrain =
		"#version 450 core" \
		"\n" \
		"in vec2 out_texture0_coord;" \
		"out vec4 FragColor;" \
		"uniform sampler2D u_texture0_sampler;" \

		"void main(void)" \
		"{" \
			"vec4 final = texture(u_texture0_sampler,out_texture0_coord);" \
			"FragColor = final;"\
		"}";


	glShaderSource(gFragmentShaderObjectTextureTerrain, 1, (const GLchar**)&fragmentShaderSourceCodeTextureTerrain, NULL);

	//compile shader
	glCompileShader(gFragmentShaderObjectTextureTerrain);
	glGetShaderiv(gFragmentShaderObjectTextureTerrain, GL_COMPILE_STATUS, &iShaderCompiledStatus);
	if (iShaderCompiledStatus == GL_FALSE)
	{
		glGetShaderiv(gFragmentShaderObjectTextureTerrain, GL_INFO_LOG_LENGTH, &iShaderCompiledStatus);
		if (iInfoLength > 0)
		{
			szInfoLog = (char *)malloc(iInfoLength);
			if (szInfoLog != NULL)
			{
				GLsizei written;
				glGetShaderInfoLog(gFragmentShaderObjectTextureTerrain, iInfoLength, &written, szInfoLog);
				fprintf(gpFile, "Fragment shader compilation Log %s\n", szInfoLog);
				free(szInfoLog);
				uninitialize();
				exit(0);
			}
		}
	}


	//*****SHADER PROGRAM******
	//create shader program
	gShaderProgramObjectTextureTerrain = glCreateProgram();

	//attach vertex shader to shader program
	glAttachShader(gShaderProgramObjectTextureTerrain, gVertexShaderObjectTextureTerrain);

	//attach fragment shader to shader program
	glAttachShader(gShaderProgramObjectTextureTerrain, gFragmentShaderObjectTextureTerrain);

	glBindAttribLocation(gShaderProgramObjectTextureTerrain, VDG_ATTRIBUTE_VERTEX, "vPosition");

	glBindAttribLocation(gShaderProgramObjectTextureTerrain, VDG_ATTRIBUTE_TEXTURE0, "vTexture0_coord");

	//link shader
	glLinkProgram(gShaderProgramObjectTextureTerrain);

	iShaderProgramLinkStatus = 0;
	glGetProgramiv(gShaderProgramObjectTextureTerrain, GL_LINK_STATUS, &iShaderCompiledStatus);
	if (iShaderCompiledStatus == GL_FALSE)
	{
		glGetProgramiv(gShaderProgramObjectTextureTerrain, GL_INFO_LOG_LENGTH, &iInfoLength);
		if (iInfoLength > 0)
		{
			szInfoLog = (char*)malloc(iInfoLength);
			if (szInfoLog != NULL)
			{
				GLsizei written;
				glGetProgramInfoLog(gShaderProgramObjectTextureTerrain, iInfoLength, &written, szInfoLog);
				fprintf(gpFile, "Shader Program Link Status %s\n", szInfoLog);
				free(szInfoLog);
				uninitialize();
				exit(0);
			}
		}
	}

	// post-linking retrieving uniform locations
	gTexture_sampler_uniformTerrain = glGetUniformLocation(gShaderProgramObjectTextureTerrain, "u_texture0_sampler");

	const GLfloat CoordsQuad[] =
	{
		-1.0f,-1.0f,0.0f,
		-1.0f,1.0f,0.0f,
		 1.0f,1.0f,0.0f,
		 1.0f,-1.0f,0.0f
	};

	const GLfloat TexCorrdsQuad[] =
	{
		0.0f,0.0f,
		0.0f,1.0f,
		1.0f,1.0f,
		1.0f,0.0f
	};

	//VAO SMILEY
	glGenVertexArrays(1, &gVao_QuadTerrain);
	glBindVertexArray(gVao_QuadTerrain);

	glGenBuffers(1, &gVboPosQuadTerrain);
	glBindBuffer(GL_ARRAY_BUFFER, gVboPosQuadTerrain);
	glBufferData(GL_ARRAY_BUFFER, sizeof(CoordsQuad), CoordsQuad, GL_STATIC_DRAW);
	glVertexAttribPointer(VDG_ATTRIBUTE_VERTEX, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(VDG_ATTRIBUTE_VERTEX);

	glBindBuffer(GL_ARRAY_BUFFER, 0);


	glGenBuffers(1, &gVboTexQuadTerrain);
	glBindBuffer(GL_ARRAY_BUFFER, gVboTexQuadTerrain);
	glBufferData(GL_ARRAY_BUFFER, sizeof(TexCorrdsQuad), TexCorrdsQuad, GL_STATIC_DRAW);
	glVertexAttribPointer(VDG_ATTRIBUTE_TEXTURE0, 2, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(VDG_ATTRIBUTE_TEXTURE0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0);

	//////////////////////////////////////////////////////// FRAME BUFFER	//////////////////////////////////////////////////



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


	///////////////////////////////////// Final framebuffer (GodRays)///////////////////////////////////////////////////////////////////


	
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
		GLuint attachmentFinal[] = { GL_COLOR_ATTACHMENT0 };

		glDrawBuffers(2, attachmentFinal);
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			fprintf_s(gpFile, "Texture Framebuffer not complete");
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	//////////////////////////////////////////////////////////////////// Terrain ///////////////////////////////////////////////
	glGenFramebuffers(1, &TerrainFBO);

	glGenTextures(1, &TerrainColorBuffer);

	//normal color
	glBindTexture(GL_TEXTURE_2D, TerrainColorBuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, WIN_WIDTH, WIN_HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);


	glGenRenderbuffers(1, &TerrainDepthBuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, TerrainDepthBuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, WIN_WIDTH, WIN_HEIGHT);

	glBindFramebuffer(GL_FRAMEBUFFER, TerrainFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, TerrainColorBuffer, 0);

	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, TerrainDepthBuffer);

	GLuint attachmentTerrain[] = { GL_COLOR_ATTACHMENT0 };
	glDrawBuffers(2, attachmentTerrain);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		fprintf_s(gpFile, "Terrain Framebuffer not complete");
	glBindFramebuffer(GL_FRAMEBUFFER, 0);


	////// clear the depth buffer
	glClearDepth(1.0f);

	////enable depth
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	
	//// clear the screen by OpenGL
	//glClearColor(0.0f, 0.25f, 0.0f, 1.0f);

	glActiveTexture(GL_TEXTURE0);
	tex_displacement = load("E:/RealTimeRendering2018/Trial_Tessellation_Terrain_Sb_Fog/Trial_Tessellation_Terrain_Sb_Fog/Trial_Tonemap_exposure/02_TEXTURE_SMILEY/terragen1.ktx", 0);
	glActiveTexture(GL_TEXTURE1);
	LoadGLTextures(&tex_color, MAKEINTRESOURCE(IDBITMAP_TERRAIN));
	//tex_color = load("E:/RealTimeRendering2018/Trial_Tessellation_Terrain_Sb_Fog/Trial_Tessellation_Terrain_Sb_Fog/Trial_Tonemap_exposure/02_TEXTURE_SMILEY/terragen_color.ktx", 0);

	glPatchParameteri(GL_PATCH_VERTICES, 4);

	resize(WIN_WIDTH, WIN_HEIGHT);
	fprintf(gpFile, "Exitting Initialize..\n");

}

void display()
{
 glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//
	//static const GLfloat one = 1.0f;
//	static double lastTime = 0.0;
//	static double totalTime = 0.0;
//
	//glClearBufferfv(GL_DEPTH, 0, &one);

	mat4 modelMatrix = mat4::identity();
	mat4 viewMatrix = mat4::identity();
	mat4 projectionMatrix = mat4::identity();
	////
	//vec3 light_position = vec3(
	//	0,
	//	0.5,
	//	-5);
	////
	//modelMatrix = translate(-0.35f, 0.15f, -1.2f);
	//gPerspectiveProjectionMatrix = perspective(60.0f, (GLfloat)gWidth / (GLfloat)gHeight, 0.1f, 1000.0f);
	//projectionMatrix = gPerspectiveProjectionMatrix;

	//glBindFramebuffer(GL_FRAMEBUFFER, GodRaysFBO);
	//glViewport(0, 0, 1920, 1080);

	////
	//	/////////////////////////////////////////////// Light  

	//glUseProgram(gShaderProgramObjectLightLamp);

	//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//glClearBufferfv(GL_DEPTH, 0, &one);

	//glUniformMatrix4fv(model_matrix_uniformLightsLamp, 1, GL_FALSE, modelMatrix);
	//glUniformMatrix4fv(view_matrix_uniformLightsLamp, 1, GL_FALSE, viewMatrix);
	//glUniformMatrix4fv(projection_matrix_uniformLightsLamp, 1, GL_FALSE, projectionMatrix);
	//glUniform3fv(LightPosUniform, 1, light_position);


	//////
	//modelMatrix = modelMatrix * translate(light_position);
	//modelMatrix = modelMatrix * scale(0.19f, 0.2f, 0.0f);
	//glUniformMatrix4fv(model_matrix_uniformLightsLamp, 1, GL_FALSE, modelMatrix);
 //
	//glUniform3fv(kaUniform, 1, vec3(1.0f, 1.0, 1.0f));
	//glUniform3fv(kdUniform, 1, vec3(1.0f, 1.0, 1.0f));
	//glUniform3fv(ksUniform, 1, vec3(1.0f, 1.0, 1.0f));
	//glUniform3fv(La_uniform, 1, vec3(1.0f, 1.0, 1.0f));
	//glUniform3fv(Ld_uniform, 1, vec3(1.0f, 1.0, 1.0f));
	//glUniform3fv(Ls_uniform, 1, vec3(1.0f, 1.0, 1.0f));


	//glUniform1f(MaterialShininessUniform, 128.0f);
	//

	//	glBindVertexArray(gVao_CubeLamp);
	//	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
	//	glDrawArrays(GL_TRIANGLE_FAN, 4, 4);
	//	glDrawArrays(GL_TRIANGLE_FAN, 8, 4);
	//	glDrawArrays(GL_TRIANGLE_FAN, 12, 4);
	//	glDrawArrays(GL_TRIANGLE_FAN, 16, 4);
	//	glDrawArrays(GL_TRIANGLE_FAN, 20, 4);

	//	glBindVertexArray(0);
	//
	//glBindFramebuffer(GL_FRAMEBUFFER, 0);

	//glUseProgram(0);
	////
	////
	//////	/////////////////////////////////////////////// God Rays 
	//////
	//glBindFramebuffer(GL_FRAMEBUFFER, FinalFBO);
	//glUseProgram(gShaderProgramObjectGodRays);

	//glActiveTexture(GL_TEXTURE0);
	//glBindTexture(GL_TEXTURE_2D, colorBuffer);

	//light_position[2] -= 200.0;
	//float lightX = (2.974313*light_position[0]);
	//float lightY = (1.732113*light_position[1]);
	//float lightW = (-1 * light_position[2]);

	//lightY /= lightW;
	//lightX /= lightW;

	//lightX = (lightX + 1.0f)*(0.5);
	//lightY = (lightY + 1.0f)*(0.5f);

	//glUniform4fv(gLightPositionGodRays, 1, vec4(lightX, lightY, 0.0, 0.0));

	//glBindVertexArray(gVao_SquareGodRays);
	//glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
	////drawSphere();
	//glBindVertexArray(0);

	//glUseProgram(0);
	//glBindFramebuffer(GL_FRAMEBUFFER, 0);
	////
	///////////////////////////////////////////////////Texture 
	////
	//glUseProgram(gShaderProgramObjectTexture);


	//glActiveTexture(GL_TEXTURE0);
	//glBindTexture(GL_TEXTURE_2D, FinalcolorBuffer);
	//glUniform1i(gTexture_sampler_uniformTexture, 3);

	//glBindVertexArray(gVao_SquareGodRays);
	//glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
	//glBindVertexArray(0);
	//glBindTexture(GL_TEXTURE_2D, 0);

	//glUseProgram(0);
//
	////////////////////////////////////////////////////////////////////////////////////////////
	static const GLfloat black[] = { 0.85f, 0.95f, 1.0f, 1.0f };
	glBindFramebuffer(GL_FRAMEBUFFER, TerrainFBO);
	glViewport(0, 0, gWidth, gHeight);


	static const GLfloat one = 1.0f;
	static double lastTime = 0.0;
	static double totalTime = 0.0;

	totalTime += (current_time - lastTime);
	lastTime = current_time;
	current_time += 0.0001;

	float t = (float)totalTime * 0.03f;
	float r = sinf(t*5.37f) * 25.0f - 19.0f;
	float h = cosf(t*4.79f) * 6.0f + 3.2f;

	//glClearBufferfv(GL_COLOR, 0, black);
	//glClearBufferfv(GL_COLOR, 0, black);
	glClearBufferfv(GL_DEPTH, 0, &one);


	//start using OpenGL program object
	mat4 modelViewMatrix;
	mat4 rotationMatrix;
	mat4 modelViewProjectionMatrix;
	mat4 proj_matrix;

	modelViewMatrix = mat4::identity();
	modelViewMatrix = translate(0.0f, 0.0f, -3.0f);

	modelViewMatrix = lookat(
		vec3(sinf(t)*r, h, cosf(t)*r),
		vec3(0.0f),
		vec3(0.0f, 1.0f, 0.0f));

	//modelViewMatrix = vmath::lookat(vmath::vec3(sinf(t) * r, h, cosf(t) * r), vmath::vec3(0.0f), vmath::vec3(0.0f, 1.0f, 0.0f));

	proj_matrix = perspective(60.0f, (GLfloat)gWidth / (GLfloat)gHeight, 0.1f, 1000.0f);
	modelViewProjectionMatrix = proj_matrix * modelViewMatrix;
	glUseProgram(gShaderProgramObject);

	glUniformMatrix4fv(gMVPUniform, 1, GL_FALSE, modelViewProjectionMatrix);
	glUniformMatrix4fv(mv_matrix, 1, GL_FALSE, modelViewMatrix);
	glUniformMatrix4fv(projection_matrix, 1, GL_FALSE, proj_matrix);
	glUniform1f(dmap_depth, 1.0f);
	glUniform1f(enable_Fog, enable_fog ? 1 : 0);

	glUniform1i(tex_displacement, 0);
	glUniform1i(tex_color, 1);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, tex_color);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, tex_displacement);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	glBindVertexArray(gVao);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glDrawArraysInstanced(GL_PATCHES, 0, 4, 128 * 128);

	glBindVertexArray(0);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);


	//////////////////////////////////////////////////////////////////////////////////////////

	// unuse program
	glUseProgram(0);



	glUseProgram(gShaderProgramObjectTextureTerrain);


	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, TerrainColorBuffer);
	glUniform1i(gTexture_sampler_uniformTerrain, 0);
	glBindVertexArray(gVao_QuadTerrain);

	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);

	glUseProgram(0);

	SwapBuffers(ghdc);
}

int LoadGLTextures(GLuint *texture,TCHAR imageResourceID[])
{
	fprintf(gpFile, "Inside LoadGLTextures\n");
	HBITMAP hBitmap;
	BITMAP bmp;
	int iStatus = 0;

	glGenTextures(1, texture);
	hBitmap = (HBITMAP)LoadImage(GetModuleHandle(NULL), imageResourceID, IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION);
	{
	if (hBitmap)
		iStatus = TRUE;
		GetObject(hBitmap, sizeof(BITMAP), &bmp);
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		glBindTexture(GL_TEXTURE_2D, *texture);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, bmp.bmWidth, bmp.bmHeight, 0, GL_BGR, GL_UNSIGNED_BYTE, bmp.bmBits);

		glGenerateMipmap(GL_TEXTURE_2D);
		DeleteObject(hBitmap);
	}

	return(iStatus);
}

void resize(int width, int height)
{
	if (height == 0)
		height = 1;
	glViewport(0, 0, (GLsizei)width, (GLsizei)height);
	gWidth = width;
	gHeight = height;


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
	if (gVao)
	{
		glDeleteVertexArrays(1, &gVao);
		gVao = 0;
	}

	if (gVbo_Smiley_position)
	{
		glDeleteBuffers(1, &gVbo_Smiley_position);
		gVbo_Smiley_position = 0;
	}

	if (gVbo_Smiley_texture)
	{
		glDeleteBuffers(1, &gVbo_Smiley_texture);
		gVbo_Smiley_texture = 0;
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

























