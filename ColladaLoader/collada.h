#pragma once
#include <dae.h>
#include <dom/domCOLLADA.h>
#include <vector>
#include "collada_util.h"
#include "node.h"

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