#include "VRView.h"
#include <glm/gtx/transform.hpp>
#include  <cstdio>

namespace renderlib {

using namespace glm;

VRView::VRView() :Object(), scale(1.f) {}

void VRView::getViewFromCameraPositionAndOrientation(vec3 cameraPos, vec3 cameraDir, Object* model) {
	//Not completely robust if looking straight up
	vec3 planarDir = normalize(vec3(cameraDir.x, 0, cameraDir.z));
	float angle = acos(dot(planarDir, vec3(0, 0, -1)));

	float sign = cross(planarDir, vec3(0, 0, -1)).y;
	angle = (sign > 0) ? angle : -angle;
	
	mat4 viewRotation = rotate(angle, vec3(0, 1, 0));

	model->position = vec3(viewRotation*vec4(position, 1)) + cameraPos;
	model->orientation = quat_cast(viewRotation*getOrientationMat4());
}

//Creates the view
void VRView::generateView(glm::vec3 cameraPos, glm::vec3 cameraDir, Object* model) {
	//Not completely robust if looking straight up
	vec3 planarDir = normalize(vec3(cameraDir.x, 0, cameraDir.z));
	float angle = acos(dot(planarDir, vec3(0, 0, -1)));

	float sign = cross(vec3(0, 0, -1), planarDir).y;
	angle = (sign > 0) ? angle : -angle;

	mat4 viewRotation = rotate(angle, vec3(0, 1, 0));
	vec3 translation = model->getPos() - cameraPos;

	position = vec3(viewRotation*vec4(translation, 1));
	orientation = quat_cast(viewRotation*model->getOrientationMat4());
}

bool loadVRViewFromFile(const char* filename, VRView* vrView) {
	FILE* f = fopen(filename, "r");
	if (!f) {
		printf("Failed to load %s\n", filename);
		return false;
	}
	vec3& p = vrView->position;
	quat& q = vrView->orientation;
	fscanf(f, "position %f %f %f\n", &p.x, &p.y, &p.z);
	fscanf(f, "orientation %f %f %f %f\n", &q.x, &q.y, &q.z, &q.w);
	fscanf(f, "scale %f\n", &vrView->scale);

	fclose(f);
	return true;
}

bool saveVRViewToFile(const char* filename, VRView* vrView) {
	FILE* f = fopen(filename, "w");
	if (!f) {
		printf("Failed to save %s\n", filename);
		return false;
	}
	vec3& p = vrView->position;
	quat& q = vrView->orientation;
	fprintf(f, "position %f %f %f\n", p.x, p.y, p.z);
	fprintf(f, "orientation %f %f %f %f\n", q.x, q.y, q.z, q.w);
	fprintf(f, "scale %f\n", vrView->scale);

	printf("Saved view to %s\n", filename);

	fclose(f);
	return true;
}

}
