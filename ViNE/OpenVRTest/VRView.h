#pragma once

#include <glmSupport.h>
#include <Object.h>

namespace renderlib {

class VRView : public Object{
public:
	VRView();

	float scale;
	void getViewFromCameraPositionAndOrientation(glm::vec3 cameraPos, glm::vec3 cameraDir, Object* model);
	void generateView(glm::vec3 cameraPos, glm::vec3 cameraDir, Object* model);
};

bool loadVRViewFromFile(const char* filename, VRView* vrView);
bool saveVRViewToFile(const char* filename, VRView* vrView);

}
