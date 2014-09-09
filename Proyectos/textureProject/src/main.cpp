#include <GL/glew.h>
#include <stdlib.h>
#include <windows.h>
#include <GL/glut.h>
#include "modelPly.h"
#include "modelXYZ.h"
#include "masterPly.h"
#include "masterSettings.h"
#include <vector>
#include <fstream>
#include <sstream>
#include <iostream>
#include <GL/glext.h>

#include "FreeImage.h"
#include "matrix4x4.h"

using namespace std;

GLuint *textures = new GLuint[2];
PFNGLACTIVETEXTUREARBPROC       glActiveTextureARB       = NULL;
PFNGLMULTITEXCOORD2FARBPROC     glMultiTexCoord2fARB     = NULL;
PFNGLCLIENTACTIVETEXTUREARBPROC glClientActiveTextureARB = NULL;
PFNGLBEGINQUERYARBPROC glBeginQueryARB = NULL;
PFNGLGENQUERIESARBPROC glGenQueriesARB = NULL;
PFNGLENDQUERYARBPROC glEndQueryARB = NULL;
PFNGLGETQUERYOBJECTUIVPROC glGetQueryObjectuivARB = NULL;

GLfloat colors[][3] = { {1.0,1.0,1.0},
                        {1.0,0.0,0.0},
                        {0.0,1.0,0.0},
                        {0.0,0.0,1.0},
                        {1.0,1.0,0.0},
                        {1.0,0.0,1.0},
                        {0.0,1.0,1.0} };

bool calibration3DMode = true;
MasterSettings* settings = NULL;

/* Mesh */

Model_XYZ** meshModel = NULL;
MasterMesh* meshMaster = NULL;
MasterMesh* meshMasterNow = NULL;

int meshCount = 3;
int meshIndex = 0;
bool meshViewMode = true;

/* Texture */

Model_PLY* textureModel = NULL;
MasterTexture* textureMaster = NULL;
MasterTexture* textureMasterNow = NULL;

int textureCount = 3;
int textureIndex = 0;
bool textureViewMode = false;
bool textureWire = true;
int* textureH;
int* textureW;

int facesCount = 2000;
int** faces;

/* Camera */

int cameraAxis = -1;
int cameraMove = -1;
GLdouble oldViewer[3];


void writeText() {
    //system("cls");
    if(calibration3DMode) {
        cout << "3D CALIBRATION" << endl;
        cout << "Mode: " << (meshViewMode ? "View" : "Calibration") << endl << endl;
        for (int i = 0; i <= meshCount; i++) {
            MasterMesh* masterNow = &meshMaster[i];
            cout << "Mesh :: " << i << endl;
            cout << "Points... " << meshModel[i]->TotalPoints << endl;
            cout << "Origin position..." << endl << masterNow->viewer[0]  << " " << masterNow->viewer[1]  << " " << masterNow->viewer[2] << endl;
            cout << "Object rotate..." << endl << masterNow->rotate[0]  << " " << masterNow->rotate[1]  << " " << masterNow->rotate[2] << endl;
            cout << endl;
        }
    } else {
        cout << "2D CALIBRATION" << endl;
        cout << "Triangles: " << textureModel->TotalConnectedTriangles << endl;
        cout << "Points: " << textureModel->TotalPoints << endl;
        cout << "Faces: " << textureModel->TotalFaces << endl;
        cout << "MinCoord: " << textureModel->MinCoord << endl;
        cout << "MaxCoord: " << textureModel->MaxCoord << endl;
        cout << "AlfaCoord: " << textureModel->AlfaCoord << endl;
        cout << "Mode: " << (textureViewMode ? "View" : "Calibration") << endl << endl;
        for (int i = 0; i <= textureCount; i++) {
            MasterTexture* masterNow = &textureMaster[i];
            cout << "Texture :: " << i << endl;
            cout << "Origin position..." << endl << masterNow->viewer[0]  << " " << masterNow->viewer[1]  << " " << masterNow->viewer[2] << endl;
            cout << "Object rotate..." << endl << masterNow->rotate[0]  << " " << masterNow->rotate[1]  << " " << masterNow->rotate[2] << endl;
            cout << endl;
        }
    }
}

