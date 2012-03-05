#pragma once
#include <dae.h>
#include <dom/domCOLLADA.h>

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

