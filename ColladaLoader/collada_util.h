#pragma once
#include <dae.h>
#include <dom/domCOLLADA.h>

bool isGeometryNode(const domNode* dom_node);
size_t countNode(const daeDatabase* dae_db, const domNode* dom_node);
size_t countGeometryNode(const daeDatabase* dae_db, const domNode* dom_node);
size_t countNode(const domVisual_scene* dom_visual_scene);
size_t countGeometryNode(const domVisual_scene* dom_visual_scene);

