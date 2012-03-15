#include <iostream>
#include <GL/glew.h>
//#include <GL/glut.h>
#include <GL/freeglut.h>
#include <opencv/cv.h>
#include <opencv/highgui.h>
#include "glsl.h"
#include "collada.h"
#include "crc32.h"
#include "vector.h"
#include "quaternion.h"
#include "matrix.h"

using namespace mathematics;

// 光源
static const GLfloat lightpos[] = {0.0f,100.0f,100.0f, 1.0f}; // 位置
static const GLfloat lightdif[] = {1.0f, 1.0f, 1.0f, 1.0f}; // 直接光強度
static const GLfloat lightspe[] = {1.0f, 1.0f, 1.0f, 1.0f}; // 直接光強度
static const GLfloat lightamb[] = {0.2f, 0.2f, 0.2f, 1.0f}; // 環境光強度

// Colladaモデル
collada::Collada* model = NULL;

// GLSLテスト
#define USE_SHADER
#ifdef USE_SHADER
static Glsl glsl0;
static Glsl glsl1;
#endif

// テクスチャ
#define USE_TEXTURE
static std::map<unsigned int, GLuint> textures;

// カメラ
static float fov = 45.0f;
static float cam_pos_z = 20.0f;

static Quaternion qc;
/**
 * 初期化
 */
static bool init(void){
	glClearColor(0.0, 0.0, 1.0, 1.0);
	glEnable(GL_DEPTH_TEST);
	glFrontFace(GL_CW);
	glEnable(GL_CULL_FACE);
//	glCullFace(GL_BACK);
	glCullFace(GL_FRONT);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, lightdif);
	glLightfv(GL_LIGHT0, GL_SPECULAR, lightspe);
	glLightfv(GL_LIGHT0, GL_AMBIENT, lightamb);
	glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE);
//	glShadeModel(GL_SMOOTH);
//	glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
	glActiveTexture(GL_TEXTURE0);

	// Colladaモデルの初期化
	try{
		model = new collada::Collada;
	}
	catch(std::bad_alloc& e){
		return false;
	}
	if(!model->load("model/miku/mikumiku.dae")){
		delete model;
		model = NULL;
		return false;
	}
#ifdef USE_SHADER
	if(!glsl0.create("shader/simple.vert", "shader/mqo.frag"))
		return false;
	if(!glsl1.create("shader/simple.vert", "shader/mqo_tex.frag"))
		return false;
#endif
	const collada::Images* images = model->getImages();
	if(images){
		const collada::StringArray* _images = images->getImages();
		std::string path;
		for(size_t i = 0; i < _images->size(); i++){
			path.clear();
			path.append(images->getPath()->c_str());
			path.append((*_images)[i].c_str());
			unsigned int uid = calcCRC32(reinterpret_cast<const unsigned char*>(path.c_str()));

			IplImage* image = cvLoadImage(path.c_str());
			cvCvtColor(image, image, CV_BGR2RGB);
			cvFlip(image, NULL, 0);
			GLuint texture;
			glGenTextures(1, &texture);
			glBindTexture(GL_TEXTURE_2D, texture);
			glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR_MIPMAP_LINEAR);
			glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_LINEAR);
			gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGB, image->width, image->height, GL_RGB, GL_UNSIGNED_BYTE, image->imageData);
			cvReleaseImage(&image);

			std::pair<unsigned int, GLuint> p(uid, texture);
			std::map<unsigned int, GLuint>::_Pairib pib = textures.insert(p);
			if(!pib.second){	// キーが重複している
			}
		}
	}
	return true;
}

/**
 * 解放
 */
void release(){
	std::map<unsigned int, GLuint>::iterator it = textures.begin();
	while(it != textures.end()){
		glDeleteTextures(1, &it->second);
		it++;
	}
	textures.clear();
}

/**
 * GLUT用コールバック
 */
static void display(void){
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();
	gluLookAt(0.0, 0.0, cam_pos_z, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
	mathematics::Matrix44 matR(qc);
	glMultMatrixf(matR);

	const collada::Scene* scene = model->getScene();
	const collada::Node* node = scene->findNode();
	while(node != NULL){
		const collada::GeometryPtrArray& geoms = node->getGeometries();
		for(size_t i = 0; i < geoms.size(); i++){
			const collada::Mesh* mesh = geoms[i]->getMesh();
			if(mesh == NULL)
				continue;
			std::map<unsigned int, collada::Material*>& bind_material = geoms[i]->getBindMaterial();
			const collada::TrianglesPtrArray* triangles = mesh->getTriangles();
			for(size_t j = 0; j < triangles->size(); j++){
				// マテリアル
				collada::Material* material = bind_material[(*triangles)[j]->getMaterialUid()];
				glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, material->getEmission()->color);
				glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, material->getAmbient()->color);
				glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, material->getDiffuse()->color);
				glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, material->getSpecular()->color);
				glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, material->getShininess());
				// 位置
				const collada::UintArray* indices = (*triangles)[j]->getIndices();
				const collada::Input* position = (*triangles)[j]->getPosition();
				if(!indices || !position)
					continue;
				glEnableClientState(GL_VERTEX_ARRAY);
				glVertexPointer(position->stride, GL_FLOAT, 0, &position->f_array[0]);
				// 法線
				const collada::Input* normal = (*triangles)[j]->getNormal();
				if(normal){
					glEnableClientState(GL_NORMAL_ARRAY);
					glNormalPointer(GL_FLOAT, 0, &normal->f_array[0]);
				}
