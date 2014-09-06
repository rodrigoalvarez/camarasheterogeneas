#include <GL/glew.h>
#include<stdlib.h>
#include<windows.h>
#include<GL/glut.h>
#include "Model_PLY.h"
#include "masterPly.h"
#include<iostream>
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
    GLdouble newViewer[3];

    Matrix4x4f TextureTransform;
    float MVmatrix[16];

    float rotateX;
    float rotateY;
    float rotateZ;
    float newRotateX;
    float newRotateY;
    float newRotateZ;
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

//int countHits = 0;

void writeText() {
    system("cls");
    cout << "Triangles: " << TotalConnectedTriangles << endl;
    cout << "Points: " << TotalPoints << endl;
    cout << "Faces: " << TotalFaces << endl;
    cout << "MinCoord: " << MinCoord << endl;
    cout << "MaxCoord: " << MaxCoord << endl;
    cout << "AlfaCoord: " << AlfaCoord << endl;
    cout << "Mode: " << (modeTexture ? "Texture" : "View") << endl << endl;
    //cout << "Hints: " << countHits << endl << endl;

    for (int i = 0; i < 4; i++) {
        MasterTexture* masterNow = &master[i];
        cout << "Texture :: " << i << endl;
        cout << "Origin position..." << endl << masterNow->viewer[0]  << " " << masterNow->viewer[1]  << " " << masterNow->viewer[2] << endl;
        cout << "Origin new position..." << endl << masterNow->newViewer[0]  << " " << masterNow->newViewer[1]  << " " << masterNow->newViewer[2] << endl;
        cout << "Object rotate..." << endl << masterNow->rotateX  << " " << masterNow->rotateY  << " " << masterNow->rotateZ << endl;
        cout << "Texture rotate..." << endl << masterNow->newRotateX  << " " << masterNow->newRotateY  << " " << masterNow->newRotateZ << endl << endl;
    }/**/
}

void setVertex(int index) {
    GLfloat vert[3] = { model->Faces_Triangles[index * 3], model->Faces_Triangles[index * 3 + 1], model->Faces_Triangles[index * 3 + 2] };
    glVertex3fv(vert);
}

//GLuint localHits = 0;
//int testFaces[912];

void triangle(int index) {

    /*if (localHits > 150) {
        glColor3f(1.0f, 0.0f, 0.0f);
    } else if (localHits > 100) {
        glColor3f(1.0f, 0.3f, 0.3f);
    } else if (localHits > 50) {
        glColor3f(1.0f, 0.6f, 0.6f);
    } else if (localHits > 0) {
        glColor3f(1.0f, 0.8f, 0.8f);
    } else {
        glColor3f(1.0f, 1.0f, 1.0f);
    }
    glColor3f(1.0f, 0.0f, 0.0f);*/

    glColor3f(1.0f, 1.0f, 1.0f);

	glBegin(GL_POLYGON);
		//glColor3fv(colors[0]);
		setVertex(index * 3);
		//glColor3fv(colors[1]);
		setVertex(index * 3 + 1);
		//glColor3fv(colors[2]);
		setVertex(index * 3 + 2);
	glEnd();
    glBegin(GL_LINE_LOOP);
        glColor3f(0.5f, 0.5f, 0.5f);
        setVertex(index * 3);
        glColor3f(0.5f, 0.5f, 0.5f);
        setVertex(index * 3 + 1);
        glColor3f(0.5f, 0.5f, 0.5f);
        setVertex(index * 3 + 2);
    glEnd();/**/
}