void IncludeMesh (Model_XYZ* model, MasterMesh master) {
    glPushMatrix();
    glLoadIdentity();
    glTranslatef(master.viewer[0], master.viewer[1], master.viewer[2]);
    glRotatef(master.rotate[0], -1.0f,0.0f,0.0f);
    glRotatef(master.rotate[1], 0.0f,-1.0f,0.0f);
    glRotatef(master.rotate[2], 0.0f,0.0f,-1.0f);
    GLdouble m[16];
    glGetDoublev(GL_MODELVIEW_MATRIX, m);
    glPopMatrix();

    model->Include(model, m);

    cout << m[0] << " " << m[1] << " " << m[2] << " " << m[3] << endl;
    cout << m[4] << " " << m[5] << " " << m[6] << " " << m[7] << endl;
    cout << m[8] << " " << m[9] << " " << m[10] << " " << m[11] << endl;
    cout << m[12] << " " << m[13] << " " << m[14] << " " << m[15] << endl;
}

void setFaceVertex(int index) {
    GLfloat vert[3] = { textureModel->Faces_Triangles[index * 3], textureModel->Faces_Triangles[index * 3 + 1], textureModel->Faces_Triangles[index * 3 + 2] };
    glVertex3fv(vert);
}

void setPointVertex(int index) {
    GLfloat vert[3] = { meshModel[meshIndex]->Points[index * 3], meshModel[meshIndex]->Points[index * 3 + 1], meshModel[meshIndex]->Points[index * 3 + 2] };
    glVertex3fv(vert);
}

void draw2DElement(int index) {
    glColor3f(1.0f, 1.0f, 1.0f);
    glBegin(GL_POLYGON);
        setFaceVertex(index * 3);
        setFaceVertex(index * 3 + 1);
        setFaceVertex(index * 3 + 2);
    glEnd();
    if (textureWire) {
        glColor3f(0.5f, 0.5f, 0.5f);
        glBegin(GL_LINE_LOOP);
            setFaceVertex(index * 3);
            setFaceVertex(index * 3 + 1);
            setFaceVertex(index * 3 + 2);
        glEnd();
    }
}

void draw3D() {
    glColor3fv(colors[meshIndex]);
    for (int i = 0; i < meshModel[meshIndex]->TotalPoints; i++) {
        glBegin(GL_POINTS);
            setPointVertex(i);
        glEnd();
    }
}

void draw2DView() {
    for (int i = 0; i < textureModel->TotalFaces; i++) {
        int hits = 0;
        for (int k = 1; k <= textureCount; k++) {
            hits = max(hits, faces[k][i]);
        }
        if (hits > 0 && faces[textureIndex][i] == hits) {
            draw2DElement(i);
        }
    }
}

void draw2DCalibration() {
    GLuint queries[textureModel->TotalFaces];
    GLuint sampleCount;
    glGenQueriesARB(textureModel->TotalFaces, queries);
    glDisable(GL_BLEND);
    glDepthFunc(GL_LESS);
    glDepthMask(GL_TRUE);
    for (int i = 0; i < textureModel->TotalFaces; i++) {
        glBeginQueryARB(GL_SAMPLES_PASSED_ARB, queries[i]);
        draw2DElement(i);
        glEndQueryARB(GL_SAMPLES_PASSED_ARB);
    }
    glEnable(GL_BLEND);
    glDepthFunc(GL_EQUAL);
    glDepthMask(GL_FALSE);
    for (int i = 0; i < textureModel->TotalFaces; i++) {
        glGetQueryObjectuivARB(queries[i], GL_QUERY_RESULT_ARB, &sampleCount);
        if (sampleCount > 0) {
            if (textureIndex > 0) {
                faces[textureIndex][i] = sampleCount;
            }
            draw2DElement(i);
        }
    }
    glDisable(GL_BLEND);
    glDepthFunc(GL_LESS);
    glDepthMask(GL_TRUE);
}

void textureProjection(Matrix4x4f &mv) {

	Matrix4x4f inverseMV = Matrix4x4f::invertMatrix(mv);
	glMatrixMode(GL_TEXTURE);
	glLoadIdentity();
	glTranslatef(0.5f,0.5f,0.0f);//Bias

    float wImg = textureW[textureIndex-1];
    float hImg = textureH[textureIndex-1];
	if (wImg < hImg) {
        glScalef(1.0f,(1.f*wImg)/hImg,1.0f);//Scale
	} else {
        glScalef((1.f*hImg)/wImg,1.0f,1.0f);//Scale
	}

	glFrustum(-0.035,0.035,-0.035,0.035,0.1,1.9);//MV for light map
	glTranslatef(0.0f,0.0f,-1.0f);
	glMultMatrixf(inverseMV.getMatrix());//Inverse ModelView
	glMatrixMode(GL_MODELVIEW);
}


