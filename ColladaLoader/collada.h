#pragma once
#include <dae.h>
#include <dom/domCOLLADA.h>
#include <vector>
#include "collada_util.h"


class Node{
friend class NodeBank;
public:
	Node();
	bool load(domNode* dom_node);
	void addSibling(Node* sibling);
	void addChild(Node* child);
	void update();
public:
	std::string name;
private:
	Node* sibling;
	Node* child;
	unsigned int uid;
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