#include "collada.h"
#include "crc32.h"


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
	domNode* dom_node = findGeometryNode(dae_db, 0, dom_inst_node->getUrl().fragment().c_str());
	if(!dom_node)
		return false;

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

