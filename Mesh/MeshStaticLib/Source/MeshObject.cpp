#include "pch.h"
#include "MeshObject.h"
#include <fstream>
#include <Eigen/Sparse>
#include <map>
using namespace std;
#define Quad
//#define Harmonic

struct OpenMesh::VertexHandle const OpenMesh::PolyConnectivity::InvalidVertexHandle;

#pragma region MyMesh

MyMesh::MyMesh()
{
	request_vertex_normals();
	request_vertex_status();
	request_face_status();
	request_edge_status();
}

MyMesh::~MyMesh()
{
	// clear();
}

#pragma endregion

#pragma region GLMesh

GLMesh::GLMesh()
{

}

GLMesh::~GLMesh()
{

}


bool GLMesh::Init(std::string fileName)
{
	/*meshList.clear();
	vaoList.clear();
	eboList.clear();
	vboNList.clear();
	vboTList.clear();
	vboVList.clear();*/
	for (int i = 0; i < TEXTURESUM; i++) {
		meshList.push_back(MyMesh());
		vaoList.push_back(GLuint());
		eboList.push_back(GLuint());
		vboNList.push_back(GLuint());
		vboTList.push_back(GLuint());
		vboVList.push_back(GLuint());
	}
	if (LoadModel(fileName, 0))
	{
		LoadToShader(0);
	}
	else
		return false;
	return true;
}

void GLMesh::Render()
{
	/*
	glBindVertexArray(vao);
	glDrawElements(GL_TRIANGLES, mesh.n_faces() * 3, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
	*/
}


bool GLMesh::LoadModel(std::string fileName, int layer)
{
	OpenMesh::IO::Options ropt;
	if (OpenMesh::IO::read_mesh(meshList[layer], fileName, ropt) && layer == 0)
	{
		if (!ropt.check(OpenMesh::IO::Options::VertexNormal) && meshList[layer].has_vertex_normals())
		{
			meshList[layer].request_face_normals();
			meshList[layer].update_normals();
			meshList[layer].release_face_normals();
		}

		for (int i = 1; i < TEXTURESUM; i++)
		{
			meshList[i] = meshList[0];
		}
		return true;
	}
	return false;
}

