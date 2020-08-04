#include<string.h>
#include<glm.hpp>
#include<vector>
#include<GL/glew.h>

struct Vertex {
	glm::vec3 pos, normal;
};

class TetrahedraMarcher
{
public:
	TetrahedraMarcher(void);
	~TetrahedraMarcher(void);


	//function to set volume dimension
	void SetVolumeDimension(const int xdim, const int ydim, const int zdim);

	//Function to set total number of sampling voxels
	//more volxels will give a higher density mesh
	void SetNumSamplingVoxels(const int x, const int y, const int z);

	//set isosurface valye
	void SetIsoSurfaceVaule(const GLubyte value);


	//load volume dataset
	bool LoadVolume(const std::string& filename);

	//march the volume dataset
	void marchVolume();

	//get the number of vertices generated
	size_t GetTotalVertices();

	//get pointer to vertex buffer
	Vertex * GetVertexPointer();

protected:
	//volume sampling function,give the x,y,z values returns the density value
	//in the volume at that location
	GLubyte SampleVolume(const int x, const int y, const int z);

	//get the normal at given location using center finite difference approximation
	glm::vec3 GetNormal(const int x, const int y, const int z);

	//samples a voxel at given location and scale
	void SampleVoxel(const int x, const int y, const int z, glm::vec3 scale);

	//returns the voxels between two sample values
	float GetOffset(const GLubyte v1, const GLubyte v2);

	//volume dataset dimensions and inverse volume dimensions
	int XDIM, YDIM, ZDIM;
	glm::vec3 invDim;

	//sampling distances in voxels
	int X_SAMPLING_DIST;
	int Y_SAMPLING_DIST;
	int Z_SAMPLING_DIST;

	//volume data pointer
	GLubyte*pVolume;

	//the given isovalue look for
	GLubyte isoValue;

	//vertices vector storing positions and normals
	std::vector<Vertex> vertices;

};