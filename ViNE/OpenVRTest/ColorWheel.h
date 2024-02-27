#pragma once
#include <Drawable.h>
#include <vector>
#include <TemplatedShader.h>
#include <ShadedMat.h>
#include <ColorSetMat.h>
#include <Camera.h>

//Typically needs ColorSetMat
class ColorWheel : public renderlib::Drawable {
	std::vector<glm::vec3> points;
	std::vector<glm::vec3> normals;
	std::vector<unsigned char> colors;
	std::vector<glm::vec2> uvCoords;
	std::vector<unsigned int> indices;
	std::map<unsigned int, float> depressionList;

	const float RAISED_HEIGHT = 0.01f;

	std::vector<std::vector<unsigned int>> affectedVertices;

	enum {
		POSITION=0,
		NORMAL,
		COLOR
	};

	void generateSlice(float theta, float angularWidth, int thetaDivisions, 
		int radiusDivisions, unsigned char color);
	void generateColorWheelGeometry();
	void generateColorWheelGeometry2();
	glm::vec3 modelspaceNormal();

public:
	glm::vec3 origin;
	glm::vec3 bx;
	glm::vec3 by;
	int colorNum;
	int subdivisionNum;
	int selectedColor;

	ColorWheel(glm::vec3 origin, glm::vec3 bx, glm::vec3 by, 
		int colorNum, int subdivisionNum);

	void thumbPos(glm::vec2 pos);

	void selectColor(int color);
	glm::vec3 trackpadLightPosition(float dist);
};

class ColorWheelShaderBin : public renderlib::ShaderT<renderlib::ShadedMat, renderlib::ColorSetMat> {
	static std::vector<std::pair<GLenum, std::string>> shaders();

public:
	ColorWheelShaderBin(int maxColorNum);
	void draw(const renderlib::Camera &leftCam, const renderlib::Camera &rightCam, glm::vec3 lightPos, glm::vec3 center, renderlib::Drawable &obj);
	void drawNew(const renderlib::Camera &leftCam, const renderlib::Camera &rightCam, glm::vec3 lightPos, glm::vec3 center, renderlib::Drawable &obj);
};