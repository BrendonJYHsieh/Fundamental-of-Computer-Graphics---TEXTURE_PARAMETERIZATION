#pragma once
#include <Common.h>
#include <ViewManager.h>
#include <ResourcePath.h>
#include <fstream>

#include "OpenMesh.h"
#include "MeshObject.h"
#include "DrawModelShader.h"
#include "PickingShader.h"
#include "PickingTexture.h"
#include "DrawPickingFaceShader.h"
#include "DrawTextureShader.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"
#include "stb_image.h"
//#define STB_IMAGE_IMPLEMENTATION

#define DOR(angle) (angle*3.1415/180);
#define TEXTURESUM 0

using namespace glm;
using namespace std;

int windowWidth = 650;
int windowHeight = 500;
glm::vec3 worldPos;
bool updateFlag = false;
bool isRightButtonPress = false;
GLuint currentFaceID = 0;
int currentMouseX = 0;
int currentMouseY = 0;

GLuint			program;
mat4			proj_matrix;
float			aspect = 0.5f;

GLuint			textureID;
vector<GLuint>	textureIDList(TEXTURESUM);
ViewManager		meshWindowCam, texCoordWindowCam;
vector<string> textureFileName;

bool drawTexture = false;

vector<float> uvPreRotate;
vector<float> uvRotate;
vector<mat4> uvTranslate;
vector<mat4> uvScale;
MeshObject model;
bool isReLoad = false;

vector<int> sequences = { 0,1,2,3,4,11,5,12,8,9,10,6,7,13 };

DrawTextureShader drawTextureShader;
DrawModelShader drawModelShader;
DrawPickingFaceShader drawPickingFaceShader;
PickingShader pickingShader;
PickingTexture pickingTexture;

// vbo for drawing point
GLuint vboPoint;

enum SelectMode {
	ADD_FACE,
	DEL_FACE,
	MOVE_FACE
};
SelectMode selectionMode = ADD_FACE;
int selectTextureIndex = 0;

mat4 Translate(float x, float y, float z) {
	vec4 t = vec4(x, y, z, 1);
	vec4 c1 = vec4(1, 0, 0, 0);
	vec4 c2 = vec4(0, 1, 0, 0);
	vec4 c3 = vec4(0, 0, 1, 0);
	mat4 M = mat4(c1, c2, c3, t);
	return M;
}
mat4 Scale(float x, float y, float z) {
	vec4 c1 = vec4(x, 0, 0, 0);
	vec4 c2 = vec4(0, y, 0, 0);
	vec4 c3 = vec4(0, 0, z, 0);
	vec4 c4 = vec4(0, 0, 0, 1);
	mat4 M = mat4(c1, c2, c3, c4);
	return M;
}
mat4 Rotate(float angle, float x, float y, float z) {
	float r = DOR(angle);
	mat4 M = mat4(1);

	vec4 c1 = vec4(cos(r) + (1 - cos(r)) * x * x, (1 - cos(r)) * y * x + sin(r) * z, (1 - cos(r)) * z * x - sin(r) * y, 0);
	vec4 c2 = vec4((1 - cos(r)) * y * x - sin(r) * z, cos(r) + (1 - cos(r)) * y * y, (1 - cos(r)) * z * y + sin(r) * x, 0);
	vec4 c3 = vec4((1 - cos(r)) * z * x + sin(r) * y, (1 - cos(r)) * z * y - sin(r) * x, cos(r) + (1 - cos(r)) * z * z, 0);
	vec4 c4 = vec4(0, 0, 0, 1);
	M = mat4(c1, c2, c3, c4);
	return M;
}

void ResetTexture()
{
	uvRotate[selectTextureIndex] = 0.0;
	uvTranslate[selectTextureIndex] = mat4(1.0);
	uvScale[selectTextureIndex] = mat4(1.0);
}

void TranslateTextureX(int value)
{
	uvTranslate[selectTextureIndex] = Translate(0.1f * value, 0, 0);
}

void TranslateTextureY(int value)
{
	uvTranslate[selectTextureIndex] = Translate(0, 0.1f * value, 0);
}

void RotateTexture(int value)
{
	uvRotate[selectTextureIndex] = value;
}

void ScaleTexture(int value)
{
	float fvalue = (float)value / 5;
	uvScale[selectTextureIndex] = Scale(fvalue, fvalue, fvalue);
}
void SelectionHandler(unsigned int x, unsigned int y);
TextureData Load_png(const char* path, bool mirroredY = true)
{
	TextureData texture;
	int n;
	stbi_uc* data = stbi_load(path, &texture.width, &texture.height, &n, 4);
	if (data != NULL)
	{
		texture.data = new unsigned char[texture.width * texture.height * 4 * sizeof(unsigned char)];
		memcpy(texture.data, data, texture.width * texture.height * 4 * sizeof(unsigned char));
		// vertical-mirror image data
		if (mirroredY)
		{
			for (size_t i = 0; i < texture.width; i++)
			{
				for (size_t j = 0; j < texture.height / 2; j++)
				{
					for (size_t k = 0; k < 4; k++) {
						std::swap(texture.data[(j * texture.width + i) * 4 + k], texture.data[((texture.height - j - 1) * texture.width + i) * 4 + k]);
					}
				}
			}
		}
		stbi_image_free(data);
	}
	else
		cout << "Load_png error" << endl;
	return texture;
}
void LoadModel();
void LoadTextures();
void InitOpenGL();
void InitData();
void My_Init();
void My_Paint();
void uvPaint();
void Parameterization();