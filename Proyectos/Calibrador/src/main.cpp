#include "MeshMain.h"
#include "TextureMain.h"
#include <GL/glut.h>
#include "masterPly.h"
#include "masterSettings.h"
#include <vector>
#include <fstream>
#include <sstream>
#include <iostream>

#include "matrix4x4.h"

using namespace std;

bool REAL_TIME = true;
int REAL_TIME_FPS = 10;
int REAL_TIME_PORT = 3232;

MeshMain* mMain;
TextureMain* tMain;

bool modeMesh = true;


void saveXmlFile() {
	ofxXmlSettings settings;

	settings.addTag("settings");
	settings.pushTag("settings");

	settings.setValue("realTime", REAL_TIME);
	settings.setValue("realTimeFPS", REAL_TIME_FPS);
	settings.setValue("realTimePort", REAL_TIME_PORT);

	settings.addTag("cameras");
	settings.pushTag("cameras");

	for(int i = 0; i < mMain->meshCount; i++) {
		settings.addTag("camera");
		settings.pushTag("camera", i);

		settings.addValue("id", i);
		settings.addValue("resolutionX", 800);//800
		settings.addValue("resolutionY", 600);//600
		settings.addValue("resolutionDownSample", 1);
		settings.addValue("FPS", 30);// camera i fps
		settings.addValue("colorRGB", true);// camera i color rgb
		settings.addValue("use2D", false);// camera i use2d
		settings.addValue("use3D", true);// camera i use 3d
		settings.addValue("dataContext", "");// data context

		settings.addTag("depthSettings");
		settings.pushTag("depthSettings");
		settings.addValue("near", 10);
		settings.addValue("far", 100);
		settings.addValue("pointsDownSample", 1);

		settings.popTag();

		settings.addTag("matrix");
		settings.pushTag("matrix");


        GLdouble m[16];
        MasterSettings::CalculateMatrix(mMain->cloudMaster[i], m);

		for(int j = 0; j < 16; j++) {
            std::stringstream cellM;
            cellM << "r" << j / 4 << j % 4;
            settings.addValue(cellM.str(), m[j]);
		}

		settings.popTag();
		settings.popTag();
	}

	for(int i = 0; i < tMain->textureCount; i++) {
		settings.addTag("camera");
		settings.pushTag("camera",  tMain->textureCount+i);

		settings.addValue("id", i);
		settings.addValue("resolutionX", 800);//800
		settings.addValue("resolutionY", 600);//600
		settings.addValue("resolutionDownSample", 1);
		settings.addValue("FPS", 30);// camera i fps
		settings.addValue("colorRGB", true);// camera i color rgb
		settings.addValue("use2D", true);// camera i use2d
		settings.addValue("use3D", false);// camera i use 3d
		settings.addValue("dataContext", "");// data context

		settings.addTag("depthSettings");
		settings.pushTag("depthSettings");
		settings.addValue("near", 10);
		settings.addValue("far", 100);
		settings.addValue("pointsDownSample", 1);

		settings.popTag();

		settings.addTag("matrix");
		settings.pushTag("matrix");



        MasterTexture* masterNow = &(tMain->textureMaster[i+1]);

        char* v = new char[50];
        sprintf ( v, "%f %f %f",masterNow->viewer[0], masterNow->viewer[1], masterNow->viewer[2] );
        char* r = new char[50];
        sprintf ( r, "%f %f %f",masterNow->rotate[0], masterNow->rotate[1], masterNow->rotate[2] );
        settings.addValue("viewer", v);
        settings.addValue("rotate", r);

		settings.popTag();
		settings.popTag();
	}

	settings.popTag();
	settings.popTag();

	settings.saveFile("settings.xml");
}

void IncludeMesh (Model_XYZ* model, Model_XYZ* newModel, MasterMesh master) {
    GLdouble m[16];
    MasterSettings::CalculateMatrix(master, m);
    model->Include(newModel, m);
}

void display(void) {
    if (modeMesh)
        mMain->display();
    else
        tMain->display();
}

void keys(unsigned char key, int x, int y) {
    if (key == 't'){
        if (mMain->faces != NULL){
            modeMesh = false;
            tMain->textureModel->MemoryLoadCalibrator(mMain->faces,*(mMain->numberFaces));
        }
    }
    if (modeMesh)
        mMain->keys(key, x, y);
    else
        tMain->keys(key, x, y);

    if (key == 'v' && !modeMesh){
        saveXmlFile();
    }
}

void mouse(int btn, int state, int x, int y) {
    if (modeMesh)
        mMain->mouse(btn, state, x, y);
    else
        tMain->mouse(btn, state, x, y);
}

void mouseMove(int x, int y) {
    if (modeMesh)
        mMain->mouseMove(x,y);
    else
        tMain->mouseMove(x,y);
}

