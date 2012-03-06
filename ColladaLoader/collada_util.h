#pragma once
#include <dae.h>
#include <dom/domCOLLADA.h>

namespace collada{

typedef enum{
	TransformationElement_Lookat,
	TransformationElement_Matrix,
	TransformationElement_Rotate,
	TransformationElement_Scale,
	TransformationElement_Skew,
	TransformationElement_Translate,
	TransformationElement_Unknown,
	TransformationElement_Size
}TransformationElementType;

bool isGeometryNode(const domNode* dom_node);
size_t countNode(const daeDatabase* dae_db, const domNode* dom_node);
size_t countGeometryNode(const daeDatabase* dae_db, const domNode* dom_node);
size_t countNode(const domVisual_scene* dom_visual_scene);
size_t countGeometryNode(const domVisual_scene* dom_visual_scene);
bool isTransformationElement(domElement* dom_elem);
bool isTransformationElement(TransformationElementType type);
TransformationElementType getTransformationType(domElement* dom_elem);

} // namespace collada