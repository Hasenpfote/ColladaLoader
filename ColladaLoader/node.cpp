#include <cstdio>
#include <cstddef>
#include <new>
#include "log.h"
#include "node.h"
#include "crc32.h"

#define INVALID_ID (unsigned int)-1

/**
 * Table of prime numbers 2^n+a, 2<=n<=30.
 */
static unsigned int primes[] = {
	(1 << 3) + 3,
	(1 << 4) + 3,
	(1 << 5) + 5,
	(1 << 6) + 3,
	(1 << 7) + 3,
	(1 << 8) + 27,
	(1 << 9) + 9,
	(1 << 10) + 9,
	(1 << 11) + 5,
	(1 << 12) + 3,
	(1 << 13) + 27,
	(1 << 14) + 43,
	(1 << 15) + 3,
	(1 << 16) + 45,
	(1 << 17) + 29,
	(1 << 18) + 3,
	(1 << 19) + 21,
	(1 << 20) + 7,
	(1 << 21) + 17,
	(1 << 22) + 15,
	(1 << 23) + 9,
	(1 << 24) + 43,
	(1 << 25) + 35,
	(1 << 26) + 15,
	(1 << 27) + 29,
	(1 << 28) + 3,
	(1 << 29) + 11,
	(1 << 30) + 85,
	0
};

static size_t new_size(size_t size){
	size_t newsize = (1 << 3);
    for(size_t i = 0; i < sizeof(primes)/sizeof(primes[0]); i++){
		if(newsize > size)
			return primes[i];
		newsize <<= 1;
	}
	// Ran out of polynomials
	return 0;	// should raise exception
}

Node::Node(){
	sibling = NULL;
	child = NULL;
	uid = INVALID_ID;
}

bool Node::load(domNode* dom_node){
	


	return true;
}

void Node::addSibling(Node* sibling){
	if(this->sibling){
		sibling->sibling = this->sibling;
	}
	this->sibling = sibling;
}

void Node::addChild(Node* child){
	if(this->child){
		child->sibling = this->child;
	}
	this->child = child;
}

void Node::update(){	// 引数は親の行列
	// 何か処理
	printf("node: %s\n", name.c_str());
	if(child)
		child->update();
	Node* next = sibling;
	while(next){
		next->update();
		next = next->sibling;
	}
}

NodeBank::NodeBank(){
	size = 0;
	nodes = NULL;
}

NodeBank::~NodeBank(){
	if(nodes)
		delete[] nodes;
}

bool NodeBank::alloc(size_t size){
	if(nodes)
		return false;
	const size_t actual_size = new_size(size);
	try{
		nodes = new Node[actual_size];
		for(size_t i = 0; i < actual_size; i++){
			nodes[i].sibling = NULL;
			nodes[i].child = NULL;
			nodes[i].uid = INVALID_ID;
		}
	}
	catch(std::bad_alloc& e){
		Log_e("exception: %s", e.what());
		return false;
	}
	this->size = actual_size;
	return true;
}

void NodeBank::free(){
	if(nodes){
		delete[] nodes;
		nodes = NULL;
	}
	size = 0;
}

Node* NodeBank::create(unsigned int uid){
	Node* node = find(uid);
	if(node == NULL){
		node = addNode(uid);
	}
	return node;
}

unsigned int NodeBank::findFirstKey(unsigned int start_index, unsigned int key){
	const size_t size = this->size;
	unsigned int index = INVALID_ID;
	for(size_t i = 0; i < size; i++){
		unsigned int actual_index = (start_index + i) % size;
		if(nodes[actual_index].uid == key){
			index = actual_index;
			break;
		}
		else
		if(nodes[actual_index].uid == INVALID_ID){
			break;
		}
	}
	return index;
}

Node* NodeBank::find(unsigned int uid){
	unsigned int index = findFirstKey(getHash(uid), uid);
	return (index != INVALID_ID)? &nodes[index] : NULL;
}

Node* NodeBank::addNode(unsigned int uid){
	unsigned int index = findFirstKey(uid, INVALID_ID);
	if(index != INVALID_ID){
		Node* node = &nodes[index];
		node->uid = uid;
		return node;
	}
	return NULL;
}