#ifdef USE_TEXTURE
 #ifdef USE_SHADER
				glUseProgram(glsl0.getProgram());
 #endif
				// テクスチャ座標
				const collada::InputPtrArray* texcoords = (*triangles)[j]->getTexCoords();
				if(texcoords){
					glActiveTexture(GL_TEXTURE0);
					glClientActiveTexture(GL_TEXTURE0);
					glEnable(GL_TEXTURE_2D);
					if(material->getDiffuse()->sampler){
 #ifdef USE_SHADER
						glUseProgram(glsl1.getProgram());
 #endif
						glBindTexture(GL_TEXTURE_2D, textures[material->getDiffuse()->sampler->image_uid]);
					}
					glClientActiveTexture(GL_TEXTURE0);
					glEnableClientState(GL_TEXTURE_COORD_ARRAY);
					collada::InputPtrArray::const_iterator it = texcoords->begin();
					glTexCoordPointer((*it)[0].stride, GL_FLOAT, 0, &(*it)[0].f_array[0]);
				}
#endif
				// 描画
				glDrawElements(GL_TRIANGLES, indices->size(), GL_UNSIGNED_INT, &(*indices)[0]);
				// 後始末
				glDisableClientState(GL_VERTEX_ARRAY);
				glDisableClientState(GL_NORMAL_ARRAY);
				glClientActiveTexture(GL_TEXTURE0);
				glDisable(GL_TEXTURE_2D);
			}
		}
		node = node->getNext();
	}
//	glPopMatrix();
#ifdef USE_SHADER
	glUseProgram(0);
#endif
	glutSwapBuffers();
}

/**
 * GLUT用コールバック
 */
static void idle(void){
	glutPostRedisplay();
}

/**
 * GLUT用コールバック
 */
static void resize(int w, int h){
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective((double)fov, (double)w / (double)h, 1.0, 100.0);
	glMatrixMode(GL_MODELVIEW);
}

static int begin_pos_x;
static int begin_pos_y;
/**
 * GLUT用コールバック
 */
void mouse(int button, int state, int x, int y){
	if(button == GLUT_LEFT_BUTTON){
		if(state == GLUT_DOWN){
			begin_pos_x = x;
			begin_pos_y = y;
		}
		else
		if(state == GLUT_UP){
		}
	}
}

/**
 * GLUT用コールバック
 */
void motion(int x, int y){
	GLint vp[4];
	glGetIntegerv(GL_VIEWPORT, vp);
	float dx = (float)(x - begin_pos_x) / vp[2];
	float dy = (float)(begin_pos_y - y) / vp[3];
	float sq = sqrtf(dx*dx+dy*dy);
	if(sq > 0.0f){
		Quaternion q;
		QuaternionRotation(&q, &Vector3(dy/sq, dx/sq, 0.0f), sq * 10.0f);
//		QuaternionMul(&qc, &q, &qc);
		QuaternionMul(&qc, &qc, &q);
	}
}

/**
 * FREEGLUT用コールバック
 */
void MouseWheel(int wheel_number, int direction, int x, int y){
	cam_pos_z -= (float)direction * 0.1f;
	if(cam_pos_z < 1.0f)
		cam_pos_z = 1.0f;
}

/**
 * エントリ
 */
int main(int argc, char *argv[]){
	// initialize GLUT
	glutInitWindowPosition(0, 0);
	glutInitWindowSize(640, 480);
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE);
	glutCreateWindow("ColladaLoader"); // argv[0]
	// initialize GLEW
    GLenum glew_error;
	glew_error = glewInit();
	if(glew_error != GLEW_OK){
		std::cerr << "error: " << glewGetErrorString( glew_error ) << std::endl;
		return EXIT_FAILURE;
	}
	std::cout << "OpenGL version: " << glGetString(GL_VERSION) << std::endl;
	std::cout << "GLEW version: " << glewGetString(GLEW_VERSION) << std::endl;
	// register callback
	glutDisplayFunc(display);
	glutIdleFunc(idle);
	glutReshapeFunc(resize);
	glutMouseFunc(mouse);
	glutMotionFunc(motion);
	//glutKeyboardFunc(keyboard);
	glutMouseWheelFunc(MouseWheel);
	if(!init())
		return EXIT_FAILURE;
	glutMainLoop();
	return EXIT_SUCCESS;
}