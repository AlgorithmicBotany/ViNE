#pragma once

#include "Shader.h"
#include "Drawable.h"
#include "Camera.h"
#include "Scene.h"
#include "TemplatedShader.h"
#include "ShadedMat.h"
#include "ColorMat.h"
#include "TextureMat.h"
#include <vector>
#include <glm/glm.hpp>


//Defines accepted: USING_TEXTURE

namespace renderlib {

template<typename ColorSrc>
constexpr const char* colorSrcToCString() {
	if constexpr (std::is_same<ColorSrc, ColorMat>::value)
		return "color";
	else
		return "colorTexture";
}

template<typename ColorSrc>
class BlinnPhongShaderVR_t : public ShaderT<ShadedMat, ColorSrc> {
	static inline vector<pair<GLenum, string>> shaders{
	{ GL_VERTEX_SHADER, "shaders/binBPShaded.vert" },
	{ GL_FRAGMENT_SHADER, "shaders/binBPShaded.frag" }};

	static std::pair<GLenum, string> defines() {
		if constexpr (std::is_same<ColorSrc, TextureMat>::value)
			return { GL_FRAGMENT_SHADER, "#define USING_TEXTURE\n" };
		else
			return {};
	}

public:
	BlinnPhongShaderVR_t() :
		ShaderT<ShadedMat, ColorSrc>(shaders, {defines()},
			{ "ka", "kd", "ks", "alpha", colorSrcToCString<ColorSrc>(),
				"view_projection_matrix",
				"model_matrix", "camera_position", "lightPos" })
	{}
	
	void draw(const Camera &cam_left, const Camera &cam_right, vec3 lightPos, Drawable &obj)
	{
		glUseProgram(programID);

		mat4 vp_matrix[2] = {
			cam_left.getProjectionMatrix()*cam_left.getCameraMatrix(),
			cam_right.getProjectionMatrix()*cam_right.getCameraMatrix() };

		mat4 m_matrix = obj.getTransform();
		vec3 camera_pos[2] = { cam_left.getPosition(), cam_right.getPosition() };

		loadMaterialUniforms(obj);
		glUniformMatrix4fv(uniformLocations[VP_MATRIX_LOCATION], 2, false, &vp_matrix[0][0][0]);
		glUniformMatrix4fv(uniformLocations[M_MATRIX_LOCATION], 1, false, &m_matrix[0][0]);
		glUniform3fv(uniformLocations[CAMERA_POS_LOCATION], 2, &camera_pos[0][0]);
		glUniform3f(uniformLocations[LIGHT_POS_LOCATION],
			lightPos. x, lightPos.y, lightPos.z);
		obj.getGeometry().drawGeometry(programID);
		glUseProgram(0);
	}
};

using BlinnPhongShaderVR = BlinnPhongShaderVR_t<ColorMat>;
using BlinnPhongTexShaderVR = BlinnPhongShaderVR_t<TextureMat>;

}