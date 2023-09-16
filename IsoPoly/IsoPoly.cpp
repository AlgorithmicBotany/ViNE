// IsoPoly.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <tinyply.h>
#define _USE_MATH_DEFINES
#include <cmath>
#include <iostream>
#include <fstream>
#include <openvdb/openvdb.h>
#include <openvdb/tools/GridTransformer.h>
#include <openvdb/tools/VolumeToMesh.h>
#include <openvdb/tools/Dense.h>
#include <chrono>


using vec3f = std::array<float, 3>;

struct QuadMesh {
	std::vector<vec3f> vertexes;
	std::vector<uint32_t> indexes;

	void export_to_obj(std::ostream& os) const;
	void export_to_ply_txt(std::ostream& os) const;
	void export_to_ply_bin(std::ostream& os) const;
};

void writeToPly(const char* filename, std::vector<vec3f>* vertices, std::vector<uint32_t>* faces) {
	std::filebuf fb;
	fb.open(filename, std::ios::out | std::ios::binary);
	std::ostream outstream(&fb);
	if (outstream.fail()) throw std::runtime_error("failed to open " + std::string(filename));

	using namespace tinyply;

	PlyFile plyOutput;

	plyOutput.add_properties_to_element("vertex", { "x", "y", "z" },
		Type::FLOAT32, vertices->size(), reinterpret_cast<uint8_t*>(vertices->data()), Type::INVALID, 0);
	plyOutput.add_properties_to_element("face", { "vertex_indices" },
		Type::UINT32, faces->size() / 4, reinterpret_cast<uint8_t*>(faces->data()), Type::UINT8, 4);

	plyOutput.write(outstream, true);

	fb.close();
}

void QuadMesh::export_to_obj(std::ostream& out) const {
	for (vec3f v : this->vertexes) {
		out << "v " << v[0] << " " << v[1] << " " << v[2] << "\n";
	}
	for (size_t i = 0; i < this->indexes.size(); i += 4) {
		out << "f ";
		out << this->indexes[i] + 1 << " ";
		out << this->indexes[i + 1] + 1 << " ";
		out << this->indexes[i + 2] + 1 << " ";
		out << this->indexes[i + 3] + 1 << " ";
		out << "\n";
	}
}

static void write_ply_header(std::ostream& out, const char* format,
	uint32_t vertex_count, uint32_t face_count) {
	out << "ply\nformat " << format << " 1.0\nelement vertex ";
	out << vertex_count;
	out << "\nproperty float x\nproperty float y\nproperty float z\nelement face ";
	out << face_count;
	out << "\nproperty list uchar uint vertex_index\nend_header\n";
}

void QuadMesh::export_to_ply_txt(std::ostream& out) const {
	write_ply_header(out, "ascii", this->vertexes.size(), this->indexes.size() / 4);
	for (vec3f v : this->vertexes) {
		out << v[0] << " " << v[1] << " " << v[2] << "\n";
	}
	for (size_t i = 0; i < this->indexes.size(); i += 4) {
		out << "4 ";
		out << this->indexes[i] << " ";
		out << this->indexes[i + 1] << " ";
		out << this->indexes[i + 2] << " ";
		out << this->indexes[i + 3] << " ";
		out << "\n";
	}
}

bool on_boundary(openvdb::Vec3s v, float xDim, float yDim, float zDim) {
	float tolerance = 0.001f; // 0.99999f;
	return v.x() < tolerance || v.x() > xDim - tolerance ||
		   v.y() < tolerance || v.y() > yDim - tolerance ||
		   v.z() < -tolerance || v.z() > zDim - tolerance;
}

void remove_outside_faces(const std::vector<openvdb::Vec3s>& vertexes, std::vector<openvdb::Vec4I>& quad_indexes,
	                      float xDim, float yDim, float zDim) 
{
	printf("Dim = (%f, %f, %f)\n", xDim, yDim, zDim);
	quad_indexes.erase(std::remove_if(quad_indexes.begin(), quad_indexes.end(), [&](const openvdb::Vec4I& i) {
		return on_boundary(vertexes[i.x()], xDim, yDim, zDim) &&
			   on_boundary(vertexes[i.y()], xDim, yDim, zDim) &&
			   on_boundary(vertexes[i.z()], xDim, yDim, zDim) &&
			   on_boundary(vertexes[i.w()], xDim, yDim, zDim);
	}), quad_indexes.end());
}

