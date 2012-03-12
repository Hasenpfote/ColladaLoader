#pragma once
#include <dae.h>
#include <dom/domCOLLADA.h>
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
	std::vector<Input*>* getTexCoords(){ return texcoords; }
	const std::vector<Input*>* getTexCoords() const { return texcoords; }
	std::vector<unsigned int>* getIndices(){ return indices; }
	const std::vector<unsigned int>* getIndices() const { return indices; }
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
	std::vector<Input*>* texcoords;
	std::vector<unsigned int>* indices;
};

class Mesh{
public:
	Mesh();
	~Mesh();
	void cleanup();
	bool load(domMesh*);

	std::vector<Triangles*>* getTriangles(){ return triangles; }
	const std::vector<Triangles*>* getTriangles() const { return triangles; }
public:
	std::vector<Triangles*>* triangles;
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
