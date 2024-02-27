//#define _CRT_SECURE_NO_WARNINGS

#include "VolumeIO.h"
#include <stdio.h>
#include <vector>
#include <algorithm>
#include <sstream>
#include <../tinyply/source/tinyply.h>


using namespace std;
using namespace renderlib;

string removeChar(string originalString, char toRemove) {
	string newString = originalString;
	for (int i = 0; i < newString.size(); i++) {
		if (newString[i] == toRemove) {
			newString.erase(i--, 1);
		}
	}

	return newString;
}

//Extension may or may not include period, it will be ignored
bool hasExtension(string filename, string matchedExtension) {
	matchedExtension = removeChar(matchedExtension, '.');
	size_t periodPos = filename.find_last_of('.');
	if (periodPos < filename.size()) {
		string extension = filename.substr(periodPos + 1,
			filename.size() - (periodPos + 1));
		return extension == matchedExtension;
	}
	else {
		return false;
	}
}

//Extension may or may not include period
string swapExtension(string filename, string newExtension) {
	newExtension = removeChar(newExtension, '.');
	size_t periodPos = filename.find_last_of('.');
	if (periodPos < filename.size()) {
		filename.erase(periodPos + 1);
	}
	else {
		filename.push_back('.');
	}

	filename.insert(filename.size(), newExtension);
	return filename;
}

template<typename T>
T maxValLessThan(T a, T b, T lessThan) {
	if (std::min(a, b) >= lessThan)
		return lessThan;
	else if (a >= lessThan)
		return b;
	else if (b >= lessThan)
		return a;
	else
		return std::max(a, b);
}

string getFilename(string filepath) {
	size_t lastForwardSlashPos = filepath.find_last_of('/');
	size_t lastBackSlashPos = filepath.find_last_of('\\');
	size_t splitPos = maxValLessThan(lastForwardSlashPos, lastBackSlashPos, filepath.size()) + 1;
	string filename;
	if (splitPos < filepath.size())
		filename = filepath.substr(splitPos, filepath.size() - splitPos);
	else
		filename = filepath;

	return filename;
}

//Changes filename if file exists with that name
string findFilenameVariation(string filepath) {
	int counter = 1;
	size_t splitPos = filepath.find_last_of('.');
	if (splitPos > filepath.size()) {
		splitPos = filepath.size() - 1;
	}
	FILE *f;
	errno_t err = fopen_s(&f, filepath.c_str(), "r");
	while (f != nullptr) {
		if (counter == 1)
			filepath.insert(splitPos, to_string(counter));
		else {
			filepath.erase(splitPos, to_string(counter - 1).size());
			filepath.insert(splitPos, to_string(counter));
		}
		fclose(f);
		errno_t err = fopen_s(&f, filepath.c_str(), "r");
		counter++;
	}


	return filepath;
}


bool saveVolume(std::string saveFileName, std::string objName, const unsigned char* colors, int pointNum) {
	std::ofstream f(saveFileName.c_str(), ios::binary);
	if (!f.is_open()) {
		printf("VolumeIO::saveVolume - File %s could not be opened\n", saveFileName.c_str());
		return false;
	}

	f << objName.c_str() << endl;

	for (int i = 0; i < pointNum; i++) {
		f << char(colors[i]);
	}

	printf("pointNum = %d\n", pointNum);
	return true;
}

bool loadVolume(std::string saveFileName, MeshInfoLoader* minfo, std::vector<unsigned char>* colors, std::string* objName) {
	std::ifstream f(saveFileName.c_str());
	if (!f.is_open()) {
		printf("VolumeIO::loadVolume - File %s could not be opened\n", saveFileName.c_str());
		return false;
	}

	const int BUFFER_SIZE = 1024;
	char buffer[BUFFER_SIZE];
	f.getline(buffer, BUFFER_SIZE, '\n');
	char value;
	while (f.read(&value, 1)){ //f >> value) {
		colors->push_back(value);
	}

	if (hasExtension(std::string(buffer), ".obj")) {
		if (!minfo->loadModel(buffer))
			return false;
	}
	else if (hasExtension(std::string(buffer), ".ply")) {
		if (!minfo->loadModelPly(buffer))
			return false;
	}
	else
		return false;

	if (minfo->vertices.size() != colors->size()) {
		printf("VolumeIO::loadVolume - Vertex and color size not equal\n");
		printf("\tVertice size = %d Color size = %d\n", minfo->vertices.size(), colors->size());
		return false;
	}

	(*objName) = buffer;

	return true;
}