/*void oldDrawMesh() {
    for (int i = 0; i < model->TotalFaces; i++) {
        localHits = testFaces[i];
        if (localHits > 0) {
            triangle(i);
        }
    }
}*/

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

    //countHits = 0;
    for (int i = 0; i < model->TotalFaces; i++) {
        glGetQueryObjectuivARB(queries[i], GL_QUERY_RESULT_ARB, &sampleCount);
        if (sampleCount > 0) {
            //testFaces[i] = sampleCount;
            //localHits = sampleCount;
            triangle(i);
            //countHits++;
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
	glScalef(0.5f,0.5f,1.0f);		//Scale
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

        glRotatef(masterNow->newRotateX - master[0].rotateX, 1.0f,0.0f,0.0f);
        glRotatef(masterNow->newRotateY - master[0].rotateY, 0.0f,1.0f,0.0f);
        glRotatef(masterNow->newRotateZ - master[0].rotateZ, 0.0f,0.0f,1.0f);

        glTranslatef(-master[0].viewer[0],
                     -master[0].viewer[1],
                     -master[0].viewer[2] + 20);
        glTranslatef(master[0].viewer[0] - masterNow->newViewer[0],
                     master[0].viewer[1] - masterNow->newViewer[1],
                     master[0].viewer[2] - masterNow->newViewer[2]);
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

//bool modeHits = true;

void display(void) {
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

    if (!modeTexture) {
        textureIndex = 1;
        masterNow = &master[1];
        stepTexture();
        textureIndex = 2;
        masterNow = &master[2];
        stepTexture();
        textureIndex = 3;
        masterNow = &master[3];
        stepTexture();/**/

        textureIndex = 0;
        masterNow = &master[0];
    } else {
        stepTexture();
    }

	glLoadIdentity();
	glTranslatef(masterNow->viewer[0], masterNow->viewer[1], masterNow->viewer[2] - 20);
	glRotatef(masterNow->rotateX, -1.0f,0.0f,0.0f);
    glRotatef(masterNow->rotateY, 0.0f,-1.0f,0.0f);
    glRotatef(masterNow->rotateZ, 0.0f,0.0f,-1.0f);

    drawMesh();
    /*if (modeHits) {
        drawMesh();
    } else {
        oldDrawMesh();
    }*/

	stepClearTexture();
	glFlush();
	glutSwapBuffers();

    writeText();
}

void savePosition() {
    masterNow->newRotateX = masterNow->rotateX;
    masterNow->newRotateY = masterNow->rotateY;
    masterNow->newRotateZ = masterNow->rotateZ;
    masterNow->newViewer[0] = masterNow->viewer[0];
    masterNow->newViewer[1] = masterNow->viewer[1];
    masterNow->newViewer[2] = masterNow->viewer[2];
}

void keys(unsigned char key, int x, int y) {
    if(key == 't' && modeTexture) {
        masterNow->newRotateX = 0;
        masterNow->newRotateY = 0;
        masterNow->newRotateZ = 0;
    }
    if(key == 'v' && textureIndex == 3) {
        savePosition();
        modeTexture = false;
        textureIndex = 0;
        masterNow = &master[0];
    }
    /*if (key == 'h') {
        modeHits = false;
    }*/

	if(key == '1') {
        savePosition();
        textureIndex = 1;
        masterNow = &master[1];
        display();
	}
	if(key == '2') {
        savePosition();
        textureIndex = 2;
        masterNow = &master[2];
        display();
	}
	if(key == '3') {
        savePosition();
        textureIndex = 3;
        masterNow = &master[3];
        display();
	}

	if(key == 'w') masterNow->rotateX += 2.0;
	if(key == 's') masterNow->rotateX -= 2.0;
	if(key == 'a') masterNow->rotateY += 2.0;
	if(key == 'd') masterNow->rotateY -= 2.0;
	if(key == 'e') masterNow->rotateZ += 2.0;
	if(key == 'q') masterNow->rotateZ -= 2.0;

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

	//set up texture generation mode and set the corresponding planes
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

	FreeImage_Unload(dib);
}

int main(int argc, char **argv) {

    //MasterPly* ply = new MasterPly();
    //ply->Sampling("mallaRodrigo.ply", "test");

    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            master[i].viewer[j] = 0.0;
            master[i].newViewer[j] = 0.0;
        }
        master[i].rotateX = 0;
        master[i].rotateY = 0;
        master[i].rotateZ = 0;
        master[i].newRotateX = 0;
        master[i].newRotateY = 0;
        master[i].newRotateZ = 0;
    }
    textureIndex = 0;
    masterNow = &master[0];

    model = new Model_PLY();
    model->Load("mallaRodrigo.ply");
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
	loadLightMapTexture("lightmap.bmp");

    glActiveTextureARB(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, textures[1]);
    glDisable(GL_TEXTURE_2D);
	loadLightMapTexture("lightmap3.JPG");

    glActiveTextureARB(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, textures[2]);
    glDisable(GL_TEXTURE_2D);
	loadLightMapTexture("lightmap2.JPG");

	glClearColor( 0.0f, 0.0f, 0.0f, 1.0f );

	glutMainLoop();
}

