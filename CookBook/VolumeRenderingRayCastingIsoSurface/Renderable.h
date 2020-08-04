#pragma once
#include<GL/glew.h>

class RenderableObject
{
public:
	GLuint mvpUniform;
	RenderableObject(void);
	virtual ~RenderableObject(void);
	void Render(const GLfloat* MVP );

	virtual int GetTotalVertices() = 0;
	virtual int GetTotalIndices() = 0;
	virtual GLenum GetPrimitiveType() = 0;

	virtual void FillVertexBuffer(GLfloat* pBuffer) = 0;
	virtual void FillIndexBuffer(GLuint *pBuffer) = 0;


	void Init();
	void Destroy();
	GLuint gVertexShaderObjectBase, gFragmentShaderObjectBase, gShaderProgramObjectTexture, gMVPUniformTexture;

protected:
	GLuint vaoID;
	GLuint vboVerticesID;
	GLuint vboIndicesID;
	
	GLenum primType;
	int totalVertices, totalIndices;
};