std::vector<glm::vec3> colorMapLoader(std::string colorFileName) {
	std::ifstream f(colorFileName.c_str());
	if (!f.is_open()) {
		printf("VolumeIO::colorMapLoader - File %s could not be opened\n", colorFileName.c_str());
		return std::vector<glm::vec3>();
	}

	const int BUFFER_SIZE = 1024;
	char buffer[BUFFER_SIZE];

	unsigned int r, g, b;
	std::vector<glm::vec3> colors;

	do {
		f.getline(buffer, BUFFER_SIZE);
		std::stringstream ss;
		ss << std::string(buffer);
		int matched = 0;
		if (ss >> r) matched++;
		if (ss >> g) matched++;
		if (ss >> b) matched++;
		if (matched == 3)
			colors.push_back(glm::vec3(float(r) / 255.f, float(g) / 255.f, float(b) / 255.f));
		else
			printf("Error\n");
	} while (f.good());

	return colors;

}

std::string createPLYWithColors(std::string filename, 
	unsigned int* faces, unsigned int faceNum,
	glm::vec3* positions, glm::vec3* normals, const unsigned char* colors, 
	glm::vec3* colorMap, unsigned int pointNum, Bitmask visibility)
{
	std::filebuf fb;
	fb.open(filename, std::ios::out | std::ios::binary);
	std::ostream outstream(&fb);
	if (outstream.fail()) throw std::runtime_error("failed to open " + string(filename));

	std::vector<bool> keptVertices(pointNum, false);
	std::vector<unsigned int> newFaces;
	for (unsigned int i = 0; i < faceNum; i++) {
		unsigned int v_a = faces[3 * i];
		unsigned int v_b = faces[3 * i + 1];
		unsigned int v_c = faces[3 * i + 2];

		if (!visibility.test(colors[v_a]) || !visibility.test(colors[v_b]) || !visibility.test(colors[v_c])) {
			keptVertices[v_a] = true;
			keptVertices[v_b] = true;
			keptVertices[v_c] = true;
		}
	}

	std::map<unsigned int, unsigned int> indexMap;
	std::vector<glm::vec3> newPositions;
	std::vector<glm::vec3> newNormals;
	std::vector<unsigned char> newColorIndices;
	for (unsigned int i = 0; i < pointNum; i++) {
		if (keptVertices[i]) {
			indexMap[i] = newPositions.size();
			newPositions.push_back(positions[i]);
			newNormals.push_back(normals[i]);
			newColorIndices.push_back(colors[i]);
		}
	}

	//Remove deleted faces
	for (unsigned int i = 0; i < faceNum; i++) {
		unsigned int v_a = faces[3 * i];
		unsigned int v_b = faces[3 * i + 1];
		unsigned int v_c = faces[3 * i + 2];

		if (!visibility.test(colors[v_a]) || !visibility.test(colors[v_b]) || !visibility.test(colors[v_c])) {
			newFaces.push_back(indexMap[v_a]);
			newFaces.push_back(indexMap[v_b]);
			newFaces.push_back(indexMap[v_c]);
		}
	}

	//Calculate colors
	std::vector<unsigned char> newColors;
	for (auto colorIndex : newColorIndices) {
		vec3 color = colorMap[colorIndex];
		newColors.push_back(color.x*255.f);
		newColors.push_back(color.y*255.f);
		newColors.push_back(color.z*255.f);
	}

	using namespace tinyply;

	PlyFile plyOutput;

	plyOutput.add_properties_to_element("vertex", { "x", "y", "z" },
		Type::FLOAT32, newPositions.size(), reinterpret_cast<uint8_t*>(newPositions.data()), Type::INVALID, 0);
	plyOutput.add_properties_to_element("vertex", { "nx", "ny", "nz" },
		Type::FLOAT32, newNormals.size(), reinterpret_cast<uint8_t*>(newNormals.data()), Type::INVALID, 0);
	plyOutput.add_properties_to_element("vertex", { "red", "green", "blue" }, 
		Type::UINT8, newColors.size(), newColors.data(), Type::INVALID, 0);
	plyOutput.add_properties_to_element("face", { "vertex_indices" },
		Type::UINT32, newFaces.size()/3, reinterpret_cast<uint8_t*>(newFaces.data()), Type::UINT8, 3);

	plyOutput.write(outstream, true);

	fb.close();

	return filename;
}