#include "BubbleShader.h"
#include "ColorMat.h"

using namespace glm;

namespace renderlib {

enum {
	VP_MATRIX_LOCATION = ColorMat::COUNT,
	M_MATRIX_LOCATION,
	VIEW_LOCATION,
	COUNT
};
BubbleShaderBin::BubbleShaderBin()
	:ShaderT<ColorMat>(
		{{GL_VERTEX_SHADER, "shaders/binBubbleShader.vert"},
		{GL_FRAGMENT_SHADER, "shaders/binBubbleShader.frag"} },
		{},
		{ "color", "view_projection_matrix", "model_matrix", "view_position" })
{}
void BubbleShaderBin::draw(const Camera& cam_left, const Camera& cam_right, Drawable &obj) {
	glUseProgram(programID);

	mat4 vp_matrix[2] = {
		cam_left.getProjectionMatrix()*cam_left.getCameraMatrix(),
		cam_right.getProjectionMatrix()*cam_right.getCameraMatrix() };

	mat4 m_matrix = obj.getTransform();
	vec3 camera_pos[2] = { cam_left.getPosition(), cam_right.getPosition() };

	loadMaterialUniforms(obj);
	glUniformMatrix4fv(uniformLocations[VP_MATRIX_LOCATION], 2, false, &vp_matrix[0][0][0]);
	glUniformMatrix4fv(uniformLocations[M_MATRIX_LOCATION], 1, false, &m_matrix[0][0]);
	glUniform3fv(uniformLocations[VIEW_LOCATION], 2, &camera_pos[0][0]);

	obj.getGeometry().drawGeometry(programID);
	glUseProgram(0);
}		


vector<pair<GLenum, string>> BubbleShader::defaultShaders() {
	return{ { GL_VERTEX_SHADER, "shaders/bubbleShader.vert" },
	{ GL_FRAGMENT_SHADER, "shaders/bubbleShader.frag" } };
}


BubbleShader::BubbleShader(map<GLenum, string> defines):SimpleShader(BubbleShader::defaultShaders(), defines) {
	calculateUniformLocations();
}

//Alt shader should use the same uniforms
BubbleShader::BubbleShader(vector<pair<GLenum, string>> alt_shaders, map<GLenum, string> defines) 
	:SimpleShader(alt_shaders, defines) {
	calculateUniformLocations();
}

void BubbleShader::calculateUniformLocations() {
	uniformLocations.clear();		//Clear old locations
	glUseProgram(programID);

	//Material uniforms
	uniformLocations.resize(COUNT);
	uniformLocations[ColorMat::COLOR_LOCATION] = glGetUniformLocation(programID, "color");

	//Other uniforms
	uniformLocations[VP_MATRIX_LOCATION] = glGetUniformLocation(programID,
		"view_projection_matrix");
	uniformLocations[M_MATRIX_LOCATION] = glGetUniformLocation(programID,
		"model_matrix");
	uniformLocations[VIEW_LOCATION] = glGetUniformLocation(programID,
		"view_position");
}

void BubbleShader::loadUniforms(const mat4& vp_matrix, const mat4& m_matrix, vec3 viewLocation) {
	glUniformMatrix4fv(uniformLocations[VP_MATRIX_LOCATION], 1, false, &vp_matrix[0][0]);
	glUniformMatrix4fv(uniformLocations[M_MATRIX_LOCATION], 1, false, &m_matrix[0][0]);
	glUniform3f(uniformLocations[VIEW_LOCATION], viewLocation.x, viewLocation.y, viewLocation.z);
}

void BubbleShader::draw(const Camera &cam, Drawable &obj) {
	glUseProgram(programID);
	loadUniforms(cam.getProjectionMatrix()*cam.getCameraMatrix(), obj.getTransform(), cam.getPosition());
	obj.loadUniforms(ColorMat::id, &uniformLocations[0]);

	obj.getGeometry().drawGeometry(programID);
	glUseProgram(0);
}

}