void stepTransformTexture() {
	if (textureViewMode) {
        glTranslatef(textureMaster[0].viewer[0],
                     textureMaster[0].viewer[1],
                     textureMaster[0].viewer[2] - 20);

        glRotatef(textureMasterNow->rotate[0] - textureMaster[0].rotate[0], 1.0f,0.0f,0.0f);
        glRotatef(textureMasterNow->rotate[1] - textureMaster[0].rotate[1], 0.0f,1.0f,0.0f);
        glRotatef(textureMasterNow->rotate[2] - textureMaster[0].rotate[2], 0.0f,0.0f,1.0f);

        glTranslatef(-textureMaster[0].viewer[0],
                     -textureMaster[0].viewer[1],
                     -textureMaster[0].viewer[2] + 20);
        glTranslatef(textureMaster[0].viewer[0] - textureMasterNow->viewer[0],
                     textureMaster[0].viewer[1] - textureMasterNow->viewer[1],
                     textureMaster[0].viewer[2] - textureMasterNow->viewer[2]);
	} else {
        glRotatef(0, 1.0f,0.0f,0.0f);
        glRotatef(0, 0.0f,1.0f,0.0f);
        glRotatef(0, 0.0f,0.0f,1.0f);
	}
}

void stepTexture() {
	glPushMatrix();
	glLoadIdentity();

    if (textureIndex > 0) {
        glActiveTextureARB(GL_TEXTURE0 + textureIndex - 1);
        glEnable(GL_TEXTURE_2D);
        stepTransformTexture();
    }

	glGetFloatv(GL_MODELVIEW_MATRIX, textureMasterNow->MVmatrix);
	textureMasterNow->TextureTransform.setMatrix(textureMasterNow->MVmatrix);
	glPopMatrix();
	textureProjection(textureMasterNow->TextureTransform);
}

void stepClearTexture() {
    for (int i = 0; i < textureCount; i++) {
        glActiveTextureARB(GL_TEXTURE0 + i);
        glDisable(GL_TEXTURE_2D);
    }
}

void display(void) {
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

    if (calibration3DMode) {

        glLoadIdentity();
        glTranslatef(meshMasterNow->viewer[0], meshMasterNow->viewer[1], meshMasterNow->viewer[2] - 20);
        glRotatef(meshMasterNow->rotate[0], -1.0f,0.0f,0.0f);
        glRotatef(meshMasterNow->rotate[1], 0.0f,-1.0f,0.0f);
        glRotatef(meshMasterNow->rotate[2], 0.0f,0.0f,-1.0f);
        draw3D();

    } else {

        if (textureViewMode) {
            for (int i = 1; i <= textureCount; i++) {
                textureIndex = i;
                textureMasterNow = &textureMaster[textureIndex];
                stepTexture();
                glLoadIdentity();
                glTranslatef(textureMaster[0].viewer[0], textureMaster[0].viewer[1], textureMaster[0].viewer[2] - 20);
                glRotatef(textureMaster[0].rotate[0], -1.0f,0.0f,0.0f);
                glRotatef(textureMaster[0].rotate[1], 0.0f,-1.0f,0.0f);
                glRotatef(textureMaster[0].rotate[2], 0.0f,0.0f,-1.0f);
                draw2DView();
                stepClearTexture();
            }
            textureIndex = 0;
            textureMasterNow = &textureMaster[0];

        } else {
            stepTexture();
            glLoadIdentity();
            glTranslatef(textureMasterNow->viewer[0], textureMasterNow->viewer[1], textureMasterNow->viewer[2] - 20);
            glRotatef(textureMasterNow->rotate[0], -1.0f,0.0f,0.0f);
            glRotatef(textureMasterNow->rotate[1], 0.0f,-1.0f,0.0f);
            glRotatef(textureMasterNow->rotate[2], 0.0f,0.0f,-1.0f);
            draw2DCalibration();
            stepClearTexture();
        }
    }

	glFlush();
	glutSwapBuffers();

    writeText();
}

