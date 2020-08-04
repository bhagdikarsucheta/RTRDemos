
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include"vmath.h"

using namespace std;

enum
{
	VDG_ATTRIBUTE_VERTEX = 0,
	VDG_ATTRIBUTE_NORMAL,
	VDG_ATTRIBUTE_TEXTURE0,
	VDG_ATTRIBUTE_TANGENT,
	VDG_ATTRIBUTE_BITANGENT,
	VDG_ATTRIBUTE_COLOR,
};

/////Mesh Related
struct Vertex {
	vmath::vec3 Position;
	vmath::vec3 Normal;
	vmath::vec2 TexCoords;
	vmath::vec3 Tangent;
	vmath::vec3 Bitangent;

};

struct Texture
{
	GLuint id;
	string type;
	aiString path;
};


class Mesh
{
public:
	vector<Vertex> vertices;
	vector<GLuint>indices;
	vector<Texture>textures;

	//Constructor
	Mesh(vector<Vertex> vertices, vector<GLuint> indices, vector<Texture> textures)
	{
		this->vertices = vertices;
		this->indices = indices;
		this->textures = textures;
		this->setup();
	}


	void Draw(GLuint shaderProgramObject)
	{
		GLuint diffuseNr = 1;
		GLuint specularNr = 1;
		GLuint normalNr = 1;
		GLuint heightNr = 1;

		for (GLuint i = 0; i < this->textures.size(); i++)
		{
			glActiveTexture(GL_TEXTURE0 + i);

			stringstream ss;
			string number;
			string name = this->textures[i].type;

			//if (name == "texture_diffuse")
			//	number = std::to_string(diffuseNr++);
			//else if (name == "texture_specular")
			//	number = std::to_string(specularNr++); // transfer unsigned int to stream
			//else if (name == "texture_normal")
			//	number = std::to_string(normalNr++); // transfer unsigned int to stream
			//else if (name == "texture_height")
			//	number = std::to_string(heightNr++); // transfer unsigned int to stream

			if ("texture_diffuse" == name)
			{
				ss << diffuseNr++;
			}
			else if ("texture_specular" == name)
			{
				ss << specularNr++;
			}

			number = ss.str();

			glUniform1i(glGetUniformLocation(shaderProgramObject, (name + number).c_str()), i);

			glBindTexture(GL_TEXTURE_2D, this->textures[i].id);
		}

		glUniform1f(glGetUniformLocation(shaderProgramObject, "material_shininess"), 16.0f);

		glBindVertexArray(VAO);
		//glDrawElements(GL_TRIANGLES, this->indices.size(), GL_UNSIGNED_INT, 0);
		//glDrawArrays(GL_POINTS, 0, 1000000);
		glDrawArrays(GL_TRIANGLES, 0, (GLsizei)this->vertices.size());
		//glDrawArraysInstanced(GL_TRIANGLES, 0, (GLsizei)this->vertices.size(), 2);
		glBindVertexArray(0);
		
		for (GLuint i = 0; i < this->textures.size(); i++)
		{
			glActiveTexture(GL_TEXTURE0 + i);
			glBindTexture(GL_TEXTURE_2D, 0);
		}

		//glActiveTexture(0);
	}
private:
	GLuint VAO, VBO, EBO;

	void setup()
	{
		glGenVertexArrays(1, &this->VAO);

		glBindVertexArray(this->VAO);
		glGenBuffers(1, &this->VBO);

		glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
		glBufferData(GL_ARRAY_BUFFER, this->vertices.size() * sizeof(Vertex), &this->vertices[0], GL_STATIC_DRAW);

		glGenBuffers(1, &this->EBO);
 
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->indices.size() * sizeof(GLuint), &this->indices[0], GL_STATIC_DRAW);

		//vertices
		glEnableVertexAttribArray(VDG_ATTRIBUTE_VERTEX);
		glVertexAttribPointer(VDG_ATTRIBUTE_VERTEX, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)0);
		//normals
		glEnableVertexAttribArray(VDG_ATTRIBUTE_NORMAL);
		glVertexAttribPointer(VDG_ATTRIBUTE_NORMAL, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, Normal));
		//TexCoords
		glEnableVertexAttribArray(VDG_ATTRIBUTE_TEXTURE0);
		glVertexAttribPointer(VDG_ATTRIBUTE_TEXTURE0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, TexCoords));
		//////Tangents
		//glEnableVertexAttribArray(VDG_ATTRIBUTE_TANGENT);
		//glVertexAttribPointer(VDG_ATTRIBUTE_TANGENT, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, Tangent));
		//////BiTangent
		//glEnableVertexAttribArray(VDG_ATTRIBUTE_BITANGENT);
		//glVertexAttribPointer(VDG_ATTRIBUTE_BITANGENT, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, Bitangent));

		glBindVertexArray(0);

	}
};














