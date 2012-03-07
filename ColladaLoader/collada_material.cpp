#include "collada_material.h"

namespace collada{

////////////////////////////////////////////////////////////////////////////////

bool VertexInput::load(domInstance_material::domBind_vertex_input* dom_bind_vert_input){
	semantic.append(dom_bind_vert_input->getSemantic());
	input_semantic.append(dom_bind_vert_input->getInput_semantic());
	set = dom_bind_vert_input->getInput_set();
	return true;
}

////////////////////////////////////////////////////////////////////////////////

Material::Sampler::Sampler(){
	wrap_s = Wrap_Clamp;
	wrap_t = Wrap_Clamp;
	wrap_p = Wrap_Clamp;
	minfilter = Filter_Linear;
	magfilter = Filter_Linear;
	mipfilter = Filter_Linear;
}

////////////////////////////////////////////////////////////////////////////////

Material::Param::Param(){
	type = Param_Color;
	color[0] = 0.0f;
	color[1] = 0.0f;
	color[2] = 0.0f;
	color[3] = 1.0f;
	sampler = NULL;
}

Material::Param::~Param(){
	if(sampler)
		delete sampler;
}

////////////////////////////////////////////////////////////////////////////////

Material::Material(){
	shininess = 0.0f;
	reflectivity = 0.0f;
	transparency = 0.0f;
	index_of_refraction = 0.0f;
}

Material::~Material(){
	std::vector<VertexInput*>::iterator it = vis.begin();
	while(it != vis.end()){
		delete (*it);
		it++;
	}
}

void Material::cleanup(){
	std::vector<VertexInput*>::iterator it = vis.begin();
	while(it != vis.end()){
		delete (*it);
		(*it) = NULL;
		it++;
	}
	vis.clear();

	if(emission.sampler){
		delete emission.sampler;
		emission.sampler = NULL;
	}
	if(ambient.sampler){
		delete ambient.sampler;
		ambient.sampler = NULL;
	}
	if(diffuse.sampler){
		delete diffuse.sampler;
		diffuse.sampler = NULL;
	}
	if(specular.sampler){
		delete specular.sampler;
		specular.sampler = NULL;
	}
	if(reflective.sampler){
		delete reflective.sampler;
		reflective.sampler = NULL;
	}
	if(transparent.sampler){
		delete transparent.sampler;
		transparent.sampler = NULL;
	}
	shininess = 0.0f;
	reflectivity = 0.0f;
	transparency = 0.0f;
	index_of_refraction = 0.0f;
}

bool Material::load(domInstance_material* dom_inst_mtrl){
	bool result = false;
	const char* target = dom_inst_mtrl->getTarget().fragment().c_str();
#if 1
	daeDatabase* dae_db = dom_inst_mtrl->getDAE()->getDatabase();
	// <library_materials>
	domMaterial* dom_mtrl;
	if(dae_db->getElement((daeElement**)&dom_mtrl, 0, target, "material") != DAE_OK)
		goto finish;
	domInstance_effect* dom_inst_effect = dom_mtrl->getInstance_effect();
	if(!dom_inst_effect)
		goto finish;
	// <library_effects>
	const char* url = dom_inst_effect->getUrl().fragment().c_str();
	domEffect* dom_effect;
	if(dae_db->getElement((daeElement**)&dom_effect, 0, url, "effect") != DAE_OK)
		goto finish;
	// マテリアルを展開する
	// 最初に見つかった<profile_COMMON>のみ適用する
	bool find = false;
	size_t prof_count = dom_effect->getFx_profile_abstract_array().getCount();
	for(size_t i = 0; i < prof_count; i++){
		domFx_profile_abstract* dom_fx_abst = dom_effect->getFx_profile_abstract_array().get(i);
		if(strcmp(dom_fx_abst->getTypeName(), "profile_COMMON") == 0){
			domProfile_COMMON* dom_prof_common = dynamic_cast<domProfile_COMMON*>(dom_fx_abst);
			if(!load(dom_prof_common))
				goto finish;
			find = true;
			break;
		}
	}
	if(!find)
		goto finish;
#endif	
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

bool Material::load(const domProfile_COMMON* dom_prof_common){
	// for constant shading
	if(dom_prof_common->getTechnique()->getConstant())
		return load(dom_prof_common->getTechnique()->getConstant());
	// for lambert shading
	if(dom_prof_common->getTechnique()->getLambert())
		return load(dom_prof_common->getTechnique()->getLambert());
	// for phong shading
	if(dom_prof_common->getTechnique()->getPhong())
		return load(dom_prof_common->getTechnique()->getPhong());
	// for blinn shading
	if(dom_prof_common->getTechnique()->getBlinn())
		return load(dom_prof_common->getTechnique()->getBlinn());
	// error
	return false;
}

bool Material::load(const domProfile_COMMON::domTechnique::domConstant* dom_constant){
	if(dom_constant->getEmission())
		if(!load(&emission, dom_constant->getEmission()))
			return false;
	if(dom_constant->getReflective())
		if(!load(&reflective, dom_constant->getReflective()))
			return false;
	if(dom_constant->getReflectivity())
		reflectivity = static_cast<float>(dom_constant->getReflectivity()->getFloat()->getValue());
	if(dom_constant->getTransparent())
		if(!load(&transparent, dom_constant->getTransparent()))
			return false;
	if(dom_constant->getTransparency())
		reflectivity = static_cast<float>(dom_constant->getTransparency()->getFloat()->getValue());
	if(dom_constant->getIndex_of_refraction())
		index_of_refraction = static_cast<float>(dom_constant->getIndex_of_refraction()->getFloat()->getValue());
	return true;
}

bool Material::load(const domProfile_COMMON::domTechnique::domLambert* dom_lambert){
	if(dom_lambert->getEmission())
		if(!load(&emission, dom_lambert->getEmission()))
			return false;
	if(dom_lambert->getAmbient())
		if(!load(&ambient, dom_lambert->getAmbient()))
			return false;
	if(dom_lambert->getDiffuse())
		if(!load(&diffuse, dom_lambert->getDiffuse()))
			return false;
	if(dom_lambert->getReflective())
		if(!load(&reflective, dom_lambert->getReflective()))
			return false;
	if(dom_lambert->getReflectivity())
		reflectivity = static_cast<float>(dom_lambert->getReflectivity()->getFloat()->getValue());
	if(dom_lambert->getTransparent())
		if(!load(&transparent, dom_lambert->getTransparent()))
			return false;
	if(dom_lambert->getTransparency())
		reflectivity = static_cast<float>(dom_lambert->getTransparency()->getFloat()->getValue());
	if(dom_lambert->getIndex_of_refraction())
		index_of_refraction = static_cast<float>(dom_lambert->getIndex_of_refraction()->getFloat()->getValue());
	return true;
}

bool Material::load(const domProfile_COMMON::domTechnique::domPhong* dom_phong){
	if(dom_phong->getEmission())
		if(!load(&emission, dom_phong->getEmission()))
			return false;
	if(dom_phong->getAmbient())
		if(!load(&ambient, dom_phong->getAmbient()))
			return false;
	if(dom_phong->getDiffuse())
		if(!load(&diffuse, dom_phong->getDiffuse()))
			return false;
	if(dom_phong->getSpecular())
		if(!load(&specular, dom_phong->getSpecular()))
			return false;
	if(dom_phong->getShininess())
		shininess = static_cast<float>(dom_phong->getShininess()->getFloat()->getValue());
	if(dom_phong->getReflective())
		if(!load(&reflective, dom_phong->getReflective()))
			return false;
	if(dom_phong->getReflectivity())
		reflectivity = static_cast<float>(dom_phong->getReflectivity()->getFloat()->getValue());
	if(dom_phong->getTransparent())
		if(!load(&transparent, dom_phong->getTransparent()))
			return false;
	if(dom_phong->getTransparency())
		reflectivity = static_cast<float>(dom_phong->getTransparency()->getFloat()->getValue());
	if(dom_phong->getIndex_of_refraction())
		index_of_refraction = static_cast<float>(dom_phong->getIndex_of_refraction()->getFloat()->getValue());
	return true;
}

bool Material::load(const domProfile_COMMON::domTechnique::domBlinn* dom_blinn){
	if(dom_blinn->getEmission())
		if(!load(&emission, dom_blinn->getEmission()))
			return false;
	if(dom_blinn->getAmbient())
		if(!load(&ambient, dom_blinn->getAmbient()))
			return false;
	if(dom_blinn->getDiffuse())
		if(!load(&diffuse, dom_blinn->getDiffuse()))
			return false;
	if(dom_blinn->getSpecular())
		if(!load(&specular, dom_blinn->getSpecular()))
			return false;
	if(dom_blinn->getShininess())
		shininess = static_cast<float>(dom_blinn->getShininess()->getFloat()->getValue());
	if(dom_blinn->getReflective())
		if(!load(&reflective, dom_blinn->getReflective()))
			return false;
	if(dom_blinn->getReflectivity())
		reflectivity = static_cast<float>(dom_blinn->getReflectivity()->getFloat()->getValue());
	if(dom_blinn->getTransparent())
		if(!load(&transparent, dom_blinn->getTransparent()))
			return false;
	if(dom_blinn->getTransparency())
		reflectivity = static_cast<float>(dom_blinn->getTransparency()->getFloat()->getValue());
	if(dom_blinn->getIndex_of_refraction())
		index_of_refraction = static_cast<float>(dom_blinn->getIndex_of_refraction()->getFloat()->getValue());
	return true;
}

static domCommon_newparam_type* find(const domCommon_newparam_type_Array& dom_common_np_type_array, const char* type){
	const size_t count = dom_common_np_type_array.getCount();
	for(size_t i = 0; i < count; i++){
		if(strcmp(type, dom_common_np_type_array.get(i)->getSid()) == 0)
			return dom_common_np_type_array.get(i);
	}
	return NULL;
}

bool Material::load(Param* param, const domCommon_color_or_texture_type* dom_common_c_or_t_type){
	if(dom_common_c_or_t_type->getColor()){
		param->type = Param::Param_Color;
		param->color[0] = static_cast<float>(dom_common_c_or_t_type->getColor()->getValue().get(0));
		param->color[1] = static_cast<float>(dom_common_c_or_t_type->getColor()->getValue().get(1));
		param->color[2] = static_cast<float>(dom_common_c_or_t_type->getColor()->getValue().get(2));
		param->color[3] = static_cast<float>(dom_common_c_or_t_type->getColor()->getValue().get(3));
	}
	else
	if(dom_common_c_or_t_type->getTexture()){
		param->type = Param::Param_Texture;

		try{
			param->sampler = new Sampler;
		}
		catch(std::bad_alloc& e){
			return false;
		}

		const char* sampler = dom_common_c_or_t_type->getTexture()->getTexture();
		const char* texcoord = dom_common_c_or_t_type->getTexture()->getTexcoord();
#ifdef DEBUG
		param->sampler->texture.clear();
		param->sampler->texture.append(sampler);
		param->sampler->texcoord.clear();
		param->sampler->texcoord.append(texcoord);
#endif
		// <constant> or <lambert> or <phong> or <blinn>
		daeElement* dae_elem = const_cast<domCommon_color_or_texture_type*>(dom_common_c_or_t_type)->getParent();
		// <technique>
		dae_elem = dae_elem->getParent();
		// <profile_COMMON>
		dae_elem = dae_elem->getParent();
		const domProfile_COMMON* dom_prof_common = dynamic_cast<domProfile_COMMON*>(dae_elem);
		// <newparam> for <sampler*>
		domCommon_newparam_type* dom_newparam_type = find(dom_prof_common->getNewparam_array(), sampler);
		if(!dom_newparam_type)
			return false;
		if(!dom_newparam_type->getSampler2D())	// とりあえずSampler2Dのみ
			return false;
		if(!dom_newparam_type->getSampler2D()->getSource())
			return false;
		const char* surface = dom_newparam_type->getSampler2D()->getSource()->getValue();
		// <newparam> for <surface*>
		dom_newparam_type = find(dom_prof_common->getNewparam_array(), surface);
		if(!dom_newparam_type->getSurface())
			return false;
		if(!dom_newparam_type->getSurface()->getFx_surface_init_common())
			return false;
		// テクスチャを1枚制限にしているので最初の要素だけ抜き出す
		if(!dom_newparam_type->getSurface()->getFx_surface_init_common()->getInit_from_array().getCount())
			return false;
		const char* image = dom_newparam_type->getSurface()->getFx_surface_init_common()->getInit_from_array().get(0)->getValue().getID();
#ifdef DEBUG
		param->sampler->image.clear();
		param->sampler->image.append(image);
#endif
	}
	return true;
}

bool Material::load(Param* param, const domCommon_transparent_type* dom_common_trans_type){
	if(dom_common_trans_type->getColor()){
		param->type = Param::Param_Color;
		param->color[0] = static_cast<float>(dom_common_trans_type->getColor()->getValue().get(0));
		param->color[1] = static_cast<float>(dom_common_trans_type->getColor()->getValue().get(1));
		param->color[2] = static_cast<float>(dom_common_trans_type->getColor()->getValue().get(2));
		param->color[3] = static_cast<float>(dom_common_trans_type->getColor()->getValue().get(3));
	}
	else
	if(dom_common_trans_type->getTexture()){
		param->type = Param::Param_Texture;
		// ToDo:
	}
	return true;
}

} // namespace collada