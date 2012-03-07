#include "collada.h"
#include "crc32.h"
#include "log.h"

namespace collada{

////////////////////////////////////////////////////////////////////////////////

InstanceGeometry::InstanceGeometry(){
}

InstanceGeometry::~InstanceGeometry(){
	std::map<unsigned int, Material*>::iterator it = bind_material.begin();
	while(it != bind_material.end()){
		if(it->second){
			delete it->second;
		}
		it++;
	}
}

void InstanceGeometry::cleanup(){
	url.clear();
	std::map<unsigned int, Material*>::iterator it = bind_material.begin();
	while(it != bind_material.end()){
		if(it->second){
			delete it->second;
			it->second = NULL;
		}
		it++;
	}
	bind_material.clear();
}

bool InstanceGeometry::load(domInstance_geometry* dom_inst_geom){
	bool result = false;
	url.append(dom_inst_geom->getUrl().fragment().c_str());
//	unsigned int id = calcCRC32(reinterpret_cast<const unsigned char*>(dom_inst_geom->getUrl().fragment().c_str()));
	// <bind_material>
	domBind_material* dom_bind_mtrl = dom_inst_geom->getBind_material();
	if(dom_bind_mtrl){
		if(!load(dom_bind_mtrl))
			goto finish;
	}
	result = true;
finish:
	if(!result)
		cleanup();
	return result;
}

bool InstanceGeometry::load(domBind_material* dom_bind_mtrl){
	bool result = false;
	// <technique_common>		
	domBind_material::domTechnique_common* dom_tech_common = dom_bind_mtrl->getTechnique_common();
	// <instance_material>
	size_t mtrl_count = dom_tech_common->getInstance_material_array().getCount();
	for(size_t i = 0; i < mtrl_count; i++){
		domInstance_material* dom_inst_mtrl = dom_tech_common->getInstance_material_array().get(i);
		try{
			Material* mtrl = new Material;
#ifdef DEBUG
			mtrl->symbol.append(dom_inst_mtrl->getSymbol());
#endif
			unsigned int id = calcCRC32(reinterpret_cast<const unsigned char*>(dom_inst_mtrl->getSymbol()));
			std::pair<unsigned int, Material*> p(id, mtrl);
			std::map<unsigned int, Material*>::_Pairib pib = bind_material.insert(p);
			if(!pib.second)
				goto finish;	// キーが重複している
			if(!mtrl->load(dom_inst_mtrl))
				goto finish;
		}
		catch(std::bad_alloc& e){
			goto finish;
		}
	}
	result = true;
finish:
	if(!result)
		cleanup();
	return result;
}

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

Node::~Node(){
	for(std::vector<TransformationElement*>::iterator it = trans_elems.begin(); it != trans_elems.end(); it++){
		delete (*it);
	}
	for(std::vector<InstanceGeometry*>::iterator it = inst_geometries.begin(); it != inst_geometries.end(); it++){
		delete (*it);
	}
}

void Node::cleanup(){
	for(std::vector<TransformationElement*>::iterator it = trans_elems.begin(); it != trans_elems.end(); it++){
		delete (*it);
		(*it) = NULL;
	}
	trans_elems.clear();
	for(std::vector<InstanceGeometry*>::iterator it = inst_geometries.begin(); it != inst_geometries.end(); it++){
		delete (*it);
		(*it) = NULL;
	}
	inst_geometries.clear();
}

bool Node::load(const daeElementRefArray& dae_elem_ref_array){
	bool result = false;
	size_t cont_count = dae_elem_ref_array.getCount();
	for(size_t i = 0; i < cont_count; i++){
		daeElement* dae_elem = dae_elem_ref_array.get(i);
		TransformationElementType type = getTransformationType(dae_elem);
		if(!isTransformationElement(type))
			continue;

		TransformationElement* trans_elem;
		try{
			trans_elem = new TransformationElement;
			trans_elem->type = type;
			trans_elems.push_back(trans_elem);
		}
		catch(std::bad_alloc& e){
			goto finish;
		}

		switch(type){
		case TransformationElement_Lookat:
			load(trans_elem, dynamic_cast<domLookat*>(dae_elem));
			break;
		case TransformationElement_Matrix:
			load(trans_elem, dynamic_cast<domMatrix*>(dae_elem));
			break;
		case TransformationElement_Rotate:
			load(trans_elem, dynamic_cast<domRotate*>(dae_elem));
			break;
		case TransformationElement_Scale:
			load(trans_elem, dynamic_cast<domScale*>(dae_elem));
			break;
		case TransformationElement_Skew:
			load(trans_elem, dynamic_cast<domSkew*>(dae_elem));
			break;
		case TransformationElement_Translate:
			load(trans_elem, dynamic_cast<domTranslate*>(dae_elem));
			break;
		default:
			break;
		}
	}
	result = true;
finish:
	if(!result)
		cleanup();
	return result;
}
void Node::load(TransformationElement* tarns_elem, const domLookat* dom_lookat){
	size_t count = dom_lookat->getValue().getCount();
	for(size_t i = 0; i < count; i++)
		tarns_elem->lookat[i] = dom_lookat->getValue().get(i);
}

void Node::load(TransformationElement* tarns_elem, const domMatrix* dom_matrix){
	size_t count = dom_matrix->getValue().getCount();
	for(size_t i = 0; i < count; i++)
		tarns_elem->matrix[i] = dom_matrix->getValue().get(i);
}

void Node::load(TransformationElement* tarns_elem, const domRotate* dom_rotate){
	size_t count = dom_rotate->getValue().getCount();
	for(size_t i = 0; i < count; i++)
		tarns_elem->rotate[i] = dom_rotate->getValue().get(i);
}

void Node::load(TransformationElement* tarns_elem, const domScale* dom_scale){
	size_t count = dom_scale->getValue().getCount();
	for(size_t i = 0; i < count; i++)
		tarns_elem->scale[i] = dom_scale->getValue().get(i);
}

void Node::load(TransformationElement* tarns_elem, const domSkew* dom_skew){
	size_t count = dom_skew->getValue().getCount();
	for(size_t i = 0; i < count; i++)
		tarns_elem->skew[i] = dom_skew->getValue().get(i);
}

void Node::load(TransformationElement* tarns_elem, const domTranslate* dom_trans){
	size_t count = dom_trans->getValue().getCount();
	for(size_t i = 0; i < count; i++)
		tarns_elem->translate[i] = dom_trans->getValue().get(i);
}

bool Node::load(domNode* dom_node){
	bool result = false;
	// transformation_elements
	if(!load(dom_node->getContents()))
		goto finish;
	// <instance_geometry>
	size_t geom_count = dom_node->getInstance_geometry_array().getCount();
	for(size_t i = 0; i < geom_count; i++){
		domInstance_geometry* dom_inst_geom = dom_node->getInstance_geometry_array().get(i);
		try{
			InstanceGeometry* geom = new InstanceGeometry;
			inst_geometries.push_back(geom);
			if(!geom->load(dom_inst_geom))
				goto finish;
		}
		catch(std::bad_alloc& e){
			goto finish;
		}
	}
	result = true;
finish:
	if(!result)
		cleanup();
	return result;
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
#ifdef DEBUG
	printf("node: %s\n", name.c_str());
#endif
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

void Scene::cleanup(){
	node_bank.free();
	root = NULL;
}

bool Scene::load(domVisual_scene* dom_visual_scene){
	bool result = false;
	if(!node_bank.alloc(countGeometryNode(dom_visual_scene)))
		goto finish;
	daeDatabase* dae_db = dom_visual_scene->getDAE()->getDatabase();
	size_t node_count = dom_visual_scene->getNode_array().getCount();
	for(size_t i = 0; i < node_count; i++){
		if(!load(dae_db, dom_visual_scene->getNode_array().get(i), NULL))
			goto finish;
	}
#ifdef DEBUG
	if(root)
		root->update();
#endif
	result = true;
finish:
	if(!result)
		cleanup();
	return result;
}

bool Scene::load(daeDatabase* dae_db, domNode* dom_node, const char* parent){
	if(!isGeometryNode(dom_node))
		return true;
#ifdef DEBUG
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
#ifdef DEBUG
		myname.append(name);
#endif
	}else{
		id = calcCRC32(reinterpret_cast<const unsigned char*>(dom_node->getID()));
		const daeElement* dae_element = dom_node->getParent();
		pid = calcCRC32(reinterpret_cast<const unsigned char*>(dae_element->getID()));
#ifdef DEBUG
		myname.append(dom_node->getID());
		myname.append("\0");
#endif
	}
	Node* node = node_bank.create(id);
	if(!node->load(dom_node))
		return false;
#ifdef DEBUG
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
#ifdef DEBUG
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
	scene = NULL;
}

Collada::~Collada(){
	if(scene){
		delete scene;
	}
}

void Collada::cleanup(){
	if(scene){
		delete scene;
		scene = NULL;
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

	daeDatabase* dae_db = dae->getDatabase();

	// <scene>
	domCOLLADA::domScene* dom_scene;
	dae_db->getElement((daeElement**)&dom_scene, 0, NULL, "scene");
	if(!dom_scene->getInstance_visual_scene()){
		goto finish;
	}
	domInstanceWithExtra* dom_iwe = dom_scene->getInstance_visual_scene();
	if(!dom_iwe)
		goto finish;
	const char* id = dom_iwe->getUrl().fragment().c_str();
	domVisual_scene* dom_vis_scn;
	if(dae_db->getElement((daeElement**)&dom_vis_scn, 0, id, "visual_scene") != DAE_OK)
		goto finish;

	try{
		scene = new Scene;
		if(!scene->load(dom_vis_scn))
			goto finish;
	}
	catch(std::bad_alloc& e){
		goto finish;
	}
	result = true;
finish:
	if(!result)
		cleanup();
	dae->cleanup();
	delete dae;
	return result;
}

Node* Collada::findNode(const char* name){
	if(scene)
		return scene->findNode(name);
	return NULL;
}

const Node* Collada::findNode(const char* name) const{
	return reinterpret_cast<const Node*>(findNode(name));
}

} // namespace collada