void keys(unsigned char key, int x, int y) {
    if (key == 'z') {
        calibration3DMode = true;
    }
    if (key == 'x') {
        calibration3DMode = false;
    }

    if (calibration3DMode) {

        if (key == 'l' && !meshViewMode) {
            settings->loadMeshCalibration();
        }
        if (key == 'k' && meshViewMode) {
            settings->saveMeshCalibration();
        }
        if(key == 'c' && !meshViewMode) {
            meshMasterNow->rotate[0] = 0;
            meshMasterNow->rotate[1] = 0;
            meshMasterNow->rotate[2] = 0;
        }
        if(key == 'v') {
            meshModel[0]->Clear();
            for (int i = 1; i <= meshCount; i++) {
                IncludeMesh(meshModel[0], meshMaster[i]);
                //meshModel[0]->Include(meshModel[i], meshMaster[i].viewer[0], meshMaster[i].viewer[1], meshMaster[i].viewer[2]);
            }
            meshViewMode = true;
            meshIndex = 0;
            meshMasterNow = &meshMaster[meshIndex];
        }
        if(key >= '1' && key <= '9' && (key - 48 <= meshCount)) {
            meshIndex = key - 48;
            meshMasterNow = &meshMaster[meshIndex];
            display();
        }
        if(key == 'w') meshMasterNow->rotate[0] += 2.0;
        if(key == 's') meshMasterNow->rotate[0] -= 2.0;
        if(key == 'a') meshMasterNow->rotate[1] += 2.0;
        if(key == 'd') meshMasterNow->rotate[1] -= 2.0;
        if(key == 'e') meshMasterNow->rotate[2] += 2.0;
        if(key == 'q') meshMasterNow->rotate[2] -= 2.0;

    } else {

        if (key == 'l' && !textureViewMode) {
            settings->loadTextureCalibration();
            for (int i = 1; i <= textureCount; i++) {
                textureIndex = i;
                textureMasterNow = &textureMaster[textureIndex];
                display();
            }
            textureViewMode = true;
            textureIndex = 0;
            textureMasterNow = &textureMaster[0];
        }
        if (key == 'k' && textureViewMode) {
            settings->saveTextureCalibration();
        }
        if (key == 'm') {
            textureWire = true;
        }
        if (key == 'n') {
            textureWire = false;
        }
        if(key == 'c' && !textureViewMode) {
            textureMasterNow->rotate[0] = 0;
            textureMasterNow->rotate[1] = 0;
            textureMasterNow->rotate[2] = 0;
        }
        if(key == 'v') {
            textureViewMode = true;
            textureIndex = 0;
            textureMasterNow = &textureMaster[textureIndex];
        }
        if(key >= '1' && key <= '9' && (key - 48 <= textureCount)) {
            textureViewMode = false;
            textureIndex = key - 48;
            textureMasterNow = &textureMaster[textureIndex];
            display();
        }
        if(key == 'w') textureMasterNow->rotate[0] += 2.0;
        if(key == 's') textureMasterNow->rotate[0] -= 2.0;
        if(key == 'a') textureMasterNow->rotate[1] += 2.0;
        if(key == 'd') textureMasterNow->rotate[1] -= 2.0;
        if(key == 'e') textureMasterNow->rotate[2] += 2.0;
        if(key == 'q') textureMasterNow->rotate[2] -= 2.0;
    }

	display();
}

void mouse(int btn, int state, int x, int y) {
    cameraAxis = state == GLUT_DOWN ? btn : -1;
    if (state == GLUT_DOWN) {
        cameraMove = y;
        if (calibration3DMode) {
            oldViewer[0] = meshMasterNow->viewer[0];
            oldViewer[1] = meshMasterNow->viewer[1];
            oldViewer[2] = meshMasterNow->viewer[2];
        } else {
            oldViewer[0] = textureMasterNow->viewer[0];
            oldViewer[1] = textureMasterNow->viewer[1];
            oldViewer[2] = textureMasterNow->viewer[2];
        }
    }
    if (state == GLUT_UP) {
        cameraMove = -1;
    }

	display();
}

