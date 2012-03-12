#include <iostream>
#include <GL/glew.h>
#include <GL/glut.h>
#include "glsl.h"
#include "collada.h"

// 光源
static const GLfloat lightpos[] = { 0.0, 0.0,30.0, 1.0 }; // 位置
static const GLfloat lightcol[] = { 1.0, 1.0, 1.0, 1.0 }; // 直接光強度
static const GLfloat lightamb[] = { 0.1, 0.1, 0.1, 1.0 }; // 環境光強度

// マテリアル
static const GLfloat ambient[] = { 0.2, 0.2, 0.2, 1.0 };
static const GLfloat diffuse[] = { 0.7, 0.7, 0.7, 1.0 };
static const GLfloat specular[] = { 0.3, 0.3, 0.3, 1.0 };

// Colladaシーン
collada::Collada* scene = NULL;

// GLSLテスト
#define USE_SHADER
#ifdef USE_SHADER
static Glsl glsl0;
#endif

/**
 * 初期化
 */
static bool init(void){
	glClearColor(0.0, 0.0, 1.0, 1.0);
	glEnable(GL_DEPTH_TEST);
	glFrontFace(GL_CW);
//	glEnable(GL_CULL_FACE);
//	glCullFace(GL_BACK);
	glCullFace(GL_FRONT);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, lightcol);
	glLightfv(GL_LIGHT0, GL_SPECULAR, lightcol);
	glLightfv(GL_LIGHT0, GL_AMBIENT, lightamb);
	glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE);
//	glShadeModel(GL_SMOOTH);
//	glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);

	// Colladaシーンの初期化
	try{
		scene = new collada::Collada;
	}
	catch(std::bad_alloc& e){
		return false;
	}
	if(!scene->load("mone/mone.dae")){
		delete scene;
		scene = NULL;
		return false;
	}
#ifdef USE_SHADER
	if(!glsl0.create("shader/simple.vert", "shader/output_normal.frag"))
		return false;
#endif
	return true;
}

/**
 * GL用コールバック
 */
static void display(void){
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();
	gluLookAt(0.0, 0.0,7.5, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);

	static float r = 0.0;
	glRotatef(r, 0.0f, 1.0f, 0.0f);
	glRotatef(-90, 1.0f, 0.0f, 0.0f);
	r += 1.0f;
	r = fmodf(r, 360.0f);
#ifdef USE_SHADER
	glUseProgram(glsl0.getProgram());
#endif
#if 1
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ambient);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diffuse);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specular);
	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 100.0f);
#endif
	const collada::Node* node = scene->findNode();
	while(node != NULL){
		const collada::GeometryPtrArray& geoms = node->getGeometries();
		for(size_t i = 0; i < geoms.size(); i++){
			const collada::Mesh* mesh = geoms[i]->getMesh();
			if(mesh == NULL)
				continue;
			const collada::TrianglesPtrArray* triangles = mesh->getTriangles();
			for(size_t j = 0; j < triangles->size(); j++){

				const collada::Input* position = (*triangles)[j]->getPosition();
				const collada::Input* normal = (*triangles)[j]->getNormal();
				const collada::UintArray* indices = (*triangles)[j]->getIndices();

				glEnableClientState(GL_VERTEX_ARRAY);
				glEnableClientState(GL_NORMAL_ARRAY);
				glVertexPointer(3, GL_FLOAT, 0, &position->f_array[0]);
				glNormalPointer(GL_FLOAT, 0, &normal->f_array[0]);

				glDrawElements(GL_TRIANGLES, indices->size(), GL_UNSIGNED_INT, &(*indices)[0]);
			
				glDisableClientState(GL_VERTEX_ARRAY);
				glDisableClientState(GL_NORMAL_ARRAY);
			}
		}
		node = node->getNext();
	}
#ifdef USE_SHADER
	glUseProgram(0);
#endif
	glutSwapBuffers();
}

/**
 * GL用コールバック
 */
static void idle(void){
	glutPostRedisplay();
}

/**
 * GL用コールバック
 */
static void resize(int w, int h){
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(30.0, (double)w / (double)h, 1.0, 100.0);
	glMatrixMode(GL_MODELVIEW);
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
	//glutMouseFunc(mouse);
	//glutMotionFunc(motion);
	//glutKeyboardFunc(keyboard);
	if(!init())
		return EXIT_FAILURE;
	glutMainLoop();
	return EXIT_SUCCESS;
}