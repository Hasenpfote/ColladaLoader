#pragma once
#include <dae.h>
#include <dom/domCOLLADA.h>
#include <dom/domProfile_COMMON.h>
#include <vector>
#include "collada_def.h"
#include "collada_util.h"

namespace collada{

class VertexInput{
public:
	bool load(domInstance_material::domBind_vertex_input*);
private:
	std::string semantic;			// ここら辺は将来的にハッシュ値に
	std::string input_semantic;		// ここら辺は将来的にハッシュ値に
	unsigned int set;
};

class Material{
public:
	class Sampler{
	public:
		typedef enum{
			Filter_Nearrest,
			Filter_Linear,
			Filter_Nearrest_Mipmap_Nearrest,
			Filter_Linear_Mipmap_Nearrest,
			Filter_Nearrest_Mipmap_Linear,
			Filter_Linear_Mipmap_Linear,
			Filter_Size
		}FilterMode;
		typedef enum{
			Wrap_Clamp,
			Wrap_Clamp_To_Edge,
			Wrap_Repeat
		}WrapMode;
	public:
		Sampler();
	public:
	#ifdef DEBUG
		std::string texture;
		std::string texcoord;	// ここら辺は将来的にハッシュ値に
		std::string image;		// ここら辺は将来的にハッシュ値に
	#endif
		WrapMode wrap_s;
		WrapMode wrap_t;
		WrapMode wrap_p;
		FilterMode minfilter;
		FilterMode magfilter;
		FilterMode mipfilter;
		//<border_color>
		//<mipmap_maxlevel> 	
		//<mipmap_bias>
	};

	class Param{
	public:
		typedef enum{
			Param_Color,
			Param_Texture,
			Param_Size
		}ParamType;
	public:
		Param();
		~Param();
	public:
		ParamType type;
		float color[4];
		Sampler* sampler;
	};

public:
	Material();
	~Material();
	void cleanup();
	bool load(domInstance_material*);
private:
	bool load(const domProfile_COMMON*);
	bool load(const domProfile_COMMON::domTechnique::domConstant*);
	bool load(const domProfile_COMMON::domTechnique::domLambert*);
	bool load(const domProfile_COMMON::domTechnique::domPhong*);
	bool load(const domProfile_COMMON::domTechnique::domBlinn*);
	bool load(Param*, const domCommon_color_or_texture_type*);
	bool load(Param*, const domCommon_transparent_type*);
#ifdef DEBUG
public:
	std::string symbol;	 // for debug
#endif
private:
	VertexInputPtrArray vis;	// std::map にするか？キーはsemantic
	// ToDO: 並びは後で見直す
	Param emission;
	Param ambient;
	Param diffuse;
	Param specular;
	float shininess;
	Param reflective;
	float reflectivity;
	Param transparent;
	float transparency;
	float index_of_refraction;
};

} // namespace collada