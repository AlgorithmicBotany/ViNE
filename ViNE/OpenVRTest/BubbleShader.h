#pragma once

#include "SimpleShader.h"
#include "TemplatedShader.h"
#include "ColorMat.h"

namespace renderlib {

class BubbleShaderBin : public ShaderT<ColorMat> {
public:
	BubbleShaderBin();
	void draw(const Camera& cam_left, const Camera& cam_right, Drawable &obj);
};

//TODO: Does the inheritance make sense?
class BubbleShader : public SimpleShader {
protected:
	vector<int> uniformLocations;

	virtual vector<pair<GLenum, string>> defaultShaders();
	void calculateUniformLocations() override;
	void loadUniforms(const glm::mat4& vp_matrix, const glm::mat4& m_matrix, glm::vec3 viewLocation);
public:
	BubbleShader(map<GLenum, string> defines = map<GLenum, string>{});
	BubbleShader(vector<pair<GLenum, string>> alt_shaders,
		map<GLenum, string> defines = map<GLenum, string>{});

	virtual void draw(const Camera &cam, Drawable &obj) override;
};

}