#pragma once

#include <glm/glm.hpp>
#include <vector>
#include <map>
#include <memory>

//Container which reserves space in middle
template<typename T>
class SlotMap {

	std::vector<T> data;
	std::vector<int> timestamp;
	std::vector<int> emptySlots;
	size_t dataSize;

public:
	SlotMap() :dataSize(0) {}

/*	struct IndexData {
		int index;
		int timestamp;
	};
	*/
	struct Index {
		int index;
		int timestamp;

		Index() :index(-1), timestamp(0) {}
	//	Index(IndexData id) :index(id.index), timestamp(id.timestamp) {}
		Index(int index) :index(index), timestamp(0) {}
		Index(int index, int timestamp) :index(index), timestamp(timestamp) {}
		operator int() const {return index; }
//		operator IndexData() const{ return {.index = index,.timestamp = timestamp}; }
		operator size_t() const { return index; }
		operator bool() { return index >= 0; }
		bool operator==(Index other) { return other.index == index && other.timestamp == timestamp; }
		bool operator<(Index other) { return index < other.index; }
		bool operator>(Index other) { return index > other.index; }
		bool operator<=(Index other) { return index <= other.index; }
		bool operator>=(Index other) { return index >= other.index; }
	};

	Index add(T value) {
		dataSize++;
		if (emptySlots.empty()) {
			data.push_back(value);
			timestamp.push_back(0);
			return Index(data.size()-1);
		}
		else {
			int index = emptySlots.back();
			data[index] = value;
			timestamp[index]++;
			emptySlots.pop_back();
			return Index(index, timestamp[index]);
		}
	}

	void remove(Index i) {
		emptySlots.push_back(i);
		timestamp[i]++;
		dataSize = (dataSize == 0) ? dataSize : dataSize-1;
	}

	T& operator[](Index i) {
		return data[i.index];
	}

	/*const T& operator[] const (Index i) {
		return data[i];
	}*/

	int size() { return dataSize; }

	//Iterator
	class Iterator {
		int index;
		SlotMap<T>* container;
		
		friend SlotMap<T>;

		Iterator(int index, SlotMap<T>* container) :index(index), container(container) {}
	public:
		Iterator& operator++() {
			do {
				index++;
			} while (index < container->data.size() && (container->timestamp[index] % 2) == 1);

			return (index < container->data.size()) ? *this : container->end();
		}

		Iterator& operator--() {
			do {
				index--;
			} while (index > 0 && (container->timestamp[index] % 2) == 1);

			return (index > 0) ? *this : container->end();
		}

		operator Index() const { return{ index, timestamp[index] }; }

		T& operator*() { return container->data[index]; }
		T* operator->() { return &container->data[index]; }

		bool operator==(const Iterator& it) const { return index == it.index && container == it.container; }
		bool operator!=(const Iterator& it) const { return !(*this == it); }
		bool operator<(const Iterator& it) const { return index < it.index; }
		bool operator>(const Iterator& it) const { return index > it.index; }
		bool operator<=(const Iterator& it) const { return index <= it.index; }
		bool operator>=(const Iterator& it) const { return index >= it.index; }

		//T operator
	};

	Iterator begin() {
		if (data.size() == 0) return Iterator(0, this);

		int i = 0;
		while (i < data.size() && (timestamp[i] % 2) == 1) {
			i++;
		}
		return (i < data.size()) ? Iterator(i, this)  : Iterator(0, this);
	}