void myReshape(int w, int h) {
	glViewport(0,0,w,h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
    if (w <= h) {
        glFrustum(-2.0, 2.0, -2.0*(GLfloat)h/(GLfloat)w,2.0*(GLfloat)h/(GLfloat)w, 2.0, 20.0);
    } else {
        glFrustum(-2.0*(GLfloat)w/(GLfloat)h, 2.0*(GLfloat)w/(GLfloat)h, -2.0, 2.0, 2.0, 20.0);
    }
	glMatrixMode(GL_MODELVIEW);
}

vector<string> getCloudFiles() {
    vector<string> files;
    ofDirectory directory("C:\\CamarasHeterogeneas\\Proyecto\\camarasheterogeneas\\Proyectos\\Calibrador\\bin\\mesh");
    directory.allowExt("xyz");
    directory.listDir();
    for(int i = 0; i < directory.numFiles(); i++) {

        files.push_back(directory.getPath(i));
    }
    return files;
}

vector<string> getImageFiles() {
    vector<string> files;
    ofDirectory directory("C:\\CamarasHeterogeneas\\Proyecto\\camarasheterogeneas\\Proyectos\\Calibrador\\bin\\img");
    directory.allowExt("png");
    directory.allowExt("bmp");
    directory.allowExt("jpg");
    directory.listDir();
    for(int i = 0; i < directory.numFiles(); i++) {
        files.push_back(directory.getPath(i));
    }
    return files;
}

int main(int argc, char **argv) {



	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE|GLUT_RGB|GLUT_DEPTH);
	glutInitWindowSize(500,500);
    glutInitWindowPosition(300, 300);
	glutCreateWindow("Calibration project");
	glutReshapeFunc(myReshape);
	glutDisplayFunc(display);
	glutMouseFunc(mouse);
	glutMotionFunc(mouseMove);
	glutKeyboardFunc(keys);
	glClearColor( 0.0f, 0.0f, 0.0f, 1.0f );

	glEnable(GL_DEPTH_TEST);


    ///INICIALIZACION CALIBRACION 3D
    mMain = new MeshMain();


    /* Cloud */
    vector<string> files3D = getCloudFiles();
    mMain->meshCount = files3D.size();
    mMain->cloudMaster = new MasterMesh[mMain->meshCount + 1];
    mMain->cloudModel = new Model_XYZ*[mMain->meshCount];
    for (int i = 0; i <= mMain->meshCount; i++) {
        for (int j = 0; j < 3; j++) {
            mMain->cloudMaster[i].viewer[j] = 0.0;
            mMain->cloudMaster[i].rotate[j] = 0.0;
        }
        mMain->cloudModel[i] = new Model_XYZ();
    }

    /* Settings and files */
    mMain->settings = new MasterSettings(0, NULL, mMain->meshCount, mMain->cloudMaster);

    /* LoadClouds */
    for (int i = 0; i < mMain->meshCount; i++) {
        if (i == 0) {
            mMain->cloudModel[i+1]->Load(files3D[i].c_str(), 0);
        } else {
            mMain->cloudModel[i+1]->Load(files3D[i].c_str(), mMain->cloudModel[1]->AlfaCoord);
        }
    }

    mMain->cloudModel[0]->Clear();
    for (int i = 1; i <= mMain->meshCount; i++) {
        IncludeMesh(mMain->cloudModel[0], mMain->cloudModel[i], mMain->cloudMaster[i]);
    }
    mMain->meshIndex = 0;
    ///FIN

    ///INICIALIZACION CALIBRACION 2D

    tMain = new TextureMain();
    /* Mesh */
    tMain->textureModel = new Model_PLY();

    /* Texture */
    vector<string> files2D = getImageFiles();
    tMain->textureCount = files2D.size();
    tMain->textureMaster = new MasterTexture[tMain->textureCount + 1];
    tMain->faces = new int*[tMain->textureCount + 1];
    for (int i = 0; i <= tMain->textureCount; i++) {
        for (int j = 0; j < 3; j++) {
            tMain->textureMaster[i].viewer[j] = 0.0;
            tMain->textureMaster[i].rotate[j] = 0.0;
        }
        tMain->faces[i] = new int[tMain->facesCount];
    }

    /* Settings and files */
    tMain->settings = new MasterSettings(tMain->textureCount, tMain->textureMaster, 0, NULL);

    /* Texture config */
    glGenTextures(tMain->textureCount, tMain->textures);
    glActiveTextureARB       = (PFNGLCLIENTACTIVETEXTUREARBPROC)wglGetProcAddress("glActiveTextureARB");
    glMultiTexCoord2fARB     = (PFNGLMULTITEXCOORD2FARBPROC)wglGetProcAddress("glMultiTexCoord2fARB");
    glClientActiveTextureARB = (PFNGLACTIVETEXTUREARBPROC)wglGetProcAddress("glClientActiveTextureARB");
    glBeginQueryARB = (PFNGLBEGINQUERYARBPROC)wglGetProcAddress("glBeginQueryARB");
    glGenQueriesARB = (PFNGLGENQUERIESARBPROC)wglGetProcAddress("glGenQueriesARB");
    glEndQueryARB = (PFNGLENDQUERYARBPROC)wglGetProcAddress("glEndQueryARB");
    glGetQueryObjectuivARB = (PFNGLGETQUERYOBJECTUIVPROC)wglGetProcAddress("glGetQueryObjectuivARB");
    if( !glActiveTextureARB || !glMultiTexCoord2fARB || !glClientActiveTextureARB ) {
        MessageBox(NULL,"One or more GL_ARB_multitexture functions were not found", "ERROR",MB_OK|MB_ICONEXCLAMATION);
        return -1;
    }

    /* LoadImages */
    tMain->textureImage = new Model_IMG[tMain->textureCount];
    for (int i = 0; i < tMain->textureCount; i++) {
        glActiveTextureARB(GL_TEXTURE0 + i);
        glBindTexture(GL_TEXTURE_2D, tMain->textures[i]);
        glDisable(GL_TEXTURE_2D);
        tMain->textureIndex = i + 1;
        tMain->loadLightMapTexture(&(tMain->textureImage[i]), files2D[i]);
    }
    tMain->textureIndex = 0;
    ///FIN

    //writeText();
	glutMainLoop();
}
