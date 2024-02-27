#pragma once

#include <stdio.h>
#include <vector>
#include <glmSupport.h>
#include <functional>

struct StateAtDraw {
	glm::mat4 leftCamera;
	glm::mat4 rightCamera;
	//glm::mat4 modelTransform;
	glm::vec3 modelPosition;
	glm::quat modelOrientation;
	float modelScale;
	float brushRadius;
	char drawColor;
	//glm::mat4 controller [2];
	glm::vec3 controllerPosition[2];
	glm::quat controllerOrientation[2];
	bool controllerPainting[2];
};

std::vector<StateAtDraw> loadControllerSequence(const char* filename) {
	FILE* file = fopen(filename, "r");

	std::vector<StateAtDraw> stateSequence;
	char startFrameChar;
	while(fscanf(file, "-------------------%c\n", &startFrameChar) > 0){
		StateAtDraw state;
		glm::mat4& hl = state.leftCamera;
		fscanf(file, "LeftCamera (%f %f %f %f / %f %f %f %f / %f %f %f %f / %f %f %f %f)\n",
			&hl[0][0], &hl[0][1], &hl[0][2], &hl[0][3],
			&hl[1][0], &hl[1][1], &hl[1][2], &hl[1][3],
			&hl[2][0], &hl[2][1], &hl[2][2], &hl[2][3],
			&hl[3][0], &hl[3][1], &hl[3][2], &hl[3][3]);
		glm::mat4& hr = state.rightCamera;
		fscanf(file, "RightCamera (%f %f %f %f / %f %f %f %f / %f %f %f %f / %f %f %f %f)\n",
			&hr[0][0], &hr[0][1], &hr[0][2], &hr[0][3],
			&hr[1][0], &hr[1][1], &hr[1][2], &hr[1][3],
			&hr[2][0], &hr[2][1], &hr[2][2], &hr[2][3],
			&hr[3][0], &hr[3][1], &hr[3][2], &hr[3][3]);

		fscanf(file, "ModelPosition (%f %f %f)\n", 
			&state.modelPosition.x, &state.modelPosition.y, &state.modelPosition.z);
		fscanf(file, "ModelOrientation (%f %f %f %f)\n",
			&state.modelOrientation.w,
			&state.modelOrientation.x, &state.modelOrientation.y, &state.modelOrientation.z);
		fscanf(file, "ModelScale %f\n", &state.modelScale);

		fscanf(file, "Brush radius %f\n", &state.brushRadius);
		fscanf(file, "drawColor %d\n", &state.drawColor);

		fscanf(file, "ControllerPosition0 (%f %f %f)\n",
			&state.controllerPosition[0].x, &state.controllerPosition[0].y, &state.controllerPosition[0].z);
		fscanf(file, "ControllerOrientation0 (%f %f %f %f)\n",
			&state.controllerOrientation[0].w,
			&state.controllerOrientation[0].x, &state.controllerOrientation[0].y, &state.controllerOrientation[0].z);
		int temp = 0;
		fscanf(file, "ControllerPainting0 %d\n", &temp);
		state.controllerPainting[0] = temp;

		fscanf(file, "ControllerPosition1 (%f %f %f)\n",
			&state.controllerPosition[1].x, &state.controllerPosition[1].y, &state.controllerPosition[1].z);
		fscanf(file, "ControllerOrientation1 (%f %f %f %f)\n",
			&state.controllerOrientation[1].w,
			&state.controllerOrientation[1].x, &state.controllerOrientation[1].y, &state.controllerOrientation[1].z);
		temp = 0;
		fscanf(file, "ControllerPainting1 %d\n", &temp);
		state.controllerPainting[1] = temp;

		/*glm::mat4& m = state.modelTransform;
		fscanf(file, "Model (%f %f %f %f / %f %f %f %f / %f %f %f %f / %f %f %f %f)\n",
			&m[0][0], &m[0][1], &m[0][2], &m[0][3],
			&m[1][0], &m[1][1], &m[1][2], &m[1][3],
			&m[2][0], &m[2][1], &m[2][2], &m[2][3],
			&m[3][0], &m[3][1], &m[3][2], &m[3][3]);
		fscanf(file, "Brush radius %f\n", state.brushRadius);
		fscanf(file, "drawColor %d\n", state.drawColor);

		glm::mat4& c0 = state.controller[0];
		fscanf(file, "Left (%f %f %f %f / %f %f %f %f / %f %f %f %f / %f %f %f %f)\n",
			&c0[0][0], &c0[0][1], &c0[0][2], &c0[0][3],
			&c0[1][0], &c0[1][1], &c0[1][2], &c0[1][3],
			&c0[2][0], &c0[2][1], &c0[2][2], &c0[2][3],
			&c0[3][0], &c0[3][1], &c0[3][2], &c0[3][3]);
		glm::mat4& c1 = state.controller[1];
		fscanf(file, "Right (%f %f %f %f / %f %f %f %f / %f %f %f %f / %f %f %f %f)\n",
			&c1[0][0], &c1[0][1], &c1[0][2], &c1[0][3],
			&c1[1][0], &c1[1][1], &c1[1][2], &c1[1][3],
			&c1[2][0], &c1[2][1], &c1[2][2], &c1[2][3],
			&c1[3][0], &c1[3][1], &c1[3][2], &c1[3][3]);*/
		stateSequence.push_back(state);
	}

	return stateSequence;
}

