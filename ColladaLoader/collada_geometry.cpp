#include "collada.h"
#include "crc32.h"
#include "log.h"
#include <bitset>

namespace collada{

////////////////////////////////////////////////////////////////////////////////

static domUint getMaxOffset(const domInputLocalOffset_Array& dom_ilo_array){
	domUint offset = 0;
	const size_t input_coutn = dom_ilo_array.getCount();
	for(size_t i = 0; i < input_coutn; i++){
		domInputLocalOffset* dom_ilo = dom_ilo_array.get(i);
		if(dom_ilo->getOffset() > offset)
			offset = dom_ilo->getOffset();
	}
	return offset;
}

static domUint getOffset(const domAccessor* dom_accessor){
	domUint offset = 0;
	const size_t count = dom_accessor->getParam_array().getCount();
	for(size_t i = 0; i < count; i++){
		if(dom_accessor->getParam_array().get(i)->getName())
			break;
		offset++;
	}
	return offset;
}

static size_t getActualCount(const domAccessor* dom_accessor){
	return dom_accessor->getParam_array().getCount() - static_cast<size_t>(getOffset(dom_accessor));
}

static bool load(Input* input, domUint offset, const domP* dom_p, domUint max_offset, const domAccessor* dom_accessor){
	daeDatabase* dae_db = const_cast<domP*>(dom_p)->getDAE()->getDatabase();
	// <float_array>を取得
	const char*	source = dom_accessor->getSource().fragment().c_str();
	domFloat_array* dom_float_array;
	if(dae_db->getElement((daeElement**)&dom_float_array, 0, source, "float_array") != DAE_OK){
		return false;
	}

	const size_t param_count = getActualCount(dom_accessor);
	const size_t param_offset = getOffset(dom_accessor);
	const size_t param_stride = dom_accessor->getStride();
	const size_t fa_count = dom_float_array->getValue().getCount();
	// <p>から必要な要素を抜き出す
	const size_t p_count = dom_p->getValue().getCount();
	const size_t skip = max_offset + 1;

	for(size_t i = 0; i < p_count; i += skip){
		domUint p = dom_p->getValue().get(i + offset);
		// 現在、名無し<param>は考慮していない
		// また<name>は妥当な順番で入っていると仮定
		for(size_t j = 0; j < param_count; j++){
			size_t offs = p * param_stride + param_offset + j;
			domFloat f = dom_float_array->getValue().get(offs);
			input->f_array.push_back(f);
		}
	}
	input->stride = param_stride;
	return true;
}

Triangles::Triangles(){
	position = NULL;
	normal = NULL;
	texcoords = NULL;
	indices = NULL;
	mtrl_uid = (unsigned int)-1;
}

Triangles::~Triangles(){
	cleanup();
}

void Triangles::cleanup(){
	if(position){
		delete position;
		position = NULL;
	}
	if(normal){
		delete normal;
		normal = NULL;
	}
	if(texcoords){
		InputPtrArray::iterator it = texcoords->begin();
		while(it != texcoords->end()){
			delete (*it);
			(*it) = NULL;
			it++;
		}
		delete texcoords;
		texcoords = NULL;
	}
	if(indices){
		delete indices;
		indices = NULL;
	}
	mtrl_uid = (unsigned int)-1;
}

bool Triangles::load(const domInputLocalOffset* dom_ilo, const domP* dom_p, domUint max_offset){
	daeDatabase* dae_db = const_cast<domInputLocalOffset*>(dom_ilo)->getDAE()->getDatabase();
	// 参照している<source>を取得
	const char* source = dom_ilo->getSource().fragment().c_str();
	domSource* dom_source;
	if(dae_db->getElement((daeElement**)&dom_source, 0, source, "source") != DAE_OK)
		return false;
	// <technique_common>を取得
	domSource::domTechnique_common* dom_tech_common = dom_source->getTechnique_common();
	if(!dom_tech_common)
		return false;
	// <accessor>を取得
	const domAccessor* dom_accessor = dom_tech_common->getAccessor();
	if(!dom_accessor)
		return false;

	Input* input;
	try{
		input = new Input;
	}
	catch(std::bad_alloc& e){
		return false;
	}

	if(!collada::load(input, dom_ilo->getOffset(), dom_p, max_offset, dom_accessor)){
		delete input;
		return false;
	}

	if(strcmp(dom_ilo->getSemantic(), "POSITION") == 0){
		assert(position == NULL);
		position = input;
	}
	else
	if(strcmp(dom_ilo->getSemantic(), "NORMAL") == 0){
		assert(normal == NULL);
		normal = input;
	}
	else
	if(strcmp(dom_ilo->getSemantic(), "TEXCOORD") == 0){
		if(!texcoords){
			try{
				texcoords = new InputPtrArray;
			}
			catch(std::bad_alloc& e){
				delete input;
				return false;
			}
		}
		texcoords->push_back(input);
	}
	else{
		delete input;
	}
	return true;
}

bool Triangles::load(const domInputLocal* dom_il, const domP* dom_p, domUint max_offset, domUint offset, domUint set){
	daeDatabase* dae_db = const_cast<domInputLocal*>(dom_il)->getDAE()->getDatabase();
	// 参照している<source>を取得
	const char* source = dom_il->getSource().fragment().c_str();
	domSource* dom_source;
	if(dae_db->getElement((daeElement**)&dom_source, 0, source, "source") != DAE_OK)
		return false;
	// <technique_common>を取得
	domSource::domTechnique_common* dom_tech_common = dom_source->getTechnique_common();
	if(!dom_tech_common)
		return false;
	// <accessor>を取得
	const domAccessor* dom_accessor = dom_tech_common->getAccessor();
	if(!dom_accessor)
		return false;

	Input* input;
	try{
		input = new Input;
	}
	catch(std::bad_alloc& e){
		return false;
	}

	if(!collada::load(input, offset, dom_p, max_offset, dom_accessor)){
		delete input;
		return false;
	}

	if(strcmp(dom_il->getSemantic(), "POSITION") == 0){
		assert(position == NULL);
		position = input;
	}
	else
	if(strcmp(dom_il->getSemantic(), "NORMAL") == 0){
		assert(normal == NULL);
		normal = input;
	}
	else
	if(strcmp(dom_il->getSemantic(), "TEXCOORD") == 0){
		if(!texcoords){
			try{
				texcoords = new InputPtrArray;
			}
			catch(std::bad_alloc& e){
				delete input;
				return false;
			}
		}
		texcoords->push_back(input);
	}
	else{
		delete input;
	}
	return true;
}

bool Triangles::load(domTriangles* dom_tri){
	// マテリアル名の取得
	if(dom_tri->getMaterial()){
		const char* material = dom_tri->getMaterial();
#ifdef DEBUG
		this->material.clear();
		this->material.append(material);
#endif
		mtrl_uid = calcCRC32(reinterpret_cast<const unsigned char*>(material));
	}
	// <input>で最も大きいオフセットを取得
	const size_t max_offset = getMaxOffset(dom_tri->getInput_array());
	// インデクス配列の取得
	const domP* dom_p = dom_tri->getP();
	// <input>を展開していく
	const size_t input_coutn = dom_tri->getInput_array().getCount();
	for(size_t i = 0; i < input_coutn; i++){
		domInputLocalOffset* dom_ilo = dom_tri->getInput_array().get(i);
		if(strcmp(dom_ilo->getSemantic(), "VERTEX") == 0){
			const char* source = dom_ilo->getSource().fragment().c_str();
			domVertices* dom_verts;
			if(dom_tri->getDAE()->getDatabase()->getElement((daeElement**)&dom_verts, 0, source, "vertices") != DAE_OK){
				cleanup();
				return false;
			}
			// <vertices>を展開
			const size_t input_count = dom_verts->getInput_array().getCount();
			for(size_t j = 0; j < input_count; j++){
				domInputLocal* dom_il = dom_verts->getInput_array().get(i);
				if(!load(dom_il, dom_p, max_offset, dom_ilo->getOffset(), dom_ilo->getSet())){
					cleanup();
					return false;
				}
			}
		}
		else{
			if(!load(dom_ilo, dom_p, max_offset)){
				cleanup();
				return false;
			}
		}
	}
	// 展開した配列を圧縮しインデクス化する
	if(!optimize()){
		cleanup();
		return false;
	}
	return true;
}

/**
 * 対象となるインデクスまでで重複しているか調べる
 * @param target_index 対象となるインデクス
 * @param overlapped_index 重複しているインデクス
 * @return 重複の有無
 */
bool Triangles::isOverlapped(size_t target_index, size_t* overlapped_index){
	const float epsilon = 0.00000001f;
	for(size_t i = 0; i < target_index; i++){
		bool overlap = true;
		// 位置
		if(position){
			const size_t stride = position->stride;
			const float* p = &position->f_array[0];
			for(size_t j = 0; j < stride; j++){
				float f = fabsf(p[target_index * stride + j] - p[i * stride + j]);
				if(f >= epsilon){
					overlap = false;
					break;
				}
			}
		}
		if(!overlap)
			continue;
		// 法線
		if(normal){
			const size_t stride = normal->stride;
			const float* p = &normal->f_array[0];
			for(size_t j = 0; j < stride; j++){
				float f = fabsf(p[target_index * stride + j] - p[i * stride + j]);
				if(f >= epsilon){
					overlap = false;
					break;
				}
			}
		}
		if(!overlap)
			continue;
		// テクスチャ座標 
		if(texcoords){
			InputPtrArray::iterator it = texcoords->begin();
			while(it != texcoords->end()){
				const size_t stride = (*it)->stride;
				const float* p = &(*it)->f_array[0];
				for(size_t j = 0; j < stride; j++){
					float f = fabsf(p[target_index * stride + j] - p[i * stride + j]);
					if(f >= epsilon){
						overlap = false;
						break;
					}
				}
				if(!overlap)
					break;
				it++;
			}
		}
		// チェック
		if(overlap){
			*overlapped_index = i;
			return true;
		}
	}
	return false;
}

bool Triangles::optimize(){
	const size_t num_elements = position->f_array.size() / position->stride;

	// インデクス配列とフラグ領域の確保
	const size_t bits = sizeof(size_t) * 8;
	const size_t flag_size = static_cast<size_t>(ceilf(static_cast<float>(num_elements) / static_cast<float>(bits)));
	size_t* flags;
	try{
		indices = new UintArray;
		flags = new size_t[flag_size];
	}
	catch(std::bad_alloc& e){
		return false; // flagsでthrowされた場合、上位でcleanupしているので問題ない
	}

	// インデクス配列とフラグ領域の初期化
	for(size_t i = 0; i < num_elements; i++){
		indices->push_back(static_cast<unsigned int>(i));
	}
	memset(reinterpret_cast<void*>(flags), 0, sizeof(size_t)*flag_size);

	// 再インデクス化
	unsigned int flag_counter = 0;
	for(size_t i = 0; i < num_elements; i++){
		size_t overlapped_index;
		if(isOverlapped(i, &overlapped_index)){
			(*indices)[i] = (*indices)[overlapped_index];
			size_t mask = ~(1 << (i%bits));
			flags[i/bits] = (flags[i/bits] & mask) | ~mask;
		}
		else{
			(*indices)[i] = flag_counter;
			flag_counter++;
		}
	}

	// 頂点配列の圧縮
	size_t offset = 0;
	for(size_t i = 0; i < num_elements; i++){
		if(flags[i/bits] & (1 << (i % bits))){
			if(position){
				FloatArray::iterator it_beg = position->f_array.begin() + (i - offset) * position->stride;
				FloatArray::iterator it_end = it_beg + position->stride;
				position->f_array.erase(it_beg, it_end);
			}
			if(normal){
				FloatArray::iterator it_beg = normal->f_array.begin() + (i - offset) * normal->stride;
				FloatArray::iterator it_end = it_beg + normal->stride;
				normal->f_array.erase(it_beg, it_end);
			}
			if(texcoords){
				InputPtrArray::iterator it = texcoords->begin();
				while(it != texcoords->end()){
					FloatArray::iterator it_beg = (*it)->f_array.begin() + (i - offset) * (*it)->stride;
					FloatArray::iterator it_end = it_beg + (*it)->stride;
					(*it)->f_array.erase(it_beg, it_end);
					it++;
				}
			}
			offset++;
		}
	}

	if(position)
		FloatArray(position->f_array).swap(position->f_array);
	if(normal)
		FloatArray(normal->f_array).swap(normal->f_array);
	if(texcoords){
		InputPtrArray::iterator it = texcoords->begin();
		while(it != texcoords->end()){
			FloatArray((*it)->f_array).swap((*it)->f_array);
			it++;
		}
	}

	delete[] flags;

	return true;
}

////////////////////////////////////////////////////////////////////////////////

static void triangulation(const domPolylist* from, domTriangles* to){
	// name(optional)
	if(from->getName())
		to->setName(from->getName());
	// count
	size_t t_count = 0;
	// material(optional)
	if(from->getMaterial())
		to->setMaterial(from->getMaterial());
	// <input>
	const size_t i_count = from->getInput_array().getCount();
	for(size_t i = 0; i < i_count; i++){
		to->placeElement(from->getInput_array().get(i)->clone());
	}
	// triangulation
	domP* p = (domP*)to->createAndPlace("p");
	const size_t v_count = from->getVcount()->getValue().getCount();
	size_t offset = 0;
	for(size_t i = 0; i < v_count; i++){
		size_t ntri = from->getVcount()->getValue().get(i) - 2;	// 三角形の数
		size_t index = i_count;
		for(size_t j = 0; j < ntri; j++){
			// for first vertex
			for(size_t k = 0; k < i_count; k++){
				p->getValue().append(from->getP()->getValue().get(offset + k));
			}
			// for second vertex
			for(size_t k = 0; k < i_count; k++){
				p->getValue().append(from->getP()->getValue().get(offset + index + k));
			}
			// for third vertex
			index += i_count;
			for(size_t k = 0; k < i_count; k++){
				p->getValue().append(from->getP()->getValue().get(offset + index + k));
			}
			t_count++;
		}
		offset += from->getVcount()->getValue().get(i) * i_count;
	}
	to->setCount(t_count);
}

Mesh::Mesh(){
	triangles = NULL;
}

Mesh::~Mesh(){
	cleanup();
}

void Mesh::cleanup(){
	if(triangles){
		TrianglesPtrArray::iterator it = triangles->begin();
		while(it != triangles->end()){
			delete (*it);
			(*it) = NULL;
			it++;
		}
		triangles->clear();
		delete triangles;
		triangles = NULL;
	}
}

bool Mesh::load(domMesh* dom_mesh){
	// <polylist>
	size_t count = dom_mesh->getPolylist_array().getCount();
	for(size_t i = 0; i < count; i++){
		domPolylist* dom_polylist = dom_mesh->getPolylist_array().get(i);
		domTriangles* dom_tri = (domTriangles*)dom_mesh->createAndPlace("triangles");
		triangulation(dom_polylist, dom_tri);
	}
	// <polylist>を削除
	while(dom_mesh->getPolylist_array().getCount() > 0){
		domPolylist* dom_polylist = dom_mesh->getPolylist_array().get(0);
		dom_mesh->removeChildElement(reinterpret_cast<daeElement*>(dom_polylist));
	}

	// <triangles>
	count = dom_mesh->getTriangles_array().getCount();
	if(count > 0){
		try{
			triangles = new TrianglesPtrArray;
		}
		catch(std::bad_alloc& e){
			cleanup();
			return false;
		}
	}
	for(size_t i = 0; i < count; i++){
		domTriangles* dom_tri = dom_mesh->getTriangles_array().get(i);
		Triangles* tri;
		try{
			tri = new Triangles;
		}
		catch(std::bad_alloc& e){
			cleanup();
			return false;
		}
		if(!tri->load(dom_tri)){
			delete tri;
			cleanup();
			return false;
		}
		triangles->push_back(tri);
	}
	return true;
}

////////////////////////////////////////////////////////////////////////////////

Geometry::Geometry(){
	mesh = NULL;
}

Geometry::~Geometry(){
	cleanup();
}

void Geometry::cleanup(){
	if(mesh){
		delete mesh;
		mesh = NULL;	
	}
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

bool Geometry::load(domInstance_geometry* dom_inst_geom){
	const char* url = dom_inst_geom->getUrl().fragment().c_str();
#ifdef DEBUG
	this->url.clear();
	this->url.append(url);
#endif
#if 1
	// <geometry>
	domGeometry* dom_geom;
	if(dom_inst_geom->getDAE()->getDatabase()->getElement((daeElement**)&dom_geom, 0, url, "geometry") != DAE_OK){
		cleanup();
		return false;
	}
	if(!load(dom_geom)){
		cleanup();
		return false;
	}
#endif
	// <bind_material>
	domBind_material* dom_bind_mtrl = dom_inst_geom->getBind_material();
	if(dom_bind_mtrl){
		if(!load(dom_bind_mtrl)){
			cleanup();
			return false;
		}
	}
	return true;
}

bool Geometry::load(domGeometry* dom_geom){
#ifdef DEBUG
	id.clear();
	id.append(dom_geom->getID());
#endif
	// geometric_element(メッシュ以外は無視)
	domMesh* dom_mesh = dom_geom->getMesh();
	if(dom_mesh){
		try{
			mesh = new Mesh;
		}
		catch(std::bad_alloc& e){
			cleanup();
			return false;
		}
		if(!mesh->load(dom_mesh)){
			cleanup();
			return false;
		}
	}
	return true;
}

bool Geometry::load(domBind_material* dom_bind_mtrl){
	// <technique_common>		
	domBind_material::domTechnique_common* dom_tech_common = dom_bind_mtrl->getTechnique_common();
	// <instance_material>
	size_t mtrl_count = dom_tech_common->getInstance_material_array().getCount();
	for(size_t i = 0; i < mtrl_count; i++){
		domInstance_material* dom_inst_mtrl = dom_tech_common->getInstance_material_array().get(i);
		Material* mtrl;
		try{
			mtrl = new Material;
		}
		catch(std::bad_alloc& e){
			cleanup();
			return false;
		}
#ifdef DEBUG
		mtrl->symbol.clear();
		mtrl->symbol.append(dom_inst_mtrl->getSymbol());
#endif
		if(!mtrl->load(dom_inst_mtrl)){
			delete mtrl;
			cleanup();
			return false;
		}
		// 登録	
		unsigned int id = calcCRC32(reinterpret_cast<const unsigned char*>(dom_inst_mtrl->getSymbol()));
		std::pair<unsigned int, Material*> p(id, mtrl);
		std::map<unsigned int, Material*>::_Pairib pib = bind_material.insert(p);
		if(!pib.second){	// キーが重複している
			delete mtrl;
			cleanup();
			return false;
		}
	}
	return true;
}

} // namespace collada