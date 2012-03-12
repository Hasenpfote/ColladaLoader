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
public:
	std::vector<Triangles*>* triangles;
};

class Geometry{
public:
	Geometry();
	~Geometry();
	void cleanup();
	bool load(domInstance_geometry*);
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