	Iterator end() {
		if (data.size() == 0) return Iterator(0, this);

		int i = data.size() - 1;
		while (i > 0 && (timestamp[i] % 2) == 1) {
			i--;
		}
		return {i+1, this };
	}
};
/*

template<typename P> class Face;
template<typename P> class Vertex;

template<typename P>
struct HalfEdge {
	typename SlotMap<Vertex<P>>::Index head;
	typename SlotMap<HalfEdge<P>>::Index next;
	typename SlotMap<HalfEdge<P>>::Index pair;
	typename SlotMap<Face<P>>::Index face;
};

template<typename P>
struct Vertex {
	P pos;
	typename SlotMap<HalfEdge<P>>::Index edge;
};

template<typename P>
struct Face {
	typename SlotMap<HalfEdge<P>>::Index edge;
};


template<typename P>
class HalfEdgeMesh {	
public:
	SlotMap<Vertex<P>> vertices;
	SlotMap<Face<P>> faces;
	SlotMap<HalfEdge<P>> edges;

	//Accessor methods
	HalfEdge<P>& next(HalfEdge<P> edge) { return edges[edge.next]; }
	const HalfEdge<P>& next(HalfEdge<P> edge) const { return edges[edge.next]; }
	HalfEdge<P>& pair(HalfEdge<P> edge) { return edges[edge.pair]; }
	const HalfEdge<P>& pair(HalfEdge<P> edge) const { return edges[edge.pair]; }
	Vertex<P>& head(HalfEdge<P> edge) { return vertices[edge.head]; }
	const Vertex<P>& head(HalfEdge<P> edge) const { return vertices[edge.head]; }
	Face<P>& face(HalfEdge<P> edge) { return vertices[edge.face]; }
	const Face<P>& face(HalfEdge<P> edge) const { return vertices[edge.face]; }

	HalfEdge<P>& edge(Vertex<P> vert) { return vertices[vert.edge]; }
	const HalfEdge<P>& edge(Vertex<P> vert) const { return vertices[vert.edge]; }
	HalfEdge<P>& edge(Face<P> face) { return vertices[face.edge]; }
	const HalfEdge<P>& edge(Face<P> face) const { return vertices[face.edge]; }

	const HalfEdge<P>& operator[](typename SlotMap<HalfEdge<P>>::Index index) const { return edges[index]; }
	HalfEdge<P>& operator[](typename SlotMap<HalfEdge<P>>::Index index) { return edges[index]; }

	const Vertex<P>& operator[](typename SlotMap<Vertex<P>>::Index index) const { return vertices[index]; }
	Vertex<P>& operator[](typename SlotMap<Vertex<P>>::Index index) { return vertices[index]; }

	const Face<P>& operator[](typename SlotMap<Face<P>>::Index index) const { return faces[index]; }
	Face<P>& operator[](typename SlotMap<Face<P>>::Index index) { return faces[index]; }

	Vertex<P>& nextOnBoundary(Vertex<P> v) {
		HalfEdge<P> e = next(edge(v));
		while (!pair(e) && head(e != v)) {
			e = next(pair(e));
		}

		return head(e);
	}
};

template<typename P, typename I>
void halfEdgeToFaceList(std::vector<P>* vertices, std::vector<I>* indices, const HalfEdgeMesh<P>& mesh) {
	I startIndex = vertices.size();

	I count = 0;
	std::map<SlotMap<Vertex<P>>::Index, I> indexMap;
	for (auto& vert = mesh.vertices.begin(); vert != mesh.vertices.end(); vert++) {
		vertices->push_back(vert.pos);
		indexMap[vert.toIndex()] = count++;
	}

	for (auto& face = mesh.faces.begin(); face != mesh.faces.end(); face++) {
		auto vertIndex = mesh.head(mesh.edge(face));
		indices->push_back(indexMap[vertIndex]);
		vertIndex = mesh.head(mesh.next(mesh.edge(vertIndex)));
		indices->push_back(indexMap[vertIndex]);
		vertIndex = mesh.head(mesh.next(mesh.edge(vertIndex)));
		indices->push_back(indexMap[vertIndex]);
	}	
}

//P is vertex type, I is index type
template <typename P, typename I>
typename SlotMap<Vertex<P>>::Index faceListToHalfEdge(HalfEdgeMesh<P>* mesh, const std::vector<P>& vertices, const std::vector<I>& indices) {
	std::vector<SlotMap<Vertex<P>>::Index> vertIndices;

	struct VertexPair {
		I a, b;
		VertexPair(I a, I b) :a(a), b(b) {}
		bool operator<(VertexPair other) { return (a == other.a) ? b < other.b : a < other.a; }
	};

	for (int i = 0; i < vertices.size(); i++)
		vertIndices.push_back(mesh->vertices.add({ vertices[i], {} }));

	std::map <VertexPair, SlotMap<Vertex<P>>::Index> edgeMap;

	for (int i = 0; i+2 < indices.size(); i+=3) {

		auto face = mesh->faces.add({});

		SlotMap<HalfEdge<P>>::Index edges[3];

		for (int j = 0; j < 3; j++) {
			I a = indices[i + j];
			I b = indices[i + (j + 1) % 3];

			auto v_a = vertIndices[a];
			auto v_b = vertIndices[b];

			if (edgeMap.find(VertexPair(a, b)) == edgeMap.end()) {
				edges[j] = mesh->edges.add({
					v_a,	//Head
					{},		//Next
					{},		//Pair
					face	//Face
				});
				edgeMap[VertexPair(b, a)] = mesh->edges.add({
					v_b,	//Head
					{},		//Next
					edges[j],	//Pair
					{}
				});
				(*mesh)[v_a].edge = edges[i];
			}
			else {
				mesh->pair(mesh->pair(edgeMap[VertexPair(a, b)])) = edgeMap[VertexPair(a, b)];
				mesh->face(edgeMap[VertexPair(a, b)]) = face;
				edgeMap.remove(VertexPair(a, b));
			}
		}

		for (auto edgeIter = edgeMap.begin(); edgeIter != edgeMap.end(); edgeIter++) {
			mesh->edges.remove(edgeIter->second);
		}

		(*mesh)[edge[0]].next = edge[1];
		(*mesh)[edge[1]].next = edge[2];
		(*mesh)[edge[2]].next = edge[3];

		(*mesh)[face].edge = edge[0];
	}

	return vertIndices.back();
}

template <typename P>
typename SlotMap<Vertex<P>>::Index generateTetrahedron(HalfEdgeMesh<P>& mesh, P a, P b, P c, P d) {
	//Make sure ordering is such that faces point out
	if (dot(cross(b - a, c - a), d - a) > 0) {
		P temp = a;
		a = b;
		b = temp;
	}

//	return mesh->vertices.add({ a, {}});

	auto v_a = mesh.vertices.add({ a, {} });
	auto v_b = mesh.vertices.add({ b, {} });
	auto v_c = mesh.vertices.add({ c, {} });
	auto v_d = mesh.vertices.add({ d, {} });

	auto f_abc = mesh.faces.add({});
	auto e_ab = mesh.edges.add({});
	auto e_bc = mesh.edges.add({});
	auto e_ca = mesh.edges.add({});

	auto f_adb = mesh.faces.add({});
	auto e_ad = mesh.edges.add({});
	auto e_db = mesh.edges.add({});
	auto e_ba = mesh.edges.add({});

	auto f_bdc = mesh.faces.add({});
	auto e_bd = mesh.edges.add({});
	auto e_dc = mesh.edges.add({});
	auto e_cb = mesh.edges.add({});

	auto f_cda = mesh.faces.add({});
	auto e_cd = mesh.edges.add({});
	auto e_da = mesh.edges.add({});
	auto e_ac = mesh.edges.add({});

	mesh[v_a].edge = e_ca;
	mesh[v_b].edge = e_db;
	mesh[v_c].edge = e_dc;
	mesh[v_d].edge = e_cd;
	//Vertices complete

	mesh[f_abc].edge = e_ab;
	mesh[f_adb].edge = e_ad;
	mesh[f_bdc].edge = e_bd;
	mesh[f_cda].edge = e_cd;
	//Faces complete

	mesh[e_ab] = {
		v_b,
		e_bc,
		e_ba,
		f_abc
	};

	mesh[e_bc] = {
		v_c,
		e_ca,
		e_cb,
		f_abc
	};

	mesh[e_ca] = {
		v_a,
		e_ab,
		e_ac,
		f_abc
	};

	mesh[e_ad] = {
		v_d,
		e_db,
		e_da,
		f_adb
	};

	mesh[e_db] = {
		v_b,
		e_ba,
		e_ab,
		f_adb
	};

	mesh[e_ba] = {
		v_a,
		e_ad,
		e_ab,
		f_adb
	};

	mesh[e_bd] = {
		v_d,
		e_dc,
		e_db,
		f_bdc
	};

	mesh[e_dc] = {
		v_c,
		e_cb,
		e_cd,
		f_bdc
	};

	mesh[e_cb] = {
		v_b,
		e_bd,
		e_bc,
		f_bdc
	};

	mesh[e_cd] = {
		v_d,
		e_da,
		e_dc,
		f_cda
	};

	mesh[e_da] = {
		v_a,
		e_ac,
		e_ad,
		f_cda
	};

	mesh[e_ac] = {
		v_c,
		e_cd,
		e_ca,
		f_cda
	};

	return v_a;
	
}
*/