void mouseMove(int x, int y) {
	if (cameraAxis != -1) {
		float deltaMove = (y - cameraMove) * 0.1f;
		if (calibration3DMode) {
            if (cameraAxis == GLUT_LEFT_BUTTON) {
                meshMasterNow->viewer[0] = oldViewer[0] + deltaMove;
            } else if (cameraAxis == GLUT_RIGHT_BUTTON) {
                meshMasterNow->viewer[1] = oldViewer[1] + deltaMove;
            } else if (cameraAxis == GLUT_MIDDLE_BUTTON) {
                meshMasterNow->viewer[2] = oldViewer[2] + deltaMove;
            }
		} else {
            if (cameraAxis == GLUT_LEFT_BUTTON) {
                textureMasterNow->viewer[0] = oldViewer[0] + deltaMove;
            } else if (cameraAxis == GLUT_RIGHT_BUTTON) {
                textureMasterNow->viewer[1] = oldViewer[1] + deltaMove;
            } else if (cameraAxis == GLUT_MIDDLE_BUTTON) {
                textureMasterNow->viewer[2] = oldViewer[2] + deltaMove;
            }
		}
		display();
	}
}


void myReshape(int w, int h) {
	glViewport(0,0,w,h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	if(w<=h)
        glFrustum(-2.0,2.0,-2.0*(GLfloat)h/(GLfloat)w,2.0*(GLfloat)h/(GLfloat)w, 2.0,20.0);
	else
        glFrustum(-2.0,2.0,-2.0*(GLfloat)w/(GLfloat)h,2.0*(GLfloat)w/(GLfloat)h, 2.0,20.0);
	glMatrixMode(GL_MODELVIEW);
}

void loadLightMapTexture(const char *name) {
	GLfloat eyePlaneS[] =  {1.0f, 0.0f, 0.0f, 0.0f};
	GLfloat eyePlaneT[] =  {0.0f, 1.0f, 0.0f, 0.0f};
	GLfloat eyePlaneR[] =  {0.0f, 0.0f, 1.0f, 0.0f};
	GLfloat eyePlaneQ[] =  {0.0f, 0.0f, 0.0f, 1.0f};
    GLfloat borderColor[] = {1.f, 1.f, 1.f, 1.0f};

	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP);
	glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE);

	glTexGeni(GL_S,GL_TEXTURE_GEN_MODE,GL_EYE_LINEAR);
	glTexGenfv(GL_S,GL_EYE_PLANE,eyePlaneS);
	glTexGeni(GL_T,GL_TEXTURE_GEN_MODE,GL_EYE_LINEAR);
	glTexGenfv(GL_T,GL_EYE_PLANE,eyePlaneT);
	glTexGeni(GL_R,GL_TEXTURE_GEN_MODE,GL_EYE_LINEAR);
	glTexGenfv(GL_R,GL_EYE_PLANE,eyePlaneR);
	glTexGeni(GL_Q,GL_TEXTURE_GEN_MODE,GL_EYE_LINEAR);
	glTexGenfv(GL_Q,GL_EYE_PLANE,eyePlaneQ);

	glEnable(GL_TEXTURE_GEN_S);
    glEnable(GL_TEXTURE_GEN_T);
    glEnable(GL_TEXTURE_GEN_R);
    glEnable(GL_TEXTURE_GEN_Q);

    FREE_IMAGE_FORMAT fif = FIF_UNKNOWN;
    FIBITMAP *dib(0);
    BYTE* bits(0);
    unsigned int width(0), height(0);
    GLuint gl_texID;

    fif = FreeImage_GetFileType(name, 0);
    if(fif == FIF_UNKNOWN)
        fif = FreeImage_GetFIFFromFilename(name);
    if(fif == FIF_UNKNOWN)
        printf("Formato de imagen no reconocido.");

    if(FreeImage_FIFSupportsReading(fif))
        dib = FreeImage_Load(fif, name);
    if(!dib)
        printf("Error al cargar la imagen.");

    bits = FreeImage_GetBits(dib);
    width = FreeImage_GetWidth(dib);
    height = FreeImage_GetHeight(dib);
    if((bits == 0) || (width == 0) || (height == 0))
        printf("Error en la imagen.");

	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, 0x812D);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, 0x812D);
	glTexParameterfv(GL_TEXTURE_2D,GL_TEXTURE_BORDER_COLOR,borderColor);
    gluBuild2DMipmaps(GL_TEXTURE_2D,GL_RGB,width,height,GL_RGB,GL_UNSIGNED_BYTE,bits);

    textureW[textureIndex-1] = width;
    textureH[textureIndex-1] = height;

	FreeImage_Unload(dib);
}

