#include "collada_util.h"

/**
 * ジオメトリノードか
 */
bool isGeometryNode(const domNode* dom_node){
	if(dom_node->getInstance_camera_array().getCount())
		return false;
	if(dom_node->getInstance_controller_array().getCount())
		return false;
	if(dom_node->getInstance_light_array().getCount())
		return false;
	return true;
}

/**
 * ノード数を取得
 * <instance_node>は展開する
 */
size_t countNode(const daeDatabase* dae_db, const domNode* dom_node){
	size_t sum = 0;
	size_t node_count = dom_node->getNode_array().getCount();
	for(size_t i = 0; i < node_count; i++){
		sum += countNode(dae_db, dom_node->getNode_array().get(i));
	}
	size_t inode_count = dom_node->getInstance_node_array().getCount();
	for(size_t i = 0; i < inode_count; i++){
		domInstance_node* dom_inst_node = dom_node->getInstance_node_array().get(i);
		const char* type = dom_inst_node->getUrl().fragment().c_str();
		domNode* dom_node_target;
		if(const_cast<daeDatabase*>(dae_db)->getElement((daeElement**)&dom_node_target, 0, type, "node") == DAE_OK){
			sum += countNode(dae_db, dom_node_target) + 1;
		}
	}
	sum += node_count;
	return sum;
}

/**
 * ジオメトリノード数を取得
 * <instance_node>は展開する
 */
size_t countGeometryNode(const daeDatabase* dae_db, const domNode* dom_node){
	size_t sum = 0;
	size_t node_count = dom_node->getNode_array().getCount();
	for(size_t i = 0; i < node_count; i++){
		domNode* dom_node_target = dom_node->getNode_array().get(i);
		if(isGeometryNode(dom_node_target))
			sum += countGeometryNode(dae_db, dom_node->getNode_array().get(i)) + 1;
	}
	size_t inode_count = dom_node->getInstance_node_array().getCount();
	for(size_t i = 0; i < inode_count; i++){
		domInstance_node* dom_inst_node = dom_node->getInstance_node_array().get(i);

		const char* type = dom_inst_node->getUrl().fragment().c_str();
		domNode* dom_node_target;
		if(const_cast<daeDatabase*>(dae_db)->getElement((daeElement**)&dom_node_target, 0, type, "node") == DAE_OK){
			if(isGeometryNode(dom_node_target))
				sum += countGeometryNode(dae_db, dom_node_target) + 1;
		}
	}
	return sum;
}

/**
 * ノード数を取得
 * <instance_node>は展開する
 */
size_t countNode(const domVisual_scene* dom_visual_scene){
	daeDatabase* dae_db = const_cast<domVisual_scene*>(dom_visual_scene)->getDAE()->getDatabase();
	size_t sum = 0;
	size_t node_count = dom_visual_scene->getNode_array().getCount();
	for(size_t i = 0; i < node_count; i++){
		sum += countNode(dae_db, dom_visual_scene->getNode_array().get(i));
	}
	sum += node_count;
	return sum;
}

/**
 * ジオメトリノード数を取得
 * <instance_node>は展開する
 */
size_t countGeometryNode(const domVisual_scene* dom_visual_scene){
	daeDatabase* dae_db = const_cast<domVisual_scene*>(dom_visual_scene)->getDAE()->getDatabase();
	size_t sum = 0;
	size_t node_count = dom_visual_scene->getNode_array().getCount();
	for(size_t i = 0; i < node_count; i++){
		domNode* dom_node = dom_visual_scene->getNode_array().get(i);
		if(isGeometryNode(dom_node))
			sum += countGeometryNode(dae_db, dom_node) + 1;
	}
	return sum;
}
