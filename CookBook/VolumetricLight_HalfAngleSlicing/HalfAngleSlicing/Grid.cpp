#include<glm.hpp>
#include<GL/glew.h>
#include<stdio.h>
#include"Grid.h"
#include"Common.h"

CGrid::CGrid(int width, int depth)
{
	this->width = width;
	this->depth = depth;

	////////////////////////////////
	///Shader of Textures 
	gVertexShaderObjectBase = glCreateShader(GL_VERTEX_SHADER);

	//provide source code to shader
	const GLchar *vertexShaderSourceCodeTexture =
		"#version 430 core" \
		"\n" \
		"in vec3 vPosition;" \
		"uniform mat4 u_mvp_matrix;" \
		"out vec4 out_Color;" \
		"void main(void)" \
		"{" \
		"gl_Position = u_mvp_matrix * vec4(vPosition.xyz,1);" \
		"}";

	glShaderSource(gVertexShaderObjectBase, 1, (const GLchar **)&vertexShaderSourceCodeTexture, NULL);

	//compile shader
	glCompileShader(gVertexShaderObjectBase);
	GLint iInfoLength = 0;
	GLint iShaderCompiledStatus = 0;
	char *szInfoLog = NULL;

	glGetShaderiv(gVertexShaderObjectBase, GL_COMPILE_STATUS, &iShaderCompiledStatus);
	if (iShaderCompiledStatus == GL_FALSE)
	{
		glGetShaderiv(gVertexShaderObjectBase, GL_INFO_LOG_LENGTH, &iInfoLength);
		if (iInfoLength > 0)
		{
			szInfoLog = (char *)malloc(iInfoLength);
			if (szInfoLog != NULL)
			{
				GLsizei written;
				glGetShaderInfoLog(gVertexShaderObjectBase, iInfoLength, &written, szInfoLog);
				fprintf(gpFile, "Vertex Shader Compilation Log:%s\n", szInfoLog);
				free(szInfoLog);
				//uninitialize();
				exit(0);
			}
		}
	}

	//**********FRAGMENT SHADER*********
	//create shader
	gFragmentShaderObjectBase = glCreateShader(GL_FRAGMENT_SHADER);

	//provide source code to shader
	const GLchar *fragmentShaderSourceCodeTexture =
		"#version 430 core" \
		"\n" \
		"in vec4 out_Color;" \
		"out vec4 FragColor;" \
		"void main (void)" \
		"{" \
		"FragColor = vec4(1,1,1,1);" \
		"}";

	glShaderSource(gFragmentShaderObjectBase, 1, (const GLchar**)&fragmentShaderSourceCodeTexture, NULL);

	//compile shader
	glCompileShader(gFragmentShaderObjectBase);
	glGetShaderiv(gFragmentShaderObjectBase, GL_COMPILE_STATUS, &iShaderCompiledStatus);
	if (iShaderCompiledStatus == GL_FALSE)
	{
		glGetShaderiv(gFragmentShaderObjectBase, GL_INFO_LOG_LENGTH, &iShaderCompiledStatus);
		if (iInfoLength > 0)
		{
			szInfoLog = (char *)malloc(iInfoLength);
			if (szInfoLog != NULL)
			{
				GLsizei written;
				glGetShaderInfoLog(gFragmentShaderObjectBase, iInfoLength, &written, szInfoLog);
				fprintf(gpFile, "Fragment Shader Compilation Log : %s\n", szInfoLog);
				free(szInfoLog);
				//uninitialize();
				exit(0);
			}
		}
	}

	//********** SHADER PROGRAM *********
	//create
	gShaderProgramObjectTexture = glCreateProgram();

	//attach vertex shader to shader program
	glAttachShader(gShaderProgramObjectTexture, gVertexShaderObjectBase);

	//attach fragment shader to shader program
	glAttachShader(gShaderProgramObjectTexture, gFragmentShaderObjectBase);

	//pre-link binding of shader program object with vertex position attribute
	glBindAttribLocation(gShaderProgramObjectTexture, VDG_ATTRIBUTE_VERTEX, "vPosition");


	//Link Shader
	glLinkProgram(gShaderProgramObjectTexture);
	GLint iShaderProgramLinkStatus = 0;
	glGetProgramiv(gShaderProgramObjectTexture, GL_LINK_STATUS, &iShaderProgramLinkStatus);
	if (iShaderProgramLinkStatus == GL_FALSE)
	{
		glGetProgramiv(gShaderProgramObjectTexture, GL_INFO_LOG_LENGTH, &iInfoLength);
		if (iInfoLength > 0)
		{
			szInfoLog = (char *)malloc(iInfoLength);
			if (szInfoLog != NULL)
			{
				GLsizei written;
				glGetProgramInfoLog(gShaderProgramObjectTexture, iInfoLength, &written, szInfoLog);
				fprintf(gpFile, "Shader Program LinK Log Base Shader%s\n");
				free(szInfoLog);
				//uninitialize();
				exit(0);
			}
		}
	}


	//get MVP uniform locaion
	gMVPUniformTexture = glGetUniformLocation(gShaderProgramObjectTexture, "u_mvp_matrix");

	/////////////////////////////
	Init();


}

CGrid::~CGrid(void)
{

}

int CGrid::GetTotalVertices()
{
	return((width + 1) + (depth + 1)) * 2;
}

int CGrid::GetTotalIndices()
{
	return(width*depth);
}



GLenum CGrid::GetPrimitiveType()
{
	return GL_LINES;
}


void CGrid::FillVertexBuffer(GLfloat *pBuffer)
{
	glm::vec3* vertices = (glm::vec3*)(pBuffer);
	int count = 0;
	int width_2 = width / 2;
	int depth_2 = depth / 2;
	int i = 0;

	for (i = -width_2; i <= width_2; i++) {
		vertices[count++] = glm::vec3(i, 0, -depth_2);
		vertices[count++] = glm::vec3(i, 0, depth_2);

		vertices[count++] = glm::vec3(-width_2, 0, i);
		vertices[count++] = glm::vec3(width_2, 0, i);
	}
}


void CGrid::FillIndexBuffer(GLuint *pBuffer)
{
	int i = 0;
	//fill indices array
	GLuint *id = pBuffer;
	for (i = 0; i < width*depth; i += 4)
	{
		*id++ = i;
		*id++ = i + 1;
		*id++ = i + 2;
		*id++ = i + 3;
	}
}