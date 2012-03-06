#include "collada.h"
#include "crc32.h"
#include "log.h"

namespace collada{

////////////////////////////////////////////////////////////////////////////////

bool VertexInput::load(domInstance_material::domBind_vertex_input* dom_bind_vert_input){
	semantic.append(dom_bind_vert_input->getSemantic());
	input_semantic.append(dom_bind_vert_input->getInput_semantic());
	set = dom_bind_vert_input->getInput_set();
	return true;
}

////////////////////////////////////////////////////////////////////////////////

InstanceMaterial::~InstanceMaterial(){
	std::vector<VertexInput*>::iterator it = vis.begin();
	while(it != vis.end()){
		delete (*it);
		it++;
	}
}

void InstanceMaterial::cleanup(){
	symbol.clear();
	target.clear();
	std::vector<VertexInput*>::iterator it = vis.begin();
	while(it != vis.end()){
		delete (*it);
		(*it) = NULL;
		it++;
	}
	vis.clear();
}

bool InstanceMaterial::load(domInstance_material* dom_inst_mtrl){
	bool result = false;
	symbol.append(dom_inst_mtrl->getSymbol());
	target.append(dom_inst_mtrl->getTarget().fragment().c_str());
	// <bind_vertex_input>
	size_t vi_count = dom_inst_mtrl->getBind_vertex_input_array().getCount();
	for(size_t i = 0; i < vi_count; i++){
		domInstance_material::domBind_vertex_input* dom_bind_vert_input = dom_inst_mtrl->getBind_vertex_input_array().get(i);
		try{
			VertexInput* vi = new VertexInput;
			vis.push_back(vi);
			if(!vi->load(dom_bind_vert_input))
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

BindMateral::~BindMateral(){
	std::vector<InstanceMaterial*>::iterator it = inst_materials.begin();
	while(it != inst_materials.end()){
		delete (*it);
		it++;
	}
}

void BindMateral::cleanup(){
	std::vector<InstanceMaterial*>::iterator it = inst_materials.begin();
	while(it != inst_materials.end()){
		delete (*it);
		(*it) = NULL;
		it++;
	}
	inst_materials.clear();
}

bool BindMateral::load(domBind_material* dom_bind_mtrl){
	bool result = false;
	// <technique_common>		
	domBind_material::domTechnique_common* dom_tech_common = dom_bind_mtrl->getTechnique_common();
	// <instance_material>
	size_t mtrl_count = dom_tech_common->getInstance_material_array().getCount();
	for(size_t i = 0; i < mtrl_count; i++){
		domInstance_material* dom_inst_mtrl = dom_tech_common->getInstance_material_array().get(i);
		try{
			InstanceMaterial* inst_mtrl = new InstanceMaterial;
			inst_materials.push_back(inst_mtrl);
			if(!inst_mtrl->load(dom_inst_mtrl))
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

InstanceGeometry::InstanceGeometry(){
	bind_material = NULL;
}

InstanceGeometry::~InstanceGeometry(){
	if(bind_material)
		delete bind_material;
}

void InstanceGeometry::cleanup(){
	url.clear();
	if(bind_material){
		delete bind_material;
		bind_material = NULL;
	}
}

bool InstanceGeometry::load(domInstance_geometry* dom_inst_geom){
	bool result = false;
	url.append(dom_inst_geom->getUrl().fragment().c_str());
//	unsigned int id = calcCRC32(reinterpret_cast<const unsigned char*>(dom_inst_geom->getUrl().fragment().c_str()));
	// <bind_material>
	domBind_material* dom_bind_mtrl = dom_inst_geom->getBind_material();
	if(dom_bind_mtrl){
		try{
			bind_material = new BindMateral;
			if(!bind_material->load(dom_bind_mtrl))
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
	std::vector<InstanceGeometry*>::iterator it = inst_geometries.begin();
	while(it != inst_geometries.end()){
		delete (*it);
		it++;
	}
}

void Node::cleanup(){
	std::vector<InstanceGeometry*>::iterator it = inst_geometries.begin();
	while(it != inst_geometries.end()){
		delete (*it);
		(*it) = NULL;
		it++;
	}
	inst_geometries.clear();
}

bool Node::load(const daeElementRefArray& dae_elem_ref_array){
	size_t cont_count = dae_elem_ref_array.getCount();
	for(size_t i = 0; i < cont_count; i++){
		daeElement* dae_elem = dae_elem_ref_array.get(i);
		TransformationElement trans;
		trans.type = getTransformationType(dae_elem);
		switch(trans.type){
		case TransformationElement_Lookat:
			{
				domLookat* dom_lookat = dynamic_cast<domLookat*>(dae_elem);
			}
			break;
		case TransformationElement_Matrix:
			{
				domMatrix* dom_mtx = dynamic_cast<domMatrix*>(dae_elem);
			}
			break;
		case TransformationElement_Rotate:
			{
				domRotate* dom_rot = dynamic_cast<domRotate*>(dae_elem);
				float x = dom_rot->getValue().get(0);
				float y = dom_rot->getValue().get(1);
				float z = dom_rot->getValue().get(2);
				float w = dom_rot->getValue().get(3);
			}
			break;
		case TransformationElement_Scale:
			{
				domScale* dom_trans = dynamic_cast<domScale*>(dae_elem);
				float x = dom_trans->getValue().get(0);
				float y = dom_trans->getValue().get(1);
				float z = dom_trans->getValue().get(2);
			}
			break;
		case TransformationElement_Skew:
			{
				domSkew* dom_mtx = dynamic_cast<domSkew*>(dae_elem);
			}
			break;
		case TransformationElement_Translate:
			{
				domTranslate* dom_trans = dynamic_cast<domTranslate*>(dae_elem);
				float x = dom_trans->getValue().get(0);
				float y = dom_trans->getValue().get(1);
				float z = dom_trans->getValue().get(2);
			}
			break;
		default:
			break;
		}
	}
	return true;
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
	if(result)
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

} // namespace collada
