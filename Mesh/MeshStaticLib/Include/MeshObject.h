#pragma once

#include <string>
#include <Common.h>

#define TEXTURESUM 50

class MyMesh
#ifndef _M_CEE
	: public OpenMesh::TriMesh_ArrayKernelT<>
#endif
{
public:
	MyMesh();
	~MyMesh();
};

class GLMesh
{
public:
	GLMesh();
	~GLMesh();

	bool Init(std::string fileName);
	void Render();
	void LoadTexCoordToShader(int layer);

	std::vector<MyMesh> meshList;
	std::vector<GLuint> vaoList;
	std::vector<GLuint> eboList;
	std::vector<GLuint> vboVList;
	std::vector<GLuint> vboNList;
	std::vector<GLuint> vboTList;

private:

	bool LoadModel(std::string fileName, int layer);
	void LoadToShader(int layer);
};

class MeshObject
{
public:
	MeshObject();
	~MeshObject();

	bool Init(std::string fileName);
	void Render();
	void RenderSelectedFace();

	void RenderSelectedFace(int layer);
	bool AddSelectedFace(unsigned int faceID, int layer);
	void Parameterization(float uvRotateAngle, int layer);
	void DeleteSelectedFace(unsigned int faceID, int layer);
	void MoveSelectedFace(unsigned int faceID, int layer);
	void RenderParameterized(int layer);

	bool WriteFile();
	bool ReadFile();

	void Scale(int layer);
	void Shrink(int layer);
	bool ReadMeshFile(int layer);

	bool CheckSelectFaceListSize(int layer);
	void scaleSelectedFace(unsigned int faceID, bool ScaleUp);
private:
	GLMesh model;

	std::vector<std::vector<unsigned int> > selectedFaceList;
	std::vector<std::vector<unsigned int*> > faceIDsPtrList;
	std::vector<std::vector<int> > elementCountList;

	void CopySelectFace(MyMesh& model, int layer);

	bool WriteMeshFile(int layer);
	
};

