#pragma once

//////////////////
// NON-COMPLIANT TO ATTRIB_LOCATION
//////////////////

#include "Shader.h"
#include "Drawable.h"
#include "Camera.h"
#include "Scene.h"
#include "TemplatedShader.h"
#include "ColorSetMat.h"
#include "ShadedMat.h"
#include <vector>

namespace renderlib {

class VRColorShaderBin : public ShaderT<ShadedMat, ColorSetMat> {
	static vector<pair<GLenum, string>> shaders();
	 
public:
	VRColorShaderBin(int maxColorNum);
	VRColorShaderBin(int maxColorNum, std::vector<pair<GLenum, string>> newShaders);
	void draw(const Camera &leftCam, const Camera &rightCam, glm::vec3 lightPos,
		float fogScale, float fogDistance, glm::vec3 fogColor, Drawable &obj);
	void drawNew(const Camera &leftCam, const Camera &rightCam, glm::vec3 lightPos,
		float fogScale, float fogDistance, glm::vec3 fogColor, 
		glm::vec3 planeOrigin, glm::vec3 planeNormal, Drawable &obj);
};


class VRColorShader : public ShaderT<ShadedMat, ColorSetMat> {
	static vector<pair<GLenum, string>> shaders();

public:
	VRColorShader(int maxColorNum);
	void draw(const Camera &cam, glm::vec3 lightPos,
		float fogScale, float fogDistance, glm::vec3 fogColor, Drawable &obj);
};

/*
class VRColorShader : public Shader {
protected:
	vector<int> uniformLocations;

	void calculateUniformLocations();
	void loadUniforms(const glm::mat4& vp_matrix, const glm::mat4& m_matrix, glm::vec3 viewPosition, glm::vec3 lightPos, 
		float fogScale, float fogDistance, glm::vec3 fogColor);
public:
	VRColorShader(int maxColorNum);
	VRColorShader(map<GLenum, string> defines = map<GLenum, string>{});

	virtual bool createProgram(map<GLenum, string> defines = map<GLenum, string>{});
	virtual bool createNewProgram(vector<pair<GLenum, string>> shaderNames, map<GLenum, string> defines = map<GLenum, string>{});


	void draw(const Camera &cam, glm::vec3 lightPos, 
		float fogScale, float fogDistance, glm::vec3 fogColor, Drawable &obj);
	void draw(const Camera &cam, const Scene &scene);
};
*/


}