#include "collada.h"
#include "crc32.h"
#include "log.h"

////////////////////////////////////////////////////////////////////////////////

#define INVALID_ID (unsigned int)-1

/**
 * Table of prime numbers 2^n+a, 2<=n<=30.
 */
static unsigned int primes[] = {
	(1 <<  3) + 3,  (1 <<  4) + 3,  (1 <<  5) + 5,  (1 <<  6) + 3,  (1 <<  7) + 3,
	(1 <<  8) + 27, (1 <<  9) + 9,  (1 << 10) + 9,  (1 << 11) + 5,  (1 << 12) + 3,
	(1 << 13) + 27,	(1 << 14) + 43, (1 << 15) + 3,  (1 << 16) + 45, (1 << 17) + 29,
	(1 << 18) + 3,  (1 << 19) + 21, (1 << 20) + 7,  (1 << 21) + 17, (1 << 22) + 15,
	(1 << 23) + 9,  (1 << 24) + 43, (1 << 25) + 35, (1 << 26) + 15, (1 << 27) + 29,
	(1 << 28) + 3,  (1 << 29) + 11, (1 << 30) + 85, 0
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

////////////////////////////////////////////////////////////////////////////////

Scene::Scene(){
	root = NULL;
}

Scene::~Scene(){
}

bool Scene::load(domVisual_scene* dom_visual_scene){
	if(!node_bank.alloc(countGeometryNode(dom_visual_scene)))
		return false;
	daeDatabase* dae_db = dom_visual_scene->getDAE()->getDatabase();
	size_t node_count = dom_visual_scene->getNode_array().getCount();
	for(size_t i = 0; i < node_count; i++){
		if(!load(dae_db, dom_visual_scene->getNode_array().get(i), NULL))
			return false;
	}
#if 1
	if(root)
		root->update();
#endif
	return true;
}

bool Scene::load(daeDatabase* dae_db, domNode* dom_node, const char* parent){
	if(!isGeometryNode(dom_node))
		return true;
#if 1
	std::string myname;
#endif
	unsigned int id;
	unsigned int pid;
	if(parent){
		std::string name;
		name.append(parent);
		name.append("-");
		name.append(dom_node->getID());
		name.append("\0");
		id = calcCRC32(reinterpret_cast<const unsigned char*>(name.c_str()));
		pid = calcCRC32(reinterpret_cast<const unsigned char*>(parent));
#if 1
		myname.append(name);
#endif
	}else{
		id = calcCRC32(reinterpret_cast<const unsigned char*>(dom_node->getID()));
		const daeElement* dae_element = dom_node->getParent();
		pid = calcCRC32(reinterpret_cast<const unsigned char*>(dae_element->getID()));
#if 1
		myname.append(dom_node->getID());
		myname.append("\0");
#endif
	}
	Node* node = node_bank.create(id);
	if(!node->load(dom_node))
		return false;
#if 1
	node->name.append(myname);
#endif
	Node* pnode = node_bank.find(pid);

	if(pnode){
		pnode->addChild(node);
	}
	else
	if(root){
		root->addSibling(node);
	}
	else{
		root = node;
	}
	size_t node_count = dom_node->getNode_array().getCount();
	for(size_t i = 0; i < node_count; i++){
		if(!load(dae_db, dom_node->getNode_array().get(i), NULL))
			return false;
	}
	size_t inode_count = dom_node->getInstance_node_array().getCount();
	for(size_t i = 0; i < inode_count; i++){
		if(!load(dae_db, dom_node->getInstance_node_array().get(i), dom_node->getID()))
			return false;
	}
	return true;
}

bool Scene::load(daeDatabase* dae_db, domInstance_node* dom_inst_node, const char* parent){
	const char* type = dom_inst_node->getUrl().fragment().c_str();
	domNode* dom_node;
	if(const_cast<daeDatabase*>(dae_db)->getElement((daeElement**)&dom_node, 0, type, "node") != DAE_OK)
		return false;
	if(!isGeometryNode(dom_node))
		return true;

	std::string name;
	name.append(parent);
	name.append("-");
	name.append(dom_node->getID());
	name.append("\0");
	unsigned int id = calcCRC32(reinterpret_cast<const unsigned char*>(name.c_str()));
	Node* node = node_bank.create(id);
	if(!node->load(dom_node))
		return false;
#if 1
	node->name.append(name);
#endif
	unsigned int pid = calcCRC32(reinterpret_cast<const unsigned char*>(parent));
	Node* pnode = node_bank.find(pid);
	if(pnode){
		pnode->addChild(node);
	}
	else{
		return false;	// ありえない
	}

	size_t node_count = dom_node->getNode_array().getCount();
	for(size_t i = 0; i < node_count; i++){
		if(!load(dae_db, dom_node->getNode_array().get(i), name.c_str()))
			return false;
	}
	size_t inode_count = dom_node->getInstance_node_array().getCount();
	for(size_t i = 0; i < inode_count; i++){
		if(!load(dae_db, dom_node->getInstance_node_array().get(i), name.c_str()))
			return false;
	}
	return true;
}

Node* Scene::findNode(const char* name){
	unsigned int id = calcCRC32(reinterpret_cast<const unsigned char*>(name));
	return node_bank.find(id);
}

const Node* Scene::findNode(const char* name) const{
	return reinterpret_cast<const Node*>(findNode(name));
}

////////////////////////////////////////////////////////////////////////////////

Collada::Collada(){
}

Collada::~Collada(){
	std::vector<Scene*>::iterator it = scenes.begin();
	while(it != scenes.end()){
		if(*it)
			delete (*it);
		it++;
	}
}

bool Collada::load(const char* uri){
	bool result = false;
	DAE* dae;
	try{
		dae = new DAE;
		if(dae->load(uri) != DAE_OK){
			goto finish;
		}
	}
	catch(std::bad_alloc& e){
		goto finish;
	}

	daeDatabase* db = dae->getDatabase();

	// <scene>
	domCOLLADA::domScene* dom_scene;
	db->getElement((daeElement**)&dom_scene, 0, NULL, "scene");
	if(!dom_scene->getInstance_visual_scene()){
		goto finish;
	}

	// <library_visual_scenes>
	domLibrary_visual_scenes* dom_visual_scenes;
	db->getElement((daeElement**)&dom_visual_scenes, 0, NULL, "library_visual_scenes");
	size_t vs_count = dom_visual_scenes->getVisual_scene_array().getCount();
	for(size_t i = 0; i < vs_count; i++){
		domVisual_scene* dom_visual_scene = dom_visual_scenes->getVisual_scene_array().get(i);
		try{
			Scene* scene = new Scene;
			scenes.push_back(scene);
			if(!scene->load(dom_visual_scene))
				goto finish;
#if 1
			Node* node0 = scene->findNode("mone_pose_mqo");
			Node* node1 = scene->findNode("node1");
			Node* node2 = scene->findNode("weapon");
			Node* node3 = scene->findNode("body");
			int aaa;
			aaa=0;
#endif
		}
		catch(std::bad_alloc& e){
			goto finish;
		}
	}

	result = true;
finish:
	if(!result)
		cleanup();
	dae->cleanup();
	delete dae;
	return result;
}

void Collada::cleanup(){
	std::vector<Scene*>::iterator it = scenes.begin();
	while(it != scenes.end()){
		if(*it)
			delete (*it);
		it++;
	}
	scenes.clear();
}

Node* Collada::findNode(const char* name){
	Node* node = NULL;
	std::vector<Scene*>::iterator it = scenes.begin();
	while(it != scenes.end()){
		node = (*it)->findNode(name);
		if(node)
			break;
		it++;
	}
	return node;
}

const Node* Collada::findNode(const char* name) const{
	return reinterpret_cast<const Node*>(findNode(name));
}

