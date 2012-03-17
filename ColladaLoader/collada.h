#pragma once
#include <dae.h>
#include <dom/domCOLLADA.h>
#include <vector>
#include "collada_def.h"
#include "collada_util.h"
#include "collada_geometry.h"
#include "matrix.h"
#include "vector.h"

namespace collada{

class Node{
friend class NodeBank;
public:
	Node();
	~Node();
	void cleanup();
	bool load(domNode* dom_node);
	Node* getNext(){ return next; };
	const Node* getNext() const { return next; }
	void addSibling(Node* sibling);
	void addChild(Node* child);
	void addNext(Node* child);
	void update(bool flag = true);
	void updateMatrix(const mathematics::Matrix44* parent, bool flag = true);
	GeometryPtrArray& getGeometries(){ return geometries; }
	const GeometryPtrArray& getGeometries() const { return geometries; }
	const mathematics::Matrix44* getCurrentMatrix() const { return &current; }
private:
	bool load(const daeElementRefArray&);
	void load(float*, const domLookat*);
	void load(float*, const domMatrix*);
	void load(float*, const domRotate*);
	void load(float*, const domScale*);
	void load(float*, const domSkew*);
	void load(float*, const domTranslate*);
#ifdef DEBUG
public:
	std::string name;	// for debug
#endif
private:
	Node* sibling;
	Node* child;
	Node* next;
	unsigned int uid;
	GeometryPtrArray geometries;
	mathematics::Matrix44 local_to_world;
	mathematics::Matrix44 current;
};

class NodeBank{
public:
	NodeBank();
	~NodeBank();
	bool alloc(size_t size);
	void free();
	Node* create(unsigned int uid);
	Node* find(unsigned int uid);
private:
	unsigned int getHash(unsigned int uid){
		return uid % size;
	}
	unsigned int findFirstKey(unsigned int startIndex, unsigned int key);
	Node* addNode(unsigned int uid);
private:
	size_t size;
	Node* nodes;
};

class Scene{
public:
	Scene();
	~Scene();
	void cleanup();
	bool load(domVisual_scene* dom_visual_scene);
	Node* findNode(const char* name = NULL);
	const Node* findNode(const char* name = NULL) const;
private:
	bool load(daeDatabase* dae_db, domNode* dom_node, const char* parent);
	bool load(daeDatabase* dae_db, domInstance_node* dom_inst_node, const char* parent);
	NodeBank node_bank;
	Node* root;
};

class Images{
public:
	Images();
	~Images();
	bool load(const domLibrary_images*);
	void cleanup();
#ifdef DEBUG
	void dump();
#endif
	const std::string* getPath() const { return &path; }
	const StringArray* getImages() const { return &images; }
private:
	std::string path;
	StringArray images;
};

class Collada{
public:
	Collada();
	~Collada();
	bool load(const char* uri);
	const Scene* getScene() const { return scene; }
	const Images* getImages() const { return images; }
private:
	bool loadLibraryImages(daeDatabase*);
	bool loadScene(daeDatabase*);
private:
	void cleanup();
	Scene* scene;
	Images* images;
};

} // namespace collada