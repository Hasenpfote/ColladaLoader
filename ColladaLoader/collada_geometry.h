#pragma once
#include <dae.h>
#include <dom/domCOLLADA.h>
#include "collada_def.h"
#include "collada_util.h"
#include "collada_material.h"

namespace collada{


class Input{
public:
	std::vector<float> f_array;
	size_t stride;
};

class Triangles{
public:
//	unsigned int material;
	Triangles();
	~Triangles();
	void cleanup();
	bool load(domTriangles*);


	Input* getPosition(){ return position; }
	const Input* getPosition() const { return position; }
	Input* getNormal(){ return normal; }
	const Input* getNormal() const { return normal; }
	InputPtrArray* getTexCoords(){ return texcoords; }
	const InputPtrArray* getTexCoords() const { return texcoords; }
	UintArray* getIndices(){ return indices; }
	const UintArray* getIndices() const { return indices; }
private:
	bool load(const domInputLocalOffset*, const domP*, domUint);
	bool load(const domInputLocal*, const domP*, domUint, domUint, domUint);
	bool optimize();
	bool isOverlapped(size_t target_index, size_t* overlapped_index);
public:
#ifdef DEBUG
	std::string material;
#endif
	Input* position;
	Input* normal;
	InputPtrArray* texcoords;
	UintArray* indices;
};

class Mesh{
public:
	Mesh();
	~Mesh();
	void cleanup();
	bool load(domMesh*);

	TrianglesPtrArray* getTriangles(){ return triangles; }
	const TrianglesPtrArray* getTriangles() const { return triangles; }
public:
	TrianglesPtrArray* triangles;
};

class Geometry{
public:
	Geometry();
	~Geometry();
	void cleanup();
	bool load(domInstance_geometry*);

	Mesh* getMesh(){ return mesh; }
	const Mesh* getMesh() const { return mesh; }
private:
	bool load(domGeometry*);
	bool load(domBind_material*);
public:
#ifdef DEBUG
	std::string url;
	std::string id;
#endif
	std::map<unsigned int, Material*> bind_material;
	Mesh* mesh;	// ÉÅÉbÉVÉÖÇÃÇ›ëŒâû
};

} // namespace collada