int main(int argc, char **argv) {

    /* Texture */
    textureMaster = new MasterTexture[textureCount + 1];
    faces = new int*[textureCount + 1];
    for (int i = 0; i <= textureCount; i++) {
        for (int j = 0; j < 3; j++) {
            textureMaster[i].viewer[j] = 0.0;
            textureMaster[i].rotate[j] = 0.0;
        }
        faces[i] = new int[facesCount];
    }
    textureW = new int[textureCount];
    textureH = new int[textureCount];

    /* Mesh */
    meshMaster = new MasterMesh[meshCount + 1];
    meshModel = new Model_XYZ*[meshCount];
    for (int i = 0; i <= meshCount; i++) {
        for (int j = 0; j < 3; j++) {
            meshMaster[i].viewer[j] = 0.0;
            meshMaster[i].rotate[j] = 0.0;
        }
        meshModel[i] = new Model_XYZ();
    }

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE|GLUT_RGB|GLUT_DEPTH);
	glutInitWindowSize(500,500);
	glutCreateWindow("Calibration project");
	glutReshapeFunc(myReshape);
	glutDisplayFunc(display);
	glutMouseFunc(mouse);
	glutMotionFunc(mouseMove);
	glutKeyboardFunc(keys);
	glEnable(GL_DEPTH_TEST);

    glGenTextures(3, textures);

    glActiveTextureARB       = (PFNGLCLIENTACTIVETEXTUREARBPROC)wglGetProcAddress("glActiveTextureARB");
    glMultiTexCoord2fARB     = (PFNGLMULTITEXCOORD2FARBPROC)wglGetProcAddress("glMultiTexCoord2fARB");
    glClientActiveTextureARB = (PFNGLACTIVETEXTUREARBPROC)wglGetProcAddress("glClientActiveTextureARB");
    glBeginQueryARB = (PFNGLBEGINQUERYARBPROC)wglGetProcAddress("glBeginQueryARB");
    glGenQueriesARB = (PFNGLGENQUERIESARBPROC)wglGetProcAddress("glGenQueriesARB");
    glEndQueryARB = (PFNGLENDQUERYARBPROC)wglGetProcAddress("glEndQueryARB");
    glGetQueryObjectuivARB = (PFNGLGETQUERYOBJECTUIVPROC)wglGetProcAddress("glGetQueryObjectuivARB");

    if( !glActiveTextureARB || !glMultiTexCoord2fARB || !glClientActiveTextureARB )
    {
        MessageBox(NULL,"One or more GL_ARB_multitexture functions were not found",
            "ERROR",MB_OK|MB_ICONEXCLAMATION);
        return -1;
    }

    /* Settings and files */
    settings = new MasterSettings(textureCount, textureMaster, meshCount, meshMaster);

    textureModel = new Model_PLY();
    textureModel->Load("mesh/antMesh.ply");

    vector<string> textureFiles;
    textureFiles.push_back("img/texture1.bmp");
    textureFiles.push_back("img/texture2.jpg");
    textureFiles.push_back("img/texture3.jpg");
    for (int i = 0; i < textureCount; i++) {
        glActiveTextureARB(GL_TEXTURE0 + i);
        glBindTexture(GL_TEXTURE_2D, textures[i]);
        glDisable(GL_TEXTURE_2D);
        textureIndex = i + 1;
        loadLightMapTexture(textureFiles[i].c_str());
    }
    textureIndex = 0;
    textureMasterNow = &textureMaster[0];

    vector<string> meshFiles;
    meshFiles.push_back("mesh/3DMesh1.xyz");
    meshFiles.push_back("mesh/3DMesh2.xyz");
    meshFiles.push_back("mesh/3DMesh3.xyz");
    for (int i = 0; i < meshCount; i++) {
        if (i == 0) {
            meshModel[i+1]->Load(meshFiles[i].c_str(), 0, 0);
        } else {
            meshModel[i+1]->Load(meshFiles[i].c_str(), meshModel[1]->MinCoord, meshModel[1]->MaxCoord);
        }
    }
    meshModel[0]->Clear();
    for (int i = 1; i <= meshCount; i++) {
        IncludeMesh(meshModel[0], meshMaster[i]);
        //meshModel[0]->Include(meshModel[i], meshMaster[i].viewer[0], meshMaster[i].viewer[1], meshMaster[i].viewer[2]);
    }
    meshIndex = 0;
    meshMasterNow = &meshMaster[0];

    writeText();
	glClearColor( 0.0f, 0.0f, 0.0f, 1.0f );
	glutMainLoop();
}
