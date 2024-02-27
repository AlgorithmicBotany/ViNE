#include "VRColorShader.h"
#include "ColorSetMat.h"
#include "ShadedMat.h"
#include <string>

using namespace glm;
using namespace std;

namespace renderlib {

enum {
	VP_MATRIX_LOCATION = ShadedMat::COUNT + ColorSetMat::COUNT,
	M_MATRIX_LOCATION,
	VIEW_LOCATION,
	LIGHT_POS_LOCATION,
	FOG_DISTANCE_LOCATION,
	FOG_COLOR_LOCATION,
	FOG_SCALE_LOCATION,
	COUNT
};

struct uniform {
	enum {
	VP_MATRIX_LOCATION = ShadedMat::COUNT + ColorSetMat::COUNT,
	M_MATRIX_LOCATION,
	VIEW_LOCATION,
	LIGHT_POS_LOCATION,
	FOG_DISTANCE_LOCATION,
	FOG_COLOR_LOCATION,
	FOG_SCALE_LOCATION,
	PLANE_ORIGIN,
	PLANE_NORMAL,
	COUNT
	};
};

vector<pair<GLenum, string>> VRColorShaderBin::shaders() {
	return {
		{ GL_VERTEX_SHADER, "shaders/binMarchingCubeColor.vert" },
		{ GL_FRAGMENT_SHADER, "shaders/binMarchingCubeColor.frag" }
	};
}

VRColorShaderBin::VRColorShaderBin(int maxColorNum)
	:ShaderT<ShadedMat, ColorSetMat>(shaders(), 
		{{GL_VERTEX_SHADER, std::string("#define MAX_COLOR_NUM " + to_string(maxColorNum) + "\n") } }, 
		{ "ka", "ks", "kd", "alpha",
		"colors", "visibility", "view_projection_matrix", "model_matrix", "viewPosition", "lightPos",
		"fogDist", "fogColor", "fogScale" ,
		"planeOrigin", "planeNormal"}){}

void VRColorShaderBin::draw(const Camera &cam_left, const Camera &cam_right, glm::vec3 lightPos,
	float fogScale, float fogDistance, glm::vec3 fogColor, Drawable &obj) 
{
	glUseProgram(programID);

	mat4 vp_matrix[2] = {
			cam_left.getProjectionMatrix()*cam_left.getCameraMatrix(),
			cam_right.getProjectionMatrix()*cam_right.getCameraMatrix() };

	mat4 m_matrix = obj.getTransform();
	vec3 camera_pos[2] = { cam_left.getPosition(), cam_right.getPosition() };

	loadMaterialUniforms(obj);
	glUniformMatrix4fv(uniformLocations[uniform::VP_MATRIX_LOCATION], 2, false, &vp_matrix[0][0][0]);
	glUniformMatrix4fv(uniformLocations[uniform::M_MATRIX_LOCATION], 1, false, &m_matrix[0][0]);
	glUniform3fv(uniformLocations[uniform::VIEW_LOCATION], 2, &camera_pos[0][0]);
	glUniform3f(uniformLocations[uniform::LIGHT_POS_LOCATION], lightPos.x, lightPos.y, lightPos.z);
	glUniform1f(uniformLocations[uniform::FOG_SCALE_LOCATION], fogScale);
	glUniform1f(uniformLocations[uniform::FOG_DISTANCE_LOCATION], fogDistance);
	glUniform3f(uniformLocations[uniform::FOG_COLOR_LOCATION], fogColor.x, fogColor.y, fogColor.z);
	/*
	glUniformMatrix4fv(uniformLocations[uniform::M_MATRIX_LOCATION], 1, false, &m_matrix[0][0]);
	glUniform3fv(uniformLocations[uniform::VIEW_LOCATION], 2, &camera_pos[0][0]);
	glUniform3f(uniformLocations[uniform::LIGHT_POS_LOCATION], lightPos.x, lightPos.y, lightPos.z);
	glUniform1f(uniformLocations[uniform::FOG_SCALE_LOCATION], fogScale);
	glUniform1f(uniformLocations[uniform::FOG_DISTANCE_LOCATION], fogDistance);
	glUniform3f(uniformLocations[uniform::FOG_COLOR_LOCATION], fogColor.x, fogColor.y, fogColor.z);
	*/
	obj.getGeometry().drawGeometry();
	glUseProgram(0);
}

void VRColorShaderBin::drawNew(const Camera &cam_left, const Camera &cam_right, glm::vec3 lightPos,
	float fogScale, float fogDistance, glm::vec3 fogColor, glm::vec3 planeOrigin, glm::vec3 planeNormal, Drawable &obj)
{
	glUseProgram(programID);

	mat4 vp_matrix[2] = {
			cam_left.getProjectionMatrix()*cam_left.getCameraMatrix(),
			cam_right.getProjectionMatrix()*cam_right.getCameraMatrix() };

	mat4 m_matrix = obj.getTransform();
	vec3 camera_pos[2] = { cam_left.getPosition(), cam_right.getPosition() };

	loadMaterialUniforms(obj);
	glUniformMatrix4fv(uniformLocations[uniform::VP_MATRIX_LOCATION], 2, false, &vp_matrix[0][0][0]);
	glUniformMatrix4fv(uniformLocations[uniform::M_MATRIX_LOCATION], 1, false, &m_matrix[0][0]);
	glUniform3fv(uniformLocations[uniform::VIEW_LOCATION], 2, &camera_pos[0][0]);
	glUniform3f(uniformLocations[uniform::LIGHT_POS_LOCATION], lightPos.x, lightPos.y, lightPos.z);
	glUniform1f(uniformLocations[uniform::FOG_SCALE_LOCATION], fogScale);
	glUniform1f(uniformLocations[uniform::FOG_DISTANCE_LOCATION], fogDistance);
	glUniform3f(uniformLocations[uniform::FOG_COLOR_LOCATION], fogColor.x, fogColor.y, fogColor.z);
	glUniform3f(uniformLocations[uniform::PLANE_ORIGIN], planeOrigin.x, planeOrigin.y, planeOrigin.z);
	glUniform3f(uniformLocations[uniform::PLANE_NORMAL], planeNormal.x, planeNormal.y, planeNormal.z);

	obj.getGeometry().drawGeometry(programID);
	glUseProgram(0);
}

///////////////////
// VRColorShader
///////////////////

vector<pair<GLenum, string>> VRColorShader::shaders() {
	return {
		{ GL_VERTEX_SHADER, "shaders/marchingCubeColor.vert" },
		{ GL_FRAGMENT_SHADER, "shaders/marchingCubeColor.frag" }
	};
}

VRColorShader::VRColorShader(int maxColorNum)
	:ShaderT<ShadedMat, ColorSetMat>(shaders(),
		{ {GL_VERTEX_SHADER, std::string("#define MAX_COLOR_NUM " + to_string(maxColorNum) + "\n") } },
		{ "ka", "ks", "kd", "alpha",
		"colors", "visibility", "view_projection_matrix", "model_matrix", "viewPosition", "lightPos",
		"fogDist", "fogColor", "fogScale" }) {}

void VRColorShader::draw(const Camera &cam, glm::vec3 lightPos,
	float fogScale, float fogDistance, glm::vec3 fogColor, Drawable &obj)
{
	glUseProgram(programID);

	mat4 vp_matrix = cam.getProjectionMatrix()*cam.getCameraMatrix();

	mat4 m_matrix = obj.getTransform();
	vec3 camera_pos[2] = { cam.getPosition(), cam.getPosition() };

	loadMaterialUniforms(obj);
	glUniformMatrix4fv(uniformLocations[uniform::VP_MATRIX_LOCATION], 1, false, &vp_matrix[0][0]);
	glUniformMatrix4fv(uniformLocations[uniform::M_MATRIX_LOCATION], 1, false, &m_matrix[0][0]);
	glUniform3fv(uniformLocations[uniform::VIEW_LOCATION], 2, &camera_pos[0][0]);
	glUniform3f(uniformLocations[uniform::LIGHT_POS_LOCATION], lightPos.x, lightPos.y, lightPos.z);
	glUniform1f(uniformLocations[uniform::FOG_SCALE_LOCATION], fogScale);
	glUniform1f(uniformLocations[uniform::FOG_DISTANCE_LOCATION], fogDistance);
	glUniform3f(uniformLocations[uniform::FOG_COLOR_LOCATION], fogColor.x, fogColor.y, fogColor.z);

	obj.getGeometry().drawGeometry(programID);
	glUseProgram(0);
}

//REST SHOULD BE DEPRECATED?
/*
static vector<pair<GLenum, string>> shaders{
	{ GL_VERTEX_SHADER, "shaders/marchingCubeColor.vert" },
	{ GL_FRAGMENT_SHADER, "shaders/marchingCubeColor.frag" }
};

VRColorShader::VRColorShader(int maxColorNum) {
	createProgram({
		{GL_VERTEX_SHADER, 
		string("#define MAX_COLOR_NUM " + to_string(maxColorNum) + "\n") }});
	calculateUniformLocations();
}

VRColorShader::VRColorShader(map<GLenum, string> defines) {
	createProgram(defines);
	calculateUniformLocations();
}

bool VRColorShader::createProgram(map<GLenum, string> defines) {
	programID = createGLProgram(shaders, defines);

	return programID;
}

bool VRColorShader::createNewProgram(vector<pair<GLenum, string>> shaderNames, map<GLenum, string> defines) {
	programID = createGLProgram(shaderNames, defines);

	if (programID)
		calculateUniformLocations();

	return programID;
}

void VRColorShader::calculateUniformLocations() {
	glUseProgram(programID);

	uniformLocations.resize(COUNT);
	uniformLocations[ShadedMat::KA_LOCATION] =
		glGetUniformLocation(programID, "ka");
	uniformLocations[ShadedMat::KS_LOCATION] =
		glGetUniformLocation(programID, "ks");
	uniformLocations[ShadedMat::KD_LOCATION] =
		glGetUniformLocation(programID, "kd");
	uniformLocations[ShadedMat::ALPHA_LOCATION] =
		glGetUniformLocation(programID, "alpha");
	uniformLocations[ShadedMat::COUNT + ColorSetMat::COLORS_LOCATION] =
		glGetUniformLocation(programID, "colors");
	//Other uniforms
	uniformLocations[VP_MATRIX_LOCATION] =
		glGetUniformLocation(programID, "view_projection_matrix");
	uniformLocations[M_MATRIX_LOCATION] =
		glGetUniformLocation(programID, "model_matrix");
	uniformLocations[VIEW_LOCATION] =
		glGetUniformLocation(programID, "viewPosition");
	uniformLocations[LIGHT_POS_LOCATION] =
		glGetUniformLocation(programID, "lightPos");
	uniformLocations[FOG_COLOR_LOCATION] =
		glGetUniformLocation(programID, "fogColor");
	uniformLocations[FOG_DISTANCE_LOCATION] =
		glGetUniformLocation(programID, "fogDist");
	uniformLocations[FOG_SCALE_LOCATION] =
		glGetUniformLocation(programID, "fogScale");
}

void VRColorShader::loadUniforms(const mat4& vp_matrix, const mat4& m_matrix, vec3 viewPosition, vec3 lightPos, 
	float fogScale, float fogDistance, glm::vec3 fogColor) 
{
	glUniformMatrix4fv(uniformLocations[VP_MATRIX_LOCATION], 1, false, &vp_matrix[0][0]);
	glUniformMatrix4fv(uniformLocations[M_MATRIX_LOCATION], 1, false, &m_matrix[0][0]);
	glUniform3f(uniformLocations[VIEW_LOCATION], viewPosition.x, viewPosition.y, viewPosition.z);
	glUniform3f(uniformLocations[LIGHT_POS_LOCATION], lightPos.x, lightPos.y, lightPos.z);
	glUniform1f(uniformLocations[FOG_SCALE_LOCATION], fogScale);
	glUniform1f(uniformLocations[FOG_DISTANCE_LOCATION], fogDistance);
	glUniform3f(uniformLocations[FOG_COLOR_LOCATION], fogColor.x, fogColor.y, fogColor.z);
}


void VRColorShader::draw(const Camera &cam, glm::vec3 lightPos, 
	float fogScale, float fogDistance, glm::vec3 fogColor, Drawable &obj) 
{
	glUseProgram(programID);
	loadUniforms(cam.getProjectionMatrix()*cam.getCameraMatrix(), obj.getTransform(), cam.getPosition(), 
		lightPos, fogScale, fogDistance, fogColor);
	obj.loadUniforms(ShadedMat::id, &uniformLocations[0]);
	obj.loadUniforms(ColorSetMat::id, &uniformLocations[ShadedMat::COUNT]);
	obj.getGeometry().drawGeometry(programID);
	glUseProgram(0);
}
*/
}