bool remove_face(openvdb::Vec3s a, openvdb::Vec3s b, openvdb::Vec3s c, openvdb::Vec3s d, float xEnd, float yEnd, float zEnd)
{
	if (a.x() > xEnd || b.x() > xEnd || c.x() > xEnd || d.x() > xEnd ||
		a.y() > yEnd || b.y() > yEnd || c.y() > yEnd || d.y() > yEnd ||
		a.z() > zEnd || b.z() > zEnd || c.z() > zEnd || d.z() > zEnd)
	{
		return true;
	}

	return on_boundary(a, xEnd, yEnd, zEnd) && on_boundary(b, xEnd, yEnd, zEnd) && 
		   on_boundary(c, xEnd, yEnd, zEnd) && on_boundary(d, xEnd, yEnd, zEnd);
}

void remove_outside_faces_padded(const std::vector<openvdb::Vec3s>& vertexes, std::vector<openvdb::Vec4I>& quad_indexes,
	float xStart, float xEnd, float yStart, float yEnd, float zStart, float zEnd)
{
	openvdb::Vec3s offset(xStart, yStart, zStart);
	quad_indexes.erase(std::remove_if(quad_indexes.begin(), quad_indexes.end(), [&](const openvdb::Vec4I& i) {
		return remove_face(vertexes[i.x()] - offset, vertexes[i.y()] - offset, vertexes[i.z()] - offset, vertexes[i.w()] - offset, xEnd, yEnd, zEnd);
	}), quad_indexes.end());
}

template <class T>
void write_binary(std::ostream& out, T v) {
	out.write(reinterpret_cast<char*>(&v), sizeof(v));
}

void QuadMesh::export_to_ply_bin(std::ostream& out) const {
	write_ply_header(out, "binary_little_endian", this->vertexes.size(), this->indexes.size() / 4);
	for (vec3f v : this->vertexes) {
		write_binary(out, v);
	}
	for (size_t i = 0; i < this->indexes.size(); i += 4) {
		write_binary(out, uint8_t(4));
		write_binary(out, uint32_t(this->indexes[i]));
		write_binary(out, uint32_t(this->indexes[i + 1]));
		write_binary(out, uint32_t(this->indexes[i + 2]));
		write_binary(out, uint32_t(this->indexes[i + 3]));
	}
}

static vec3f to_vec3f(const openvdb::Vec3s& p) {
	return { p[0], p[1], p[2] };
}

// transform container
static void to_vec_vec3f(std::vector<vec3f>& to, const std::vector<openvdb::Vec3s>& from) {
	to.resize(from.size());
	std::transform(from.begin(), from.end(), to.begin(), to_vec3f);
}

// transform and flatten container
static void to_vec_4uint(std::vector<unsigned int>& to, const std::vector<openvdb::Vec4I>& from) {
	to.resize(4 * from.size());
	size_t i = 0;
	for (const openvdb::Vec4I& qi : from) {
		to[i++] = qi[0];
		to[i++] = qi[3];
		to[i++] = qi[2];
		to[i++] = qi[1];
	}
}

static QuadMesh create_mesh(const std::vector<openvdb::Vec3s>& vertexes,
	const std::vector<openvdb::Vec4I>& indexes) {
	QuadMesh m;
	to_vec_vec3f(m.vertexes, vertexes);
	to_vec_4uint(m.indexes, indexes);
	return m;
}

namespace io2 {

using byte = char;

size_t byteSizeOfFile(std::ifstream &fileIn) {
	using namespace std;

	fileIn.seekg(0, ios::end);
	size_t bytes = fileIn.tellg();
	fileIn.seekg(0, ios::beg);

	return bytes;
}

template <typename T> bool bufferDivibleBy(uint32_t bufferSize) {
	return bufferSize % sizeof(T) == 0;
}

template <typename T> std::vector<T> readRawFile(std::string const &filePath, bool swapEndianness=false) {
	using namespace std;

	ifstream fileIn(filePath, ios::binary);
	if (!fileIn)
		return std::vector<T>();

	auto bytes = byteSizeOfFile(fileIn);

	if (!bufferDivibleBy<T>(bytes))
		std::cerr << "[ERROR] file size in not divisible by " << sizeof(T)
		<< std::endl;
	assert(bufferDivibleBy<T>(bytes));

	auto count = bytes / sizeof(T);
	std::vector<T> buffer(count);

	fileIn.read(reinterpret_cast<byte *>(buffer.data()), bytes);

	if (swapEndianness) {
		byte* data = reinterpret_cast<byte*>(buffer.data());
		printf("Swapping endianness\n");
		for (size_t i = 0; i + 1 < buffer.size() * 2; i += 2) {
			byte first = data[i];
			data[i] = data[i + 1];
			data[i + 1] = first;
		}
	}

	return buffer;
}

template <typename T>
void writeRawFile(std::string const &filePath, std::vector<T> const &buffer) {
	using namespace std;

	ofstream fileOut(filePath, ios::binary);
	if (!fileOut)
		return;

	fileOut.write(reinterpret_cast<byte const *>(buffer.data()),
		sizeof(T) * buffer.size());
}

} // namespace io

