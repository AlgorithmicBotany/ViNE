#pragma once
#include <string>
#include <fstream>
#include <vector>
#include "MeshInfoLoader.h"
#include <Bitmask.h>

bool saveVolume(std::string saveFileName, std::string objName, const unsigned char* colors, int pointNum);
bool loadVolume(std::string saveFileName, renderlib::MeshInfoLoader* minfo, std::vector<unsigned char>* colors, std::string* objName);

std::vector<glm::vec3> colorMapLoader(std::string colorFileName);

//Filename parsing and string manipulation functions
std::string removeChar(std::string originalString, char toRemove);
bool hasExtension(std::string filename, std::string matchedExtension);
std::string swapExtension(std::string filename, std::string newExtension);
std::string getFilename(std::string filepath);
std::string findFilenameVariation(std::string filepath);
std::string createPLYWithColors(std::string filename,
	unsigned int* faces, unsigned int faceNum,
	glm::vec3* positions, glm::vec3* normals, const unsigned char* colors,
	glm::vec3* colorMap, unsigned int pointNum, Bitmask visibility);