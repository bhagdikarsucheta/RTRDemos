#pragma once
#include<string>
#include<fstream>
#include<sstream>
#include<iostream>
#include<vector>
#include<gl/glew.h>
#include<assimp/Importer.hpp>
#include<assimp/scene.h>
#include<assimp/postprocess.h>
#include"mesh.h"
#include"vmath.h"
#define STB_IMAGE_IMPLEMENTATION
#include"stb_image.h"

using namespace std;
GLuint TextureFromFile(const char *path, string directory);


class Model
{
public:
	Model(GLchar *path)
	{
		this->loadModel(path);
	}

	void Draw(GLuint shaderProgramObject)
	{
		for (GLuint i = 1; i < this->meshes.size(); i++)
		{
			this->meshes[i].Draw(shaderProgramObject);
		}
	}

private:
	vector<Mesh> meshes;
	string directory;
	vector<Texture> textures_loaded;

	void loadModel(string path)
	{
		Assimp::Importer importer;
		const aiScene *scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);

		if (!scene || scene->mFlags == AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
		{
			cout << "Error::Assimp::" << importer.GetErrorString() << endl;
			return;
		}
		this->directory = path.substr(0, path.find_last_of('/'));

		this->processNode(scene->mRootNode, scene);
	}

	void processNode(aiNode*node, const aiScene *scene)
	{
		for (GLuint i = 0; i < node->mNumMeshes; i++)
		{
			aiMesh * mesh = scene->mMeshes[node->mMeshes[i]];

			this->meshes.push_back(this->processMesh(mesh, scene));
		}
		for (GLuint i = 0; i < node->mNumChildren; i++)
		{
			this->processNode(node->mChildren[i], scene);
		}
	}


	Mesh processMesh(aiMesh *mesh, const aiScene *scene)
	{

		vector<Vertex>vertices;
		vector<GLuint>indices;
		vector<Texture>textures;

		for (GLuint i = 0; i < mesh->mNumVertices; i++)
		{
			Vertex vertex;
			vmath::vec3 vector;

			vector[0] = mesh->mVertices[i].x;
			vector[1] = mesh->mVertices[i].y;
			vector[2] = mesh->mVertices[i].z;
			vertex.Position = vector;

			vector[0] = mesh->mNormals[i].x;
			vector[1] = mesh->mNormals[i].y;
			vector[2] = mesh->mNormals[i].z;
			vertex.Normal = vector;

			if (mesh->mTextureCoords[0])
			{
				vmath::vec2 vec;
				vec[0] = mesh->mTextureCoords[0][i].x;
				vec[1] = mesh->mTextureCoords[0][i].y;

				vertex.TexCoords = vec;
			}
			else
			{
				vertex.TexCoords = vmath::vec2(0.0f, 0.0f);
			}
			vertices.push_back(vertex);
		}

		for (GLuint i = 0; i < mesh->mNumFaces; i++)
		{
			aiFace face = mesh->mFaces[i];
			for (GLuint j = 0; j < face.mNumIndices; j++)
			{
				indices.push_back(face.mIndices[j]);
			}
		}
		if (mesh->mMaterialIndex >= 0)
		{
			aiMaterial * material = scene->mMaterials[mesh->mMaterialIndex];

			vector<Texture> diffuseMaps = this->loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
			textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());

			vector<Texture> specularMaps = this->loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
			textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());

		}

		return Mesh(vertices, indices, textures);
	}

	vector<Texture> loadMaterialTextures(aiMaterial *mat, aiTextureType type, string typeName)
	{

		vector<Texture> tectures;

		for (GLuint i = 0; i < mat->GetTextureCount(type); i++)
		{
			aiString str;
			mat->GetTexture(type, i, &str);

			GLboolean skip = false;

			for (GLuint j = 0; j < textures_loaded.size(); j++)
			{
				if (textures_loaded[j].path == str)
				{
					tectures.push_back(textures_loaded[j]);
					skip = true;

					break;
				}
			}

			if (!skip)
			{
				Texture texture;
				texture.id = TextureFromFile(str.C_Str(), this->directory);
				texture.type = typeName;
				texture.path = str;
				tectures.push_back(texture);

				this->textures_loaded.push_back(texture);
			}
		}
		return tectures;
	}
};





GLuint TextureFromFile(const char *path, string directory)
{
	string filename = string(path);
	filename = directory + '/' + filename;

	GLuint textureID;
	glGenTextures(1, &textureID);

	int width, height, nrComponents;
	unsigned char *data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);

	if (data)
	{
		GLenum format;
		if (nrComponents == 1)
			format = GL_RED;
		else if (nrComponents == 3)
			format = GL_RGB;
		else if (nrComponents == 4)
			format = GL_RGBA;



		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);

	}
	else
	{
		//fprintf(gpFile2 , "ERROR:Image Path : %s\n", path);
		stbi_image_free(data);

	}
	return textureID;
}