void saveControllerSequence(const std::vector<StateAtDraw>& stateSequence, const char* filename) {
	FILE* file = fopen(filename, "w");

	for (auto state : stateSequence) {
		fprintf(file, "-------------------|\n");
		glm::mat4 m = state.leftCamera;
		fprintf(file, "LeftCamera (%f %f %f %f / %f %f %f %f / %f %f %f %f / %f %f %f %f)\n",
			m[0][0], m[0][1], m[0][2], m[0][3],
			m[1][0], m[1][1], m[1][2], m[1][3],
			m[2][0], m[2][1], m[2][2], m[2][3],
			m[3][0], m[3][1], m[3][2], m[3][3]);
		m = state.rightCamera;
		fprintf(file, "RightCamera (%f %f %f %f / %f %f %f %f / %f %f %f %f / %f %f %f %f)\n",
			m[0][0], m[0][1], m[0][2], m[0][3],
			m[1][0], m[1][1], m[1][2], m[1][3],
			m[2][0], m[2][1], m[2][2], m[2][3],
			m[3][0], m[3][1], m[3][2], m[3][3]);

		fprintf(file, "ModelPosition (%f %f %f)\n",
			state.modelPosition.x, state.modelPosition.y, state.modelPosition.z);
		fprintf(file, "ModelOrientation (%f %f %f %f)\n",
			state.modelOrientation.w,
			state.modelOrientation.x, state.modelOrientation.y, state.modelOrientation.z);
		fprintf(file, "ModelScale %f\n", state.modelScale);

		fprintf(file, "Brush radius %f\n", state.brushRadius);
		fprintf(file, "drawColor %d\n", state.drawColor);

		fprintf(file, "ControllerPosition0 (%f %f %f)\n",
			state.controllerPosition[0].x, state.controllerPosition[0].y, state.controllerPosition[0].z);
		fprintf(file, "ControllerOrientation0 (%f %f %f %f)\n",
			state.controllerOrientation[0].w,
			state.controllerOrientation[0].x, state.controllerOrientation[0].y, state.controllerOrientation[0].z);
		fprintf(file, "ControllerPainting0 %d\n", state.controllerPainting[0]);

		fprintf(file, "ControllerPosition1 (%f %f %f)\n",
			state.controllerPosition[1].x, state.controllerPosition[1].y, state.controllerPosition[1].z);
		fprintf(file, "ControllerOrientation1 (%f %f %f %f)\n",
			state.controllerOrientation[1].w,
			state.controllerOrientation[1].x, state.controllerOrientation[1].y, state.controllerOrientation[1].z);
		fprintf(file, "ControllerPainting1 %d\n", state.controllerPainting[1]);



		/*m = state.modelTransform;
		fprintf(file, "Model (%f %f %f %f / %f %f %f %f / %f %f %f %f / %f %f %f %f)\n",
			m[0][0], m[0][1], m[0][2], m[0][3],
			m[1][0], m[1][1], m[1][2], m[1][3],
			m[2][0], m[2][1], m[2][2], m[2][3],
			m[3][0], m[3][1], m[3][2], m[3][3]);
		fprintf(file, "Brush radius %f\n", state.brushRadius);
		fprintf(file, "drawColor %d\n", state.drawColor);
		
		m = state.controller[0];
		fprintf(file, "Left (%f %f %f %f / %f %f %f %f / %f %f %f %f / %f %f %f %f)\n",
			m[0][0], m[0][1], m[0][2], m[0][3],
			m[1][0], m[1][1], m[1][2], m[1][3],
			m[2][0], m[2][1], m[2][2], m[2][3],
			m[3][0], m[3][1], m[3][2], m[3][3]);
		m = state.controller[1];
		fprintf(file, "Right (%f %f %f %f / %f %f %f %f / %f %f %f %f / %f %f %f %f)\n",
			m[0][0], m[0][1], m[0][2], m[0][3],
			m[1][0], m[1][1], m[1][2], m[1][3],
			m[2][0], m[2][1], m[2][2], m[2][3],
			m[3][0], m[3][1], m[3][2], m[3][3]);
			*/
	}
}