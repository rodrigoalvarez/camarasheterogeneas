#include <GL/glew.h>
#include <stdlib.h>
#include <windows.h>
#include <GL/glut.h>
#include "modelPly.h"
#include "masterPly.h"
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


struct MasterTexture {
    GLdouble viewer[3];
    GLdouble rotate[3];

    Matrix4x4f TextureTransform;
    float MVmatrix[16];
};


Model_PLY* model = NULL;
GLuint texObject, projTexture;
GLfloat vertices[][3]={{-1.0,-1.0,-1.0},{1.0,-1.0,-1.0},
          {1.0,1.0,-1.0},{-1.0,1.0,-1.0},{-1.0,-1.0,1.0},
           {1.0,-1.0,1.0},{1.0,1.0,1.0},{-1.0,1.0,1.0}};
GLfloat colors[][3]={{0.0,0.0,0.0},{1.0,0.0,0.0},
              {1.0,1.0,0.0},{0.0,1.0,0.0},{0.0,0.0,1.0},
              {1.0,0.0,1.0},{1.0,1.0,1.0},{0.0,1.0,1.0}};
int textureIndex = 0;

MasterTexture master[4];
MasterTexture* masterNow = NULL;

int cameraAxis = -1;
int cameraMove = -1;
GLdouble oldViewer[3];

bool modeTexture = true;

int TotalConnectedTriangles = 0;
int TotalPoints = 0;
int TotalFaces = 0;
int MinCoord = 0;
int MaxCoord = 0;
int AlfaCoord = 0;

int hImg1, hImg2, hImg3;
int wImg1, wImg2, wImg3;

//int countHits = 0;

void writeText() {
    //system("cls");
    cout << "Triangles: " << TotalConnectedTriangles << endl;
    cout << "Points: " << TotalPoints << endl;
    cout << "Faces: " << TotalFaces << endl;
    cout << "MinCoord: " << MinCoord << endl;
    cout << "MaxCoord: " << MaxCoord << endl;
    cout << "AlfaCoord: " << AlfaCoord << endl;
    cout << "Mode: " << (modeTexture ? "Texture" : "View") << endl << endl;

    for (int i = 0; i < 4; i++) {
        MasterTexture* masterNow = &master[i];
        cout << "Texture :: " << i << endl;
        cout << "Origin position..." << endl << masterNow->viewer[0]  << " " << masterNow->viewer[1]  << " " << masterNow->viewer[2] << endl;
        cout << "Object rotate..." << endl << masterNow->rotate[0]  << " " << masterNow->rotate[1]  << " " << masterNow->rotate[2] << endl;
        cout << endl;
    }
}

void setVertex(int index) {
    GLfloat vert[3] = { model->Faces_Triangles[index * 3], model->Faces_Triangles[index * 3 + 1], model->Faces_Triangles[index * 3 + 2] };
    glVertex3fv(vert);
}

int faces1[912];
int faces2[912];
int faces3[912];

void triangle(int index) {

    glColor3f(1.0f, 1.0f, 1.0f);
	glBegin(GL_POLYGON);
		setVertex(index * 3);
		setVertex(index * 3 + 1);
		setVertex(index * 3 + 2);
	glEnd();
    glBegin(GL_LINE_LOOP);
        glColor3f(0.5f, 0.5f, 0.5f);
        setVertex(index * 3);
        glColor3f(0.5f, 0.5f, 0.5f);
        setVertex(index * 3 + 1);
        glColor3f(0.5f, 0.5f, 0.5f);
        setVertex(index * 3 + 2);
    glEnd();
}

void oldDrawMesh() {
    for (int i = 0; i < model->TotalFaces; i++) {
        if (textureIndex == 1 && faces1[i] > 0 && faces1[i] >= faces2[i] && faces1[i] >= faces3[i]) {
            triangle(i);
        }
        if (textureIndex == 2 && faces2[i] > 0 && faces2[i] >= faces1[i] && faces2[i] >= faces3[i]) {
            triangle(i);
        }
        if (textureIndex == 3 && faces3[i] > 0 && faces3[i] >= faces1[i] && faces3[i] >= faces2[i]) {
            triangle(i);
        }
    }
}

void drawMesh() {

    GLuint queries[model->TotalFaces];
    GLuint sampleCount;

    glGenQueriesARB(model->TotalFaces, queries);

    glDisable(GL_BLEND);
    glDepthFunc(GL_LESS);
    glDepthMask(GL_TRUE);

    for (int i = 0; i < model->TotalFaces; i++) {
        glBeginQueryARB(GL_SAMPLES_PASSED_ARB, queries[i]);
        triangle(i);
        glEndQueryARB(GL_SAMPLES_PASSED_ARB);
    }

    glEnable(GL_BLEND);
    glDepthFunc(GL_EQUAL);
    glDepthMask(GL_FALSE);

    for (int i = 0; i < model->TotalFaces; i++) {
        glGetQueryObjectuivARB(queries[i], GL_QUERY_RESULT_ARB, &sampleCount);
        if (sampleCount > 0) {
            if (textureIndex == 1) {
                faces1[i] = sampleCount;
            }
            if (textureIndex == 2) {
                faces2[i] = sampleCount;
            }
            if (textureIndex == 3) {
                faces3[i] = sampleCount;
            }
            triangle(i);
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
	glTranslatef(0.5f,0.5f,0.0f); //Bias
	glScalef(0.5f,1.0f,1.0f);		//Scale
	glFrustum(-0.035,0.035,-0.035,0.035,0.1,1.9); //MV for light map
	glTranslatef(0.0f,0.0f,-1.0f);
	glMultMatrixf(inverseMV.getMatrix());	//Inverse ModelView
	glMatrixMode(GL_MODELVIEW);
}


void stepTransformTexture() {
	if (modeTexture) {
        glRotatef(0, 1.0f,0.0f,0.0f);
        glRotatef(0, 0.0f,1.0f,0.0f);
        glRotatef(0, 0.0f,0.0f,1.0f);
	} else {
        glTranslatef(master[0].viewer[0],
                     master[0].viewer[1],
                     master[0].viewer[2] - 20);

        glRotatef(masterNow->rotate[0] - master[0].rotate[0], 1.0f,0.0f,0.0f);
        glRotatef(masterNow->rotate[1] - master[0].rotate[1], 0.0f,1.0f,0.0f);
        glRotatef(masterNow->rotate[2] - master[0].rotate[2], 0.0f,0.0f,1.0f);

        glTranslatef(-master[0].viewer[0],
                     -master[0].viewer[1],
                     -master[0].viewer[2] + 20);
        glTranslatef(master[0].viewer[0] - masterNow->viewer[0],
                     master[0].viewer[1] - masterNow->viewer[1],
                     master[0].viewer[2] - masterNow->viewer[2]);
	}
}

void stepTexture() {
	glPushMatrix();
	glLoadIdentity();
	if (textureIndex == 1) {
	    glActiveTextureARB(GL_TEXTURE0);
        glEnable(GL_TEXTURE_2D);
        stepTransformTexture();
	}
	if (textureIndex == 2) {
	    glActiveTextureARB(GL_TEXTURE1);
        glEnable(GL_TEXTURE_2D);
        stepTransformTexture();
	}
	if (textureIndex == 3) {
	    glActiveTextureARB(GL_TEXTURE2);
        glEnable(GL_TEXTURE_2D);
        stepTransformTexture();
	}
	glGetFloatv(GL_MODELVIEW_MATRIX,masterNow->MVmatrix);
	masterNow->TextureTransform.setMatrix(masterNow->MVmatrix);
	glPopMatrix();
	textureProjection(masterNow->TextureTransform);
}

void stepClearTexture() {
    glActiveTextureARB(GL_TEXTURE0);
    glDisable(GL_TEXTURE_2D);
    glActiveTextureARB(GL_TEXTURE1);
    glDisable(GL_TEXTURE_2D);
    glActiveTextureARB(GL_TEXTURE2);
    glDisable(GL_TEXTURE_2D);
}

void display(void) {
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

    if (!modeTexture) {
        textureIndex = 1;
        masterNow = &master[1];
        stepTexture();

        glLoadIdentity();
        glTranslatef(master[0].viewer[0], master[0].viewer[1], master[0].viewer[2] - 20);
        glRotatef(master[0].rotate[0], -1.0f,0.0f,0.0f);
        glRotatef(master[0].rotate[1], 0.0f,-1.0f,0.0f);
        glRotatef(master[0].rotate[2], 0.0f,0.0f,-1.0f);
        oldDrawMesh();
        stepClearTexture();

        textureIndex = 2;
        masterNow = &master[2];
        stepTexture();

        glLoadIdentity();
        glTranslatef(master[0].viewer[0], master[0].viewer[1], master[0].viewer[2] - 20);
        glRotatef(master[0].rotate[0], -1.0f,0.0f,0.0f);
        glRotatef(master[0].rotate[1], 0.0f,-1.0f,0.0f);
        glRotatef(master[0].rotate[2], 0.0f,0.0f,-1.0f);
        oldDrawMesh();
        stepClearTexture();

        textureIndex = 3;
        masterNow = &master[3];
        stepTexture();

        glLoadIdentity();
        glTranslatef(master[0].viewer[0], master[0].viewer[1], master[0].viewer[2] - 20);
        glRotatef(master[0].rotate[0], -1.0f,0.0f,0.0f);
        glRotatef(master[0].rotate[1], 0.0f,-1.0f,0.0f);
        glRotatef(master[0].rotate[2], 0.0f,0.0f,-1.0f);
        oldDrawMesh();
        stepClearTexture();

        textureIndex = 0;
        masterNow = &master[0];

    } else {
        stepTexture();

        glLoadIdentity();
        glTranslatef(masterNow->viewer[0], masterNow->viewer[1], masterNow->viewer[2] - 20);
        glRotatef(masterNow->rotate[0], -1.0f,0.0f,0.0f);
        glRotatef(masterNow->rotate[1], 0.0f,-1.0f,0.0f);
        glRotatef(masterNow->rotate[2], 0.0f,0.0f,-1.0f);

        drawMesh();
        stepClearTexture();
    }

	glFlush();
	glutSwapBuffers();

    writeText();
}

void loadCalibration() {
    for (int i = 1; i < 4; i++) {
        MasterTexture* masterNow = &master[i];
        std::stringstream fileName;
        fileName << "texture" << i << ".txt";
        std::ifstream in(fileName.str().c_str());
        std::stringstream buffer;
        buffer << in.rdbuf();

        string line;
        if (getline(buffer, line, '\n')) {
            istringstream subBuffer(line);
            string value;
            for (int i = 0; i < 3 && getline(subBuffer, value, ' '); i++) {
                masterNow->viewer[i] = ::atof(value.c_str());
            }
        }
        if (getline(buffer, line, '\n')) {
            istringstream subBuffer(line);
            string value;
            for (int i = 0; i < 3 && getline(subBuffer, value, ' '); i++) {
                masterNow->rotate[i] = ::atof(value.c_str());
            }
        }
        in.close();
    }
    textureIndex = 1;
    masterNow = &master[textureIndex];
    display();

    textureIndex = 2;
    masterNow = &master[textureIndex];
    display();

    textureIndex = 3;
    masterNow = &master[textureIndex];
    display();

    modeTexture = false;
    textureIndex = 0;
    masterNow = &master[0];
}

void saveCalibration() {
    for (int i = 1; i < 4; i++) {
        MasterTexture* masterNow = &master[i];
        std::stringstream fileName;
        fileName << "texture" << i << ".txt";
        std::ofstream out(fileName.str().c_str());
        out << masterNow->viewer[0] << " " << masterNow->viewer[1] << " " << masterNow->viewer[2] << endl;
        out << masterNow->rotate[0] << " " << masterNow->rotate[1] << " " << masterNow->rotate[2];
        out.close();
    }
}

void keys(unsigned char key, int x, int y) {
    if(key == 't' && modeTexture) {
        masterNow->rotate[0] = 0;
        masterNow->rotate[1] = 0;
        masterNow->rotate[2] = 0;
    }
    if(key == 'v' && textureIndex == 3) {
        modeTexture = false;
        textureIndex = 0;
        masterNow = &master[0];
    }
    if (key == 'l' && modeTexture) {
        loadCalibration();
    }
    if (key == 'k' && !modeTexture) {
        saveCalibration();
    }

	if(key == '1') {
        textureIndex = 1;
        masterNow = &master[1];
        display();
	}
	if(key == '2') {
        textureIndex = 2;
        masterNow = &master[2];
        display();
	}
	if(key == '3') {
        textureIndex = 3;
        masterNow = &master[3];
        display();
	}

	if(key == 'w') masterNow->rotate[0] += 2.0;
	if(key == 's') masterNow->rotate[0] -= 2.0;
	if(key == 'a') masterNow->rotate[1] += 2.0;
	if(key == 'd') masterNow->rotate[1] -= 2.0;
	if(key == 'e') masterNow->rotate[2] += 2.0;
	if(key == 'q') masterNow->rotate[2] -= 2.0;

	display();
}

void mouse(int btn, int state, int x, int y) {
    cameraAxis = state == GLUT_DOWN ? btn : -1;
    if (state == GLUT_DOWN) {
        cameraMove = y;
        oldViewer[0] = masterNow->viewer[0];
        oldViewer[1] = masterNow->viewer[1];
        oldViewer[2] = masterNow->viewer[2];
    }
    if (state == GLUT_UP) {
        cameraMove = -1;
    }

	display();
}

void mouseMove(int x, int y) {
	if (cameraAxis != -1)
	{
		float deltaMove = (y - cameraMove) * 0.1f;
        if (cameraAxis == GLUT_LEFT_BUTTON) {
            masterNow->viewer[0] = oldViewer[0] + deltaMove;
        } else if (cameraAxis == GLUT_RIGHT_BUTTON) {
            masterNow->viewer[1] = oldViewer[1] + deltaMove;
        } else if (cameraAxis == GLUT_MIDDLE_BUTTON) {
            masterNow->viewer[2] = oldViewer[2] + deltaMove;
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

    if (textureIndex == 1) {
        wImg1 = width;
        hImg1 = height;
    }
    if (textureIndex == 2) {
        wImg2 = width;
        hImg2 = height;
    }
    if (textureIndex == 3) {
        wImg3 = width;
        hImg3 = height;
    }

	FreeImage_Unload(dib);
}

int main(int argc, char **argv) {

    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            master[i].viewer[j] = 0.0;
            master[i].rotate[j] = 0.0;
        }
    }

    model = new Model_PLY();
    model->Load("mesh/antMesh.ply");
    TotalConnectedTriangles = model->TotalConnectedTriangles;
    TotalPoints = model->TotalPoints;
    TotalFaces = model->TotalFaces;
    MinCoord = model->MinCoord;
    MaxCoord = model->MaxCoord;
    AlfaCoord = model->AlfaCoord;

    writeText();

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE|GLUT_RGB|GLUT_DEPTH);
	glutInitWindowSize(500,500);
	glutCreateWindow("Texture project");
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

    glActiveTextureARB(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textures[0]);
    glDisable(GL_TEXTURE_2D);
    textureIndex = 1;
	loadLightMapTexture("img/texture1.bmp");

    glActiveTextureARB(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, textures[1]);
    glDisable(GL_TEXTURE_2D);
    textureIndex = 2;
	loadLightMapTexture("img/texture2.jpg");

    glActiveTextureARB(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, textures[2]);
    glDisable(GL_TEXTURE_2D);
    textureIndex = 3;
	loadLightMapTexture("img/texture3.jpg");

    textureIndex = 0;
    masterNow = &master[0];

	glClearColor( 0.0f, 0.0f, 0.0f, 1.0f );

	glutMainLoop();
}
