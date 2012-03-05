#pragma once
#include <dae.h>
#include <dom/domCOLLADA.h>

domNode* findNode(const daeDatabase* dae_db, daeInt index = 0, const daeString type = NULL);
bool isGeometryNode(const domNode* dom_node);
domNode* findGeometryNode(const daeDatabase* dae_db, daeInt index = 0, const daeString type = NULL);
size_t countNode(const daeDatabase* dae_db, const domNode* dom_node);
size_t countGeometryNode(const daeDatabase* dae_db, const domNode* dom_node);
size_t countNode(const domVisual_scene* dom_visual_scene);
size_t countGeometryNode(const domVisual_scene* dom_visual_scene);