float gaussian(float standardDeviation, float center, float x) {
	return 1.f / sqrt(standardDeviation*M_PI)*exp(-0.5f*(x - center) / (standardDeviation*standardDeviation));
}

float average(const std::vector<float>& values) {
	float sum = 0.f;
	for (float value : values)
		sum += value;

	return sum / float(values.size());
}

float floatMode(const std::vector<float>& values, float standardDeviation) {
	std::vector<float> weights(values.size(), 0.f);
	float maxWeight = 0.f;
	int maxIndex = -1;

	for (int i = 0; i < values.size(); i++) {
		float weight = 0.f;
		for (float value : values){
			weight += gaussian(standardDeviation, values[i], value);
		}
		if (weight > maxWeight) {
			maxWeight = weight;
			maxIndex = i;
		}
	}

	return (maxIndex >= 0) ? values[maxIndex] : 0.f;
}

int main(int argc, char *argv[])
{
	std::string filePath = (argc > 1) ? argv[1] : "./input/CT-Head-LSB.raw";
	int WIDTH = (argc > 2) ? std::stoi(argv[2]) : 256;
	int HEIGHT = (argc > 3) ? std::stoi(argv[3]) : 256;
	int DEPTH = (argc > 4) ? std::stoi(argv[4]) : 113;
	float isovalue = (argc > 5) ? std::stof(argv[5]) : 113;
	int bits = (argc > 6) ? std::stoi(argv[6]) : 16;
	size_t voxelCompression = (argc > 7) ? std::stoi(argv[7]) : 2;
	int xSectors = (argc > 8) ? std::stoi(argv[8]) : 1;
	int ySectors = (argc > 9) ? std::stoi(argv[9]) : 1;
	int zSectors = (argc > 10) ? std::stoi(argv[10]) : 1;

	printf("Loading %s\n Width %d Height %d Depth %d\n Isovalue %f Bits %d Downsampling %d Quadrants %d %d %d\n",
		filePath.c_str(), WIDTH, HEIGHT, DEPTH, isovalue, bits, voxelCompression, xSectors, ySectors, zSectors);

	using namespace openvdb;

	openvdb::initialize();

	std::vector<std::array<int, 3>> sectors;
	for (int qx = 0; qx < xSectors; qx++) {
		for (int qy = 0; qy < ySectors; qy++) {
			for (int qz = 0; qz < zSectors; qz++) {
				sectors.push_back({ qx, qy, qz });

			}
		}
	}

	printf("-----Loading raw file-----\n");
	std::vector<float> rawFloatData;

	std::vector<uint8_t> rawData8Bit;
	std::vector<uint16_t> rawData16Bit;

	if (bits == 8) {
		rawData8Bit = std::move(io2::readRawFile<uint8_t>(filePath));
	}
	else if (bits == 16) {
		rawData16Bit = io2::readRawFile<uint16_t>(filePath, false);
	}

	for (auto sector : sectors)
	{
		int qx = sector[0];
		int qy = sector[1];
		int qz = sector[2];

		std::printf("------------------PROCESSING SECTOR (%d, %d, %d)\n", qx, qy, qz);
		using ushort = uint8_t;
		auto start = std::chrono::steady_clock::now();
		auto dense = std::make_shared< tools::Dense<float, tools::LayoutZYX>>(CoordBBox(0, 0, 0, WIDTH / voxelCompression, HEIGHT / voxelCompression, DEPTH / voxelCompression), 0.f);

		using UShortTree = tree::Tree4<ushort, 5, 4, 3>::Type;
		using UShortGrid = Grid<UShortTree>;

		FloatGrid smallGrid(CoordBBox(0, 0, 0, WIDTH / (voxelCompression*xSectors), HEIGHT / (voxelCompression*ySectors), DEPTH / (voxelCompression*zSectors)));

		auto end = std::chrono::steady_clock::now();
		std::cout << std::chrono::duration_cast<std::chrono::seconds>(end - start).count() << std::endl;
		start = end;
		printf("-----Downsampling voxel data-----\n");
		int yStart = qy * (HEIGHT / voxelCompression / ySectors - 1);
		int yEnd = yStart + (HEIGHT / voxelCompression / ySectors + 1);
		int xStart = qx * (WIDTH / voxelCompression / xSectors - 1);
		int xEnd = xStart + (WIDTH / voxelCompression / xSectors + 1);
		int zStart = qz * (DEPTH / voxelCompression / zSectors - 1);
		int zEnd = zStart + (DEPTH / voxelCompression / zSectors + 1);

		yStart *= voxelCompression;
		yEnd *= voxelCompression;
		xStart *= voxelCompression;
		xEnd *= voxelCompression;
		zStart *= voxelCompression;
		zEnd *= voxelCompression;

		printf("x (%d -> %d), y (%d -> %d), z (%d -> %d)\n", xStart, xEnd, yStart, yEnd, zStart, zEnd);

		//Bits == 8 quadrants
		if (rawData8Bit.size() > 0)
		{
			for (size_t z = zStart; z < zEnd; z++) {
				for (size_t y = yStart; y < yEnd; y++) {
					for (size_t x = xStart; x < xEnd; x++) {
						dense->setValue(x / voxelCompression, y / voxelCompression, z / voxelCompression,
							dense->getValue(x / voxelCompression, y / voxelCompression, z / voxelCompression)
							+ float(rawData8Bit[z*WIDTH*HEIGHT + y * WIDTH + x]) / float(voxelCompression*voxelCompression*voxelCompression));
					}
				}
			}
		}

		//Bits == 8
		if (rawData16Bit.size() > 0) {
			for (size_t z = 0; z < DEPTH; z++){
				for (size_t y = yStart; y < yEnd; y++) {
					for (size_t x = xStart; x < xEnd; x++) {
						dense->setValue(x / voxelCompression, y / voxelCompression, z / voxelCompression,
							dense->getValue(x / voxelCompression, y / voxelCompression, z / voxelCompression)
							+ float(rawData16Bit[z*WIDTH*HEIGHT + y * WIDTH + x]) / float(voxelCompression*voxelCompression*voxelCompression));
					}
				}
			}
		}

		rawFloatData.clear();

		end = std::chrono::steady_clock::now();
		std::cout << std::chrono::duration_cast<std::chrono::seconds>(end - start).count() << std::endl;
		start = end;
		printf("-----Copying to dense-----\n");
		tools::copyFromDense(*dense, smallGrid, 0.f);
		dense.reset();

		std::vector<openvdb::Vec3s> vertexes;
		std::vector<openvdb::Vec4I> quad_indexes;

		int gridWidth = smallGrid.evalActiveVoxelDim().x();
		int gridHeight = smallGrid.evalActiveVoxelDim().y();
		int gridDepth = smallGrid.evalActiveVoxelDim().z();

		auto bbox = smallGrid.evalActiveVoxelBoundingBox();
		printf("(%d, %d, %d)\n", bbox.min().x(), bbox.min().y(), bbox.min().z());
		printf("(%d, %d, %d)\n", bbox.max().x(), bbox.max().y(), bbox.max().z());

		printf("Small grid size = (%d %d %d)\n",
			gridWidth, gridHeight, gridDepth);

		end = std::chrono::steady_clock::now();
		std::cout << std::chrono::duration_cast<std::chrono::seconds>(end - start).count() << std::endl;
		start = end;
		printf("-----Creating marching cubes mesh-----\n");
		tools::volumeToMesh(smallGrid, vertexes, quad_indexes, isovalue);
		smallGrid.clear();

		if (xSectors > 1 || ySectors > 1 || zSectors > 1) {
			printf("-----Remove external faces-----\n");
			float minX = xStart / voxelCompression + (qx != 0);
			float maxX = xEnd / voxelCompression - (qx != xSectors-1);
			float minY = yStart / voxelCompression+ (qy != 0);
			float maxY = yEnd / voxelCompression- (qy != ySectors-1);
			float minZ = zStart / voxelCompression + (qz != 0);
			float maxZ = zEnd / voxelCompression - (qz != zSectors-1);
			printf("%f < x < %f\n%f < y < %f\n%f < z < %f\n", minX, maxX, minY, maxY, minZ, maxZ);

			remove_outside_faces_padded(vertexes, quad_indexes, minX, maxX, minY, maxY, minZ, maxZ);
		}
		auto mesh = create_mesh(vertexes, quad_indexes);
		std::string plyFile = std::string(filePath) + std::to_string(qx) + "_" + std::to_string(qy) +"_"+std::to_string(qz) + std::string(".ply");
		end = std::chrono::steady_clock::now();
		std::cout << std::chrono::duration_cast<std::chrono::seconds>(end - start).count() << std::endl;
		start = end;

		printf("-----Writing to file-----\n");
		writeToPly(plyFile.c_str(), &mesh.vertexes, &mesh.indexes);
		end = std::chrono::steady_clock::now();
		std::cout << std::chrono::duration_cast<std::chrono::seconds>(end - start).count() << std::endl;

	}
}
