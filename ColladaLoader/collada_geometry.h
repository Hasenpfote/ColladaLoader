#pragma once
#include <dae.h>
#include <dom/domCOLLADA.h>
#include "collada_def.h"
#include "collada_util.h"
#include "collada_material.h"

namespace collada{


class Input{
public:
	size_t stride;
	FloatArray f_array;
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
	unsigned int getMaterialUid() const { return mtrl_uid; }
private:
	bool load(const domInputLocalOffset*, const domP*, domUint);
	bool load(const domInputLocal*, const domP*, domUint, domUint, domUint);
	bool optimize();
	bool isOverlapped(size_t target_index, size_t* overlapped_index);
private:
	Input* position;
	Input* normal;
	InputPtrArray* texcoords;
	UintArray* indices;
	unsigned int mtrl_uid;
#ifdef DEBUG
	std::string material;
#endif
};

class Mesh{
public:
	Mesh();
	~Mesh();
	void cleanup();
	bool load(domMesh*);

	TrianglesPtrArray* getTriangles(){ return triangles; }
	const TrianglesPtrArray* getTriangles() const { return triangles; }
private:
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
	std::map<unsigned int, Material*>& getBindMaterial(){ return bind_material; }
	const std::map<unsigned int, Material*>& getBindMaterial() const { return bind_material; }
private:
	bool load(domGeometry*);
	bool load(domBind_material*);
private:
	std::map<unsigned int, Material*> bind_material;
	Mesh* mesh;	// ÉÅÉbÉVÉÖÇÃÇ›ëŒâû
#ifdef DEBUG
	std::string url;
	std::string id;
#endif
};

} // namespace collada