void GLMesh::LoadToShader(int layer)
{
	std::vector<MyMesh::Point> vertices;
	vertices.reserve(meshList[layer].n_vertices());
	for (MyMesh::VertexIter v_it = meshList[layer].vertices_begin(); v_it != meshList[layer].vertices_end(); ++v_it)
	{
		vertices.push_back(meshList[layer].point(*v_it));

		MyMesh::Point p = meshList[layer].point(*v_it);
	}

	std::vector<MyMesh::Normal> normals;
	normals.reserve(meshList[layer].n_vertices());
	for (MyMesh::VertexIter v_it = meshList[layer].vertices_begin(); v_it != meshList[layer].vertices_end(); ++v_it)
	{
		normals.push_back(meshList[layer].normal(*v_it));
	}

	std::vector<unsigned int> indices;
	indices.reserve(meshList[layer].n_faces() * 3);
	for (MyMesh::FaceIter f_it = meshList[layer].faces_begin(); f_it != meshList[layer].faces_end(); ++f_it)
	{
		for (MyMesh::FaceVertexIter fv_it = meshList[layer].fv_iter(*f_it); fv_it.is_valid(); ++fv_it)
		{
			indices.push_back(fv_it->idx());
		}
	}

	for (int i = 0; i < TEXTURESUM; i++) 
	{
		glGenVertexArrays(1, &vaoList[i]);
		glBindVertexArray(vaoList[i]);

		glGenBuffers(1, &vboVList[i]);
		glBindBuffer(GL_ARRAY_BUFFER, vboVList[i]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(MyMesh::Point) * vertices.size(), &vertices[0], GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(0);

		glGenBuffers(1, &vboNList[i]);
		glBindBuffer(GL_ARRAY_BUFFER, vboNList[i]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(MyMesh::Normal) * normals.size(), &normals[0], GL_STATIC_DRAW);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(1);

		glGenBuffers(1, &eboList[i]);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, eboList[i]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(int) * indices.size(), &indices[0], GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}

}

void GLMesh::LoadTexCoordToShader(int layer)
{
	if (meshList[layer].has_vertex_texcoords2D())
	{
		std::vector<MyMesh::TexCoord2D> texCoords;
		for (MyMesh::VertexIter v_it = meshList[layer].vertices_begin(); v_it != meshList[layer].vertices_end(); ++v_it)
		{
			MyMesh::TexCoord2D texCoord = meshList[layer].texcoord2D(*v_it);
			texCoords.push_back(texCoord);
		}

		glBindVertexArray(vaoList[layer]);

		glGenBuffers(1, &vboTList[layer]);
		glBindBuffer(GL_ARRAY_BUFFER, vboTList[layer]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(MyMesh::TexCoord2D) * texCoords.size(), &texCoords[0], GL_STATIC_DRAW);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(2);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}
}

#pragma endregion

MeshObject::MeshObject()
{

}

MeshObject::~MeshObject()
{
}

bool MeshObject::Init(std::string fileName)
{
	selectedFaceList.clear();
	faceIDsPtrList.clear();
	elementCountList.clear();
	for (int i = 0; i < TEXTURESUM; i++) {
		selectedFaceList.push_back(std::vector<unsigned int>());
		faceIDsPtrList.push_back(std::vector<unsigned int*>());
		elementCountList.push_back(std::vector<int>());
	}


	return model.Init(fileName);
}

void MeshObject::Render()
{
	// 0當基準 
	glBindVertexArray(model.vaoList[0]);
	glDrawElements(GL_TRIANGLES, model.meshList[0].n_faces() * 3, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

void MeshObject::RenderSelectedFace()
{/*
	if (selectedFace.size() > 0)
	{
		std::vector<unsigned int*> offsets(selectedFace.size());
		for (int i = 0; i < offsets.size(); ++i)
		{
			offsets[i] = (GLuint*)(selectedFace[i] * 3 * sizeof(GLuint));
		}

		std::vector<int> count(selectedFace.size(), 3);

		glBindVertexArray(model.vao);
		glMultiDrawElements(GL_TRIANGLES, &count[0], GL_UNSIGNED_INT, (const GLvoid**)&offsets[0], selectedFace.size());
		glBindVertexArray(0);
	}*/
}

void MeshObject::RenderSelectedFace(int layer)
{
	if (selectedFaceList.size() == 0)
		return;

	if (selectedFaceList[layer].size() > 0)
	{
		std::vector<unsigned int*> offsets(selectedFaceList[layer].size());
		for (int i = 0; i < offsets.size(); ++i)
		{
			offsets[i] = (GLuint*)(selectedFaceList[layer][i] * 3 * sizeof(GLuint));
		}

		std::vector<int> count(selectedFaceList[layer].size(), 3);

		glBindVertexArray(model.vaoList[layer]);
		glMultiDrawElements(GL_TRIANGLES, &count[0], GL_UNSIGNED_INT, (const GLvoid**)&offsets[0], selectedFaceList[layer].size());
		glBindVertexArray(0);
	}
}

bool MeshObject::AddSelectedFace(unsigned int faceID, int layer)
{
	if (std::find(selectedFaceList[layer].begin(), selectedFaceList[layer].end(), faceID) == selectedFaceList[layer].end() &&
		faceID >= 0 && faceID < model.meshList[layer].n_faces())
	{
		selectedFaceList[layer].push_back(faceID);
		return true;
	}
	return false;
}

void MeshObject::MoveSelectedFace(unsigned int faceID, int layer)
{
		//selectedFaceList[layer].erase(std::remove(selectedFaceList[layer].begin(), selectedFaceList[layer].end(), faceID), selectedFaceList[layer].end());
	selectedFaceList[layer].clear();
	selectedFaceList[layer].push_back(faceID);
	for (int i = 0; i < 3; ++i) Scale(layer);
	//
	
}

void MeshObject::Parameterization(float uvRotateAngle, int layer)
{
	if (selectedFaceList[layer].size() == 0) {
		return;
	}

	MyMesh mesh;
	for (MyMesh::EdgeIter e_it = mesh.edges_begin(); e_it != mesh.edges_end(); ++e_it)
	{
		if (mesh.is_boundary(*e_it))
			std::cout << "boundary" << std::endl;
	}

	std::sort(selectedFaceList[layer].begin(), selectedFaceList[layer].end());

	OpenMesh::HPropHandleT<double> heWeight;
	OpenMesh::VPropHandleT<int> row;

	mesh.add_property(heWeight, "heWeight");
	mesh.add_property(row, "row");
	mesh.request_vertex_texcoords2D();

	CopySelectFace(mesh, layer);

	//calculate weight
	MyMesh::HalfedgeHandle heh;
	for (MyMesh::EdgeIter e_it = mesh.edges_begin(); e_it != mesh.edges_end(); ++e_it)
	{
		if (!mesh.is_boundary(*e_it))
		{
			GLdouble angle1, angle2, w;
			MyMesh::HalfedgeHandle _heh = mesh.halfedge_handle(*e_it, 0);
			MyMesh::Point pFrom = mesh.point(mesh.from_vertex_handle(_heh));
			MyMesh::Point pTo = mesh.point(mesh.to_vertex_handle(_heh));
			MyMesh::Point p1 = mesh.point(mesh.opposite_vh(_heh));
			MyMesh::Point p2 = mesh.point(mesh.opposite_he_opposite_vh(_heh));

#ifdef Harmonic
			OpenMesh::Vec3d v1 = (OpenMesh::Vec3d)(p1 - pFrom);
			v1.normalize();

			OpenMesh::Vec3d v2 = (OpenMesh::Vec3d)(p1 - pTo);
			v2.normalize();

			angle1 = std::acos(OpenMesh::dot(v1, v2));


			v1 = (OpenMesh::Vec3d)(p2 - pFrom);
			v1.normalize();

			v2 = (OpenMesh::Vec3d)(p2 - pTo);
			v2.normalize();

			angle2 = std::acos(OpenMesh::dot(v1, v2));

			w = std::cos(angle1) / std::sin(angle1) + std::cos(angle2) / std::sin(angle2);

			mesh.property(heWeight, _heh) = w;
			mesh.property(heWeight, mesh.opposite_halfedge_handle(_heh)) = w;
#else
			double edgeLen = (pFrom - pTo).length();

			// weight from to
			OpenMesh::Vec3d v1 = (OpenMesh::Vec3d)(pTo - pFrom);
			v1.normalize();

			OpenMesh::Vec3d v2 = (OpenMesh::Vec3d)(p1 - pFrom);
			v2.normalize();

			angle1 = std::acos(OpenMesh::dot(v1, v2));

			v2 = (OpenMesh::Vec3d)(p2 - pFrom);
			v2.normalize();

			angle2 = std::acos(OpenMesh::dot(v1, v2));

			w = (std::tan(angle1 / 2.0f) + std::tan(angle2 / 2.0f)) / edgeLen;

			mesh.property(heWeight, _heh) = w;

			// weight to from
			v1 = -v1;

			v2 = (OpenMesh::Vec3d)(p1 - pTo);
			v2.normalize();

			angle1 = std::acos(OpenMesh::dot(v1, v2));

			v2 = (OpenMesh::Vec3d)(p2 - pTo);
			v2.normalize();

			angle2 = std::acos(OpenMesh::dot(v1, v2));

			w = (std::tan(angle1 / 2.0f) + std::tan(angle2 / 2.0f)) / edgeLen;

			mesh.property(heWeight, mesh.opposite_halfedge_handle(_heh)) = w;

#endif


		}
		else
		{
			if (!heh.is_valid())
			{
				heh = mesh.halfedge_handle(*e_it, 1);
			}
		}
	}

	// calculate matrix size
	int count = 0;
	for (MyMesh::VertexIter v_it = mesh.vertices_begin(); v_it != mesh.vertices_end(); ++v_it)
	{
		if (mesh.is_boundary(*v_it))
		{
			mesh.property(row, *v_it) = -1;
		}
		else
		{
			mesh.property(row, *v_it) = count++;
		}
	}

	// calculate perimeter
	double perimeter = 0;
	std::vector<double> segLength;
	std::vector<MyMesh::VertexHandle> vhs;
	MyMesh::HalfedgeHandle hehNext = heh;
	do
	{
		MyMesh::Point from = mesh.point(mesh.from_vertex_handle(hehNext));
		MyMesh::Point to = mesh.point(mesh.to_vertex_handle(hehNext));
		perimeter += (from - to).length();

		segLength.push_back(perimeter);
		vhs.push_back(mesh.from_vertex_handle(hehNext));

		hehNext = mesh.next_halfedge_handle(hehNext);
	} while (heh != hehNext);


#ifdef Quad

	float rd = (225 + uvRotateAngle) * M_PI / 180.0;
	float initDist = 0;
	MyMesh::TexCoord2D st(0, 0);
	float R = std::sqrt(2) / 2.0;
	st[0] = R * cos(rd) + 0.5;
	st[1] = R * sin(rd) + 0.5;

	if (st[0] > 1)
	{
		st[0] = 1;
		st[1] = tan(rd) / 2 + 0.5;
	}

	if (st[0] < 0)
	{
		st[0] = 0;
		st[1] = 0.5 - tan(rd) / 2;
	}

	if (st[1] > 1)
	{
		st[0] = tan(M_PI_2 - rd) / 2 + 0.5;
		st[1] = 1;
	}

	if (st[1] < 0)
	{
		st[0] = 0.5 - tan(M_PI_2 - rd) / 2;
		st[1] = 0;
	}


	if (uvRotateAngle <= 90)
	{
		initDist = st.length();
	}

	else if (uvRotateAngle <= 180)
	{
		initDist = 1 + st[1];
	}

	else if (uvRotateAngle <= 270)
	{
		initDist = 3 - st[0];
	}

	else
	{
		initDist = 4 - st[1];
	}

	mesh.set_texcoord2D(vhs[0], st);
	perimeter /= 4.0;
	for (int i = 1; i < vhs.size(); ++i)
	{
		double curLen = segLength[i - 1] / perimeter + initDist;
		if (curLen > 4)
		{
			curLen -= 4;
		}

		if (curLen <= 1)
		{
			st[0] = curLen;
			st[1] = 0;
		}
		else if (curLen <= 2)
		{
			st[0] = 1;
			st[1] = curLen - 1;
		}
		else if (curLen <= 3)
		{
			st[0] = 3 - curLen;
			st[1] = 1;
		}
		else
		{
			st[0] = 0;
			st[1] = 4 - curLen;
		}

		mesh.set_texcoord2D(vhs[i], st);
	}
#else

	MyMesh::TexCoord2D st(1, 0.5);
	mesh.set_texcoord2D(vhs[0], st);

	for (int i = 1; i < vhs.size(); ++i)
	{
		double angle = 2 * M_PI * segLength[i - 1] / perimeter;

		st[0] = (std::cos(angle) + 1) / 2;
		st[1] = (std::sin(angle) + 1) / 2;

		mesh.set_texcoord2D(vhs[i], st);
	}
#endif

	typedef Eigen::SparseMatrix<double> SpMat;

	SpMat A(count, count);
	Eigen::VectorXd BX(count);
	Eigen::VectorXd BY(count);
	Eigen::SimplicialLDLT<Eigen::SparseMatrix<double> > linearSolver;

	BX.setZero();
	BY.setZero();

	// fiil matrix
	for (MyMesh::VertexIter v_it = mesh.vertices_begin(); v_it != mesh.vertices_end(); ++v_it)
	{
		if (!mesh.is_boundary(*v_it))
		{
			int i = mesh.property(row, *v_it);
			double totalWeight = 0;

			for (MyMesh::VertexVertexIter vv_it = mesh.vv_iter(*v_it); vv_it.is_valid(); ++vv_it)
			{
				MyMesh::HalfedgeHandle _heh = mesh.find_halfedge(*v_it, *vv_it);
				double w = mesh.property(heWeight, _heh);

				if (mesh.is_boundary(*vv_it))
				{
					MyMesh::TexCoord2D texCoord = mesh.texcoord2D(*vv_it);
					BX[i] += w * texCoord[0];
					BY[i] += w * texCoord[1];
				}
				else
				{
					int j = mesh.property(row, *vv_it);
					A.insert(i, j) = -w;
				}
				totalWeight += w;
			}


			A.insert(i, i) = totalWeight;
		}
	}

	A.makeCompressed();

	// solve linear system
	SpMat At = A.transpose();
	linearSolver.compute(At * A);

	Eigen::VectorXd TX = linearSolver.solve(At * BX);
	Eigen::VectorXd TY = linearSolver.solve(At * BY);

	// set texcoord
	for (MyMesh::VertexIter v_it = mesh.vertices_begin(); v_it != mesh.vertices_end(); ++v_it)
	{
		if (!mesh.is_boundary(*v_it))
		{
			int i = mesh.property(row, *v_it);
			mesh.set_texcoord2D(*v_it, MyMesh::TexCoord2D(TX[i], TY[i]));
		}
	}

	// request vertex texcoord, if not exist 
	if (!model.meshList[layer].has_vertex_texcoords2D())
	{
		model.meshList[layer].request_vertex_texcoords2D();
		for (MyMesh::VertexIter v_it = model.meshList[layer].vertices_begin(); v_it != model.meshList[layer].vertices_end(); ++v_it)
		{
			model.meshList[layer].set_texcoord2D(*v_it, MyMesh::TexCoord2D(-1, -1));
		}
	}

	// map texcoord back to origin mesh
	int index = 0;
	for (MyMesh::FaceIter f_it = mesh.faces_begin(); f_it != mesh.faces_end(); ++f_it)
	{
		MyMesh::FaceHandle fh = *f_it;
		MyMesh::FaceHandle selectedFace_h = model.meshList[layer].face_handle(selectedFaceList[layer][index++]);

		MyMesh::FaceVertexIter fv_it = mesh.fv_iter(fh);
		MyMesh::FaceVertexIter selectedfv_it = model.meshList[layer].fv_iter(selectedFace_h);
		for (; fv_it.is_valid() && selectedfv_it.is_valid(); ++fv_it, ++selectedfv_it)
		{
			MyMesh::TexCoord2D texCoord = mesh.texcoord2D(*fv_it);
			model.meshList[layer].set_texcoord2D(*selectedfv_it, texCoord);
		}
	}

	model.LoadTexCoordToShader(layer);


	//fvIDsPtr.swap(std::vector<unsigned int*>(selectedFace.size()));
	faceIDsPtrList[layer] = std::vector<unsigned int*>(selectedFaceList[layer].size());
	for (int i = 0; i < faceIDsPtrList[layer].size(); ++i)
	{
		faceIDsPtrList[layer][i] = (GLuint*)(selectedFaceList[layer][i] * 3 * sizeof(GLuint));
	}
	//elemCount.swap(std::vector<int>(selectedFace.size(), 3));
	elementCountList[layer] = std::vector<int>(selectedFaceList[layer].size(), 3);
	cout << elementCountList[layer].size() << "," << faceIDsPtrList[layer].size() << endl;
}

void MeshObject::RenderParameterized(int layer)
{
	if (model.meshList[layer].has_vertex_texcoords2D() && elementCountList[layer].size() != 0)
	{
		glBindVertexArray(model.vaoList[layer]);
		glMultiDrawElements(GL_TRIANGLES, &elementCountList[layer][0], GL_UNSIGNED_INT, (const GLvoid**)&faceIDsPtrList[layer][0], elementCountList[layer].size());
		glBindVertexArray(0);
	}
}

void MeshObject::DeleteSelectedFace(unsigned int faceID, int layer)
{
	selectedFaceList[layer].erase(std::remove(selectedFaceList[layer].begin(), selectedFaceList[layer].end(), faceID), selectedFaceList[layer].end());
}

void MeshObject::CopySelectFace(MyMesh& mesh, int layer)
{
	mesh.request_vertex_normals();
	mesh.request_face_normals();

	std::vector<MyMesh::VertexHandle> vhs;
	vhs.reserve(3);

	std::map<int, int> usedVertex;

	for (std::vector<unsigned int>::iterator f_it = selectedFaceList[layer].begin(); f_it != selectedFaceList[layer].end(); ++f_it)
	{
		MyMesh::FaceHandle fh = model.meshList[layer].face_handle(*f_it);
		for (MyMesh::FaceVertexIter fv_it = model.meshList[layer].fv_iter(fh); fv_it.is_valid(); ++fv_it)
		{
			MyMesh::VertexHandle vh;
			MyMesh::Point p = model.meshList[layer].point(*fv_it);
			//int idx = mesh.FindVertex(p);

			std::map<int, int>::iterator usedVertex_it = usedVertex.find(fv_it->idx());

			if (usedVertex_it == usedVertex.end())
			{
				vh = mesh.add_vertex(p);
				usedVertex[fv_it->idx()] = vh.idx();
			}
			else
			{
				vh = mesh.vertex_handle(usedVertex_it->second);
			}

			vhs.push_back(vh);
		}

		mesh.add_face(vhs);
		vhs.clear();
	}

	mesh.update_normals();
}

bool MeshObject::ReadFile()
{
	fstream file;
	file.open("Reader.txt", ios::in);
	if (!file.is_open())
	{
		std::cout << "Can't open Reader.txt!" << endl;
		return false;
	}
	int textureCount, selectFaceCount, faceIDCount, elementCount;
	file >> textureCount;

	string temp;
	for (int i = 0; i < textureCount; i++) {
		selectedFaceList[i].clear();
		faceIDsPtrList[i].clear();
		elementCountList[i].clear();
		file >> temp;
		file >> selectFaceCount;
		selectedFaceList[i].resize(selectFaceCount);
		cout << "count=" << selectFaceCount << endl;
		for (int j = 0; j < selectFaceCount; j++) {
			file >> selectedFaceList[i][j];
		}
		/*if (selectFaceCount != 0)
			Parameterization(0, i);*/
	}
	return true;
}

bool MeshObject::WriteFile()
{
	fstream file;
	file.open("Reader.txt", ios::out);
	file.clear();
	file << selectedFaceList.size() << endl;					// 貼圖張數
	for (int i = 0; i < selectedFaceList.size(); i++) {
		file << "No." << i << endl;
		file << selectedFaceList[i].size() << endl;		// 貼圖面數
		for (int j = 0; j < selectedFaceList[i].size(); j++) {
			file << selectedFaceList[i][j] << endl;
		}
	}
	return true;
}

bool MeshObject::WriteMeshFile(int layer)
{

	OpenMesh::IO::Options wopt;
	wopt += OpenMesh::IO::Options::VertexTexCoord;
	wopt += OpenMesh::IO::Options::VertexNormal;

	string filename = to_string(layer) + "texture.obj";
	if (!OpenMesh::IO::write_mesh(model.meshList[layer], filename, wopt))
	{
		printf("Write Mesh Error\n");
		return false;
	}
	std::cout << "Save file: " << filename << endl;
	return true;
}

bool MeshObject::ReadMeshFile(int layer)
{
	MyMesh m;
	OpenMesh::IO::Options wopt;
	wopt += OpenMesh::IO::Options::VertexTexCoord;
	wopt += OpenMesh::IO::Options::VertexNormal;
	//string filename = to_string(layer) + "texture.obj";
	std::cout << "read" << endl;
	string filename = "debug2.obj";
	if (!OpenMesh::IO::read_mesh(m, filename, wopt))
	{
		printf("Read Mesh Error\n");
		return false;
	}

	std::cout << m.n_faces() << endl;

	model.LoadTexCoordToShader(layer);
}


//void MeshObject::scaleSelectedFace(unsigned int faceID, int layer) {
//	MyMesh mesh;
//	CopySelectFace(mesh, layer);
//	std::vector<unsigned int> boundary_face;
//
//	int index = 0;
//	for (MyMesh::FaceIter f_it = mesh.faces_begin(); f_it != mesh.faces_end(); ++f_it)
//	{
//		MyMesh::FaceHandle fh = *f_it;
//		MyMesh::FaceHandle selectedFace_h = model.meshList[layer].face_handle(*f_it);
//		MyMesh::FaceVertexIter fv_it = mesh.fv_iter(fh);
//		MyMesh::FaceVertexIter selectedfv_it = editorModel->mesh.fv_iter(selectedFace_h);
//		for (; fv_it.is_valid() && selectedfv_it.is_valid(); ++fv_it, ++selectedfv_it)
//		{
//			for (MyMesh::VertexFaceIter vf_it = editorModel->mesh.vf_iter(*selectedfv_it); vf_it.is_valid(); ++vf_it)
//			{
//				char dest[6];
//				sprintf(dest, "%d", (*vf_it));
//
//				if (mesh.is_boundary(*fv_it))
//				{
//					boundary_face.push_back(atoi(dest));
//				}
//			}
//		}
//	}
//
//	for (std::vector<unsigned int>::iterator h = boundary_face.begin(); h != boundary_face.end(); h++)
//	{
//		if (ScaleUp)
//			addSelectedFace(*h);
//		else
//			deleteSelectedFace(*h);
//	}
//}

void MeshObject::Scale(int layer)
{

	model.meshList[layer].request_vertex_normals();
	model.meshList[layer].request_face_normals();

	std::vector<MyMesh::VertexHandle> vhs;
	vhs.reserve(3);

	std::map<int, int> usedVertex;

	std::vector<unsigned int> temp = selectedFaceList[layer];

	// 目前所選的面
	for (std::vector<unsigned int>::iterator f_it = selectedFaceList[layer].begin(); f_it != selectedFaceList[layer].end(); ++f_it)
	{
		//std::cout << "面" << *f_it << endl;
		MyMesh::FaceHandle nowfh = model.meshList[layer].face_handle(*f_it);
		// 用面找點
		for (MyMesh::FaceVertexIter fv_it = model.meshList[layer].fv_iter(nowfh); fv_it.is_valid(); ++fv_it)
		{
			int i = 0;
			// 從點拿到附近的面
			for (MyMesh::VertexFaceIter vf_it = model.meshList[layer].vf_iter(*fv_it); vf_it.is_valid(); ++vf_it)
			{
				i++;
				MyMesh::FaceHandle fh = *vf_it;
				//std::cout << "Face ID = " << fh.idx() << endl;

				vector<unsigned int>::iterator it = std::find(temp.begin(), temp.end(), fh.idx());
				// 已經有這個面了
				if (it != temp.end())
					continue;

				//std::cout << "沒有這個面 新增" << endl;			
				temp.push_back(fh.idx());

				// 用面拿到3個點
				for (MyMesh::FaceVertexIter fv_it = model.meshList[layer].fv_iter(*vf_it); fv_it.is_valid(); ++fv_it)
				{
					MyMesh::VertexHandle vh;
					MyMesh::Point p = model.meshList[layer].point(*fv_it);
					//int idx = mesh.FindVertex(p);

					std::map<int, int>::iterator usedVertex_it = usedVertex.find(fv_it->idx());

					if (usedVertex_it == usedVertex.end())
					{
						vh = model.meshList[layer].add_vertex(p);
						usedVertex[fv_it->idx()] = vh.idx();
					}
					else
					{
						vh = model.meshList[layer].vertex_handle(usedVertex_it->second);
					}

					vhs.push_back(vh);
				}

				model.meshList[layer].add_face(vhs);
				vhs.clear();
			}
			//std::cout << "找到" << i << "個面!" << endl;

		}
	}

	selectedFaceList[layer] = temp;

	//std::cout << "現有面數=" << selectedFaceList[layer].size() << endl;
	model.meshList[layer].update_normals();

}

void MeshObject::Shrink(int layer)
{

	model.meshList[layer].request_vertex_normals();
	model.meshList[layer].request_face_normals();

	bool hasDelete = false;
	std::vector<unsigned int> temp = selectedFaceList[layer];

	for (std::vector<unsigned int>::iterator f_it = selectedFaceList[layer].begin(); f_it != selectedFaceList[layer].end(); ++f_it)
	{
		hasDelete = false;
		MyMesh::FaceHandle nowfh = model.meshList[layer].face_handle(*f_it);
		for (MyMesh::FaceVertexIter fv_it = model.meshList[layer].fv_iter(nowfh); fv_it.is_valid(); ++fv_it)
		{
			for (MyMesh::VertexFaceIter vf_it = model.meshList[layer].vf_iter(*fv_it); vf_it.is_valid(); ++vf_it)
			{
				MyMesh::FaceHandle fh = *vf_it;
				//std::cout << "Face ID = " << fh.idx() << endl;
				vector<unsigned int>::iterator it = std::find(selectedFaceList[layer].begin(), selectedFaceList[layer].end(), fh.idx());
				// 沒有存這個面 = 是邊界面
				if (it == selectedFaceList[layer].end())
				{
					temp.erase(std::remove(temp.begin(), temp.end(), nowfh.idx()), temp.end());
					hasDelete = true;
					break;
				}
			}
			if (hasDelete)
				break;
		}
	}

	selectedFaceList[layer] = temp;

	std::cout << "現有面數=" << selectedFaceList[layer].size() << endl;
	model.meshList[layer].update_normals();

}