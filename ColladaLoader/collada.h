#pragma once
#include <dae.h>
#include <dom/domCOLLADA.h>
#include <vector>
#include "collada_util.h"

namespace collada{

#define DEBUG

class VertexInput{
public:
	bool load(domInstance_material::domBind_vertex_input*);
public:
	std::string semantic;
	std::string input_semantic;
	unsigned int set;
};

class InstanceMaterial{
public:
	~InstanceMaterial();
	void cleanup();
	bool load(domInstance_material*);
public:
	std::string symbol;
	std::string target;
	std::vector<VertexInput*> vis;	// std::map にするか？キーはsemantic
};

class BindMateral{
public:
	~BindMateral();
	void cleanup();
	bool load(domBind_material*);
public:
	std::vector<InstanceMaterial*> inst_materials;
};

class InstanceGeometry{
public:
	InstanceGeometry();
	~InstanceGeometry();
	void cleanup();
	bool load(domInstance_geometry*);
public:
	std::string url;
	BindMateral* bind_material;
};

typedef struct{
	TransformationElementType type;
	union{
		float lookat[9];
		float matrix[16];
		float rotate[4];
		float scale[3];
		float skew[7];
		float translate[3];
	};
}TransformationElement;

class Node{
friend class NodeBank;
public:
	Node();
	~Node();
	void cleanup();
	bool load(domNode* dom_node);
	void addSibling(Node* sibling);
	void addChild(Node* child);
	void update();
private:
	bool load(const daeElementRefArray&);
	void load(TransformationElement*, const domLookat*);
	void load(TransformationElement*, const domMatrix*);
	void load(TransformationElement*, const domRotate*);
	void load(TransformationElement*, const domScale*);
	void load(TransformationElement*, const domSkew*);
	void load(TransformationElement*, const domTranslate*);
#ifdef DEBUG
public:
	std::string name;	// for debug
#endif
private:
	Node* sibling;
	Node* child;
	unsigned int uid;
	std::vector<TransformationElement*> trans_elems;
	std::vector<InstanceGeometry*> inst_geometries;
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
	Node* findNode(const char* name);
	const Node* findNode(const char* name) const;
private:
	bool load(daeDatabase* dae_db, domNode* dom_node, const char* parent);
	bool load(daeDatabase* dae_db, domInstance_node* dom_inst_node, const char* parent);
	NodeBank node_bank;
	Node* root;
};

class Collada{
public:
	Collada();
	~Collada();
	bool load(const char* uri);
	Node* findNode(const char* name);
	const Node* findNode(const char* name) const;
private:
	void cleanup();
	std::vector<Scene*> scenes;
};

} // namespace collada