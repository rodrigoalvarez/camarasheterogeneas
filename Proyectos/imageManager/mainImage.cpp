#include "modelPly.h"
#include <stdlib.h>
#include <windows.h>
#include <GL/glut.h>
#include "modelXYZ.h"
#include "modelImg.h"
#include "masterPly.h"
#include "masterSettings.h"
#include <vector>
#include <fstream>
#include <sstream>
#include <iostream>

#include "FreeImage.h"
#include "matrix4x4.h"

using namespace std;

GLuint *textures = new GLuint[3];

MasterSettings* settings = NULL;

/* Texture */

Model_IMG* textureImage = NULL;
Model_PLY* textureModel = NULL;
MasterTexture* textureMaster = NULL;
bool textureViewMode = false;
bool drawFast = true;
bool textureWire = true;

int textureCount = 1;
int textureIndex = 0;

int facesCount = 200000;
int** faces;
float frustum[6][4];

/* Camera */

float cameraFactor = 1.0;
int cameraAxis = -1;
int cameraMove = -1;


void writeText() {
    system("cls");
    cout << "2D CALIBRATION" << endl;
    cout << "Triangles: " << textureModel->TotalConnectedTriangles << endl;
    cout << "Points: " << textureModel->TotalPoints << endl;
    cout << "Faces: " << textureModel->TotalFaces << endl;
    cout << "MinCoord: " << textureModel->MinCoord << endl;
    cout << "MaxCoord: " << textureModel->MaxCoord << endl;
    cout << "AlfaCoord: " << textureModel->AlfaCoord << endl;
    cout << "Mode: " << (textureIndex == 0 ? "View" : "Calibration") << endl << endl;
    for (int i = 0; i <= textureCount; i++) {
        MasterTexture* masterNow = &textureMaster[i];
        cout << "Texture :: " << i << endl;
        cout << "Origin position..." << endl << masterNow->viewer[0]  << " " << masterNow->viewer[1]  << " " << masterNow->viewer[2] << endl;
        cout << "Object rotate..." << endl << masterNow->rotate[0]  << " " << masterNow->rotate[1]  << " " << masterNow->rotate[2] << endl;
        cout << endl;
    }
}

void setFaceVertex(int index) {
    GLfloat vert[3] = { textureModel->Faces_Triangles[index * 3], textureModel->Faces_Triangles[index * 3 + 1], textureModel->Faces_Triangles[index * 3 + 2] };
    glVertex3fv(vert);
    glNormal3f(textureModel->Normals[index * 3], textureModel->Normals[index * 3 + 1], textureModel->Normals[index * 3 + 2]);
}

void draw2DElement(int index) {

    glColor3f(1.0f, 1.0f, 1.0f);
    glBegin(GL_POLYGON);
        setFaceVertex(index * 3);
        setFaceVertex(index * 3 + 1);
        setFaceVertex(index * 3 + 2);
    glEnd();
    if (textureWire) {
        glDisable(GL_LIGHT0);
        glDisable(GL_LIGHTING);
        glColor3f(0.5f, 0.5f, 0.5f);
        glBegin(GL_LINE_LOOP);
            setFaceVertex(index * 3);
            setFaceVertex(index * 3 + 1);
            setFaceVertex(index * 3 + 2);
        glEnd();
        glEnable(GL_LIGHTING);
        glEnable(GL_LIGHT0);
    }
}

void draw2DBackground() {

    if (textureIndex > 0) {
        float wImg = textureImage[textureIndex-1].Width;
        float hImg = textureImage[textureIndex-1].Height;

        glColor3f(0.0f, 0.0f, 0.0f);
        glBegin(GL_POLYGON);
            GLfloat vert1[3] = { -wImg, -hImg, -20.0 };
            glVertex3fv(vert1);
            glNormal3f(0,0,1);
            GLfloat vert2[3] = { -wImg, hImg, -20.0 };
            glVertex3fv(vert2);
            glNormal3f(0,0,1);
            GLfloat vert3[3] = { wImg, hImg, -20.0 };
            glVertex3fv(vert3);
            glNormal3f(0,0,1);
            GLfloat vert4[3] = { wImg, -hImg, -20.0 };
            glVertex3fv(vert4);
            glNormal3f(0,0,1);
        glEnd();
    }
}

void ExtractFrustum() {
    float   proj[16];
    float   modl[16];
    float   clip[16];
    float   t;

    /* Get the current PROJECTION matrix from OpenGL */
    glGetFloatv( GL_PROJECTION_MATRIX, proj );

    /* Get the current MODELVIEW matrix from OpenGL */
    glGetFloatv( GL_MODELVIEW_MATRIX, modl );

    /* Combine the two matrices (multiply projection by modelview) */
    clip[ 0] = modl[ 0] * proj[ 0] + modl[ 1] * proj[ 4] + modl[ 2] * proj[ 8] + modl[ 3] * proj[12];
    clip[ 1] = modl[ 0] * proj[ 1] + modl[ 1] * proj[ 5] + modl[ 2] * proj[ 9] + modl[ 3] * proj[13];
    clip[ 2] = modl[ 0] * proj[ 2] + modl[ 1] * proj[ 6] + modl[ 2] * proj[10] + modl[ 3] * proj[14];
    clip[ 3] = modl[ 0] * proj[ 3] + modl[ 1] * proj[ 7] + modl[ 2] * proj[11] + modl[ 3] * proj[15];

    clip[ 4] = modl[ 4] * proj[ 0] + modl[ 5] * proj[ 4] + modl[ 6] * proj[ 8] + modl[ 7] * proj[12];
    clip[ 5] = modl[ 4] * proj[ 1] + modl[ 5] * proj[ 5] + modl[ 6] * proj[ 9] + modl[ 7] * proj[13];
    clip[ 6] = modl[ 4] * proj[ 2] + modl[ 5] * proj[ 6] + modl[ 6] * proj[10] + modl[ 7] * proj[14];
    clip[ 7] = modl[ 4] * proj[ 3] + modl[ 5] * proj[ 7] + modl[ 6] * proj[11] + modl[ 7] * proj[15];

    clip[ 8] = modl[ 8] * proj[ 0] + modl[ 9] * proj[ 4] + modl[10] * proj[ 8] + modl[11] * proj[12];
    clip[ 9] = modl[ 8] * proj[ 1] + modl[ 9] * proj[ 5] + modl[10] * proj[ 9] + modl[11] * proj[13];
    clip[10] = modl[ 8] * proj[ 2] + modl[ 9] * proj[ 6] + modl[10] * proj[10] + modl[11] * proj[14];
    clip[11] = modl[ 8] * proj[ 3] + modl[ 9] * proj[ 7] + modl[10] * proj[11] + modl[11] * proj[15];

    clip[12] = modl[12] * proj[ 0] + modl[13] * proj[ 4] + modl[14] * proj[ 8] + modl[15] * proj[12];
    clip[13] = modl[12] * proj[ 1] + modl[13] * proj[ 5] + modl[14] * proj[ 9] + modl[15] * proj[13];
    clip[14] = modl[12] * proj[ 2] + modl[13] * proj[ 6] + modl[14] * proj[10] + modl[15] * proj[14];
    clip[15] = modl[12] * proj[ 3] + modl[13] * proj[ 7] + modl[14] * proj[11] + modl[15] * proj[15];

    /* Extract the numbers for the RIGHT plane */
    frustum[0][0] = clip[ 3] - clip[ 0];
    frustum[0][1] = clip[ 7] - clip[ 4];
    frustum[0][2] = clip[11] - clip[ 8];
    frustum[0][3] = clip[15] - clip[12];

    /* Normalize the result */
    t = sqrt( frustum[0][0] * frustum[0][0] + frustum[0][1] * frustum[0][1] + frustum[0][2] * frustum[0][2] );
    frustum[0][0] /= t;
    frustum[0][1] /= t;
    frustum[0][2] /= t;
    frustum[0][3] /= t;

    /* Extract the numbers for the LEFT plane */
    frustum[1][0] = clip[ 3] + clip[ 0];
    frustum[1][1] = clip[ 7] + clip[ 4];
    frustum[1][2] = clip[11] + clip[ 8];
    frustum[1][3] = clip[15] + clip[12];

    /* Normalize the result */
    t = sqrt( frustum[1][0] * frustum[1][0] + frustum[1][1] * frustum[1][1] + frustum[1][2] * frustum[1][2] );
    frustum[1][0] /= t;
    frustum[1][1] /= t;
    frustum[1][2] /= t;
    frustum[1][3] /= t;

    /* Extract the BOTTOM plane */
    frustum[2][0] = clip[ 3] + clip[ 1];
    frustum[2][1] = clip[ 7] + clip[ 5];
    frustum[2][2] = clip[11] + clip[ 9];
    frustum[2][3] = clip[15] + clip[13];

    /* Normalize the result */
    t = sqrt( frustum[2][0] * frustum[2][0] + frustum[2][1] * frustum[2][1] + frustum[2][2] * frustum[2][2] );
    frustum[2][0] /= t;
    frustum[2][1] /= t;
    frustum[2][2] /= t;
    frustum[2][3] /= t;

    /* Extract the TOP plane */
    frustum[3][0] = clip[ 3] - clip[ 1];
    frustum[3][1] = clip[ 7] - clip[ 5];
    frustum[3][2] = clip[11] - clip[ 9];
    frustum[3][3] = clip[15] - clip[13];

    /* Normalize the result */
    t = sqrt( frustum[3][0] * frustum[3][0] + frustum[3][1] * frustum[3][1] + frustum[3][2] * frustum[3][2] );
    frustum[3][0] /= t;
    frustum[3][1] /= t;
    frustum[3][2] /= t;
    frustum[3][3] /= t;

    /* Extract the FAR plane */
    frustum[4][0] = clip[ 3] - clip[ 2];
    frustum[4][1] = clip[ 7] - clip[ 6];
    frustum[4][2] = clip[11] - clip[10];
    frustum[4][3] = clip[15] - clip[14];

    /* Normalize the result */
    t = sqrt( frustum[4][0] * frustum[4][0] + frustum[4][1] * frustum[4][1] + frustum[4][2] * frustum[4][2] );
    frustum[4][0] /= t;
    frustum[4][1] /= t;
    frustum[4][2] /= t;
    frustum[4][3] /= t;

    /* Extract the NEAR plane */
    frustum[5][0] = clip[ 3] + clip[ 2];
    frustum[5][1] = clip[ 7] + clip[ 6];
    frustum[5][2] = clip[11] + clip[10];
    frustum[5][3] = clip[15] + clip[14];

    /* Normalize the result */
    t = sqrt( frustum[5][0] * frustum[5][0] + frustum[5][1] * frustum[5][1] + frustum[5][2] * frustum[5][2] );
    frustum[5][0] /= t;
    frustum[5][1] /= t;
    frustum[5][2] /= t;
    frustum[5][3] /= t;
}

bool PointInFrustum(float x, float y, float z) {
    int p;
    for (p = 0; p < 6; p++) {
        if (frustum[p][0] * x + frustum[p][1] * y + frustum[p][2] * z + frustum[p][3] <= 0) {
            return false;
        }
    }
    return true;
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

void draw2DCalibrationFull() {
    ExtractFrustum();
    GLuint queries[textureModel->TotalFaces];
    GLuint sampleCount;
    glGenQueriesARB(textureModel->TotalFaces, queries);
    glDisable(GL_BLEND);
    glDepthFunc(GL_LESS);
    glDepthMask(GL_TRUE);
    for (int i = 0; i < textureModel->TotalFaces; i++) {
        int index = i * 3;
        if (PointInFrustum(textureModel->Faces_Triangles[index * 3], textureModel->Faces_Triangles[index * 3 + 1], textureModel->Faces_Triangles[index * 3 + 2])) {
            glBeginQueryARB(GL_SAMPLES_PASSED_ARB, queries[i]);
            draw2DElement(i);
            glEndQueryARB(GL_SAMPLES_PASSED_ARB);
        }
    }
    glEnable(GL_BLEND);
    glDepthFunc(GL_EQUAL);
    glDepthMask(GL_FALSE);

    for (int i = 0; i < textureModel->TotalFaces; i++) {
        int index = i * 3;
        if (PointInFrustum(textureModel->Faces_Triangles[index * 3], textureModel->Faces_Triangles[index * 3 + 1], textureModel->Faces_Triangles[index * 3 + 2])) {
            glGetQueryObjectuivARB(queries[i], GL_QUERY_RESULT_ARB, &sampleCount);
            if (sampleCount > 0) {
                if (textureIndex > 0) {
                    faces[textureIndex][i] = sampleCount;
                }
                draw2DElement(i);
            }
        }
    }
    glDisable(GL_BLEND);
    glDepthFunc(GL_LESS);
    glDepthMask(GL_TRUE);
}

void draw2DCalibrationFast() {
    for (int i = 0; i < textureModel->TotalFaces; i++) {
        draw2DElement(i);
    }
}


void textureProjection(Matrix4x4f &mv) {

    Matrix4x4f inverseMV = Matrix4x4f::invertMatrix(mv);
    glMatrixMode(GL_TEXTURE);
    glLoadIdentity();
    glTranslatef(0.5f,0.5f,0.0f);
    float wImg = textureImage[textureIndex-1].Width;
    float hImg = textureImage[textureIndex-1].Height;
    if (wImg < hImg) {
        glScalef(1.0f,(1.f*wImg)/hImg,1.0f);
    } else {
        glScalef((1.f*hImg)/wImg,1.0f,1.0f);
    }
    glFrustum(-0.035,0.035,-0.035,0.035,0.02,2.0);
    glMultMatrixf(inverseMV.getMatrix());
    glMatrixMode(GL_MODELVIEW);
}


void stepTransformTexture() {
    if (textureViewMode) {
        glTranslatef(textureMaster[0].viewer[0],
                     textureMaster[0].viewer[1],
                     textureMaster[0].viewer[2] - 20);

        glRotatef(textureMaster[textureIndex].rotate[0] - textureMaster[0].rotate[0], 1.0f,0.0f,0.0f);
        glRotatef(textureMaster[textureIndex].rotate[1] - textureMaster[0].rotate[1], 0.0f,1.0f,0.0f);
        glRotatef(textureMaster[textureIndex].rotate[2] - textureMaster[0].rotate[2], 0.0f,0.0f,1.0f);

        glTranslatef(-textureMaster[0].viewer[0],
                     -textureMaster[0].viewer[1],
                     -textureMaster[0].viewer[2] + 20);
        glTranslatef(textureMaster[0].viewer[0] - textureMaster[textureIndex].viewer[0],
                     textureMaster[0].viewer[1] - textureMaster[textureIndex].viewer[1],
                     textureMaster[0].viewer[2] - textureMaster[textureIndex].viewer[2]);
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

    glGetFloatv(GL_MODELVIEW_MATRIX, textureMaster[textureIndex].MVmatrix);
    textureMaster[textureIndex].TextureTransform.setMatrix(textureMaster[textureIndex].MVmatrix);
    glPopMatrix();
    textureProjection(textureMaster[textureIndex].TextureTransform);
}

void stepClearTexture() {
    for (int i = 0; i < textureCount; i++) {
        glActiveTextureARB(GL_TEXTURE0 + i);
        glDisable(GL_TEXTURE_2D);
    }
}

void IncludeMesh (Model_XYZ* model, Model_XYZ* newModel, MasterMesh master) {
    GLdouble m[16];
    MasterSettings::CalculateMatrix(master, m);
    model->Include(newModel, m);
}

void display(void) {
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);

    glPushMatrix();
    glLoadIdentity();
    GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 0.5 };
    GLfloat mat_shininess[] = { 50.0 };
    GLfloat light_color[] = { 1., 1., 1., 0.5 };
    GLfloat light_position[] = { 0.0, 0.0, 1.0, 0.0 };

    glShadeModel (GL_SMOOTH);
    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light_color);
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);
    glPopMatrix();


    if (textureViewMode) {
        for (int i = 1; i <= textureCount; i++) {
            textureIndex = i;
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

    } else {
        stepTexture();
        glLoadIdentity();
        glTranslatef(textureMaster[textureIndex].viewer[0], textureMaster[textureIndex].viewer[1], textureMaster[textureIndex].viewer[2] - 20);
        glRotatef(textureMaster[textureIndex].rotate[0], -1.0f,0.0f,0.0f);
        glRotatef(textureMaster[textureIndex].rotate[1], 0.0f,-1.0f,0.0f);
        glRotatef(textureMaster[textureIndex].rotate[2], 0.0f,0.0f,-1.0f);
        if (drawFast) {
            draw2DCalibrationFast();
        } else {
            draw2DCalibrationFull();
        }
        glPushMatrix();
        glLoadIdentity();
        draw2DBackground();
        glPopMatrix();
        stepClearTexture();
    }

    glDisable(GL_LIGHT0);
    glDisable(GL_LIGHTING);

    glFlush();
    glutSwapBuffers();

    writeText();
}

void keys(unsigned char key, int x, int y) {

    if (key == 'l') {
        settings->loadTextureCalibration();
        for (int i = 1; i <= textureCount; i++) {
            textureIndex = i;
            display();
        }
        textureViewMode = true;
        textureIndex = 0;
    }
    if (key == 'k') {
        settings->saveTextureCalibration();
    }
    if (key == 'm') {
        textureWire = true;
    }
    if (key == 'n') {
        textureWire = false;
    }
    if(key == 'v') {
        drawFast = false;
        display();
        drawFast = true;
        textureViewMode = true;
        textureIndex = 0;
    }
    if(key >= '1' && key <= '9' && (key - 48 <= textureCount)) {
        drawFast = false;
        display();
        drawFast = true;
        textureViewMode = false;
        textureIndex = key - 48;
        display();
    }
    if(key == '+') cameraFactor *= 1.25;
    if(key == '-') cameraFactor *= 0.8;

    if(key == 'w') textureMaster[textureIndex].rotate[0] += 2.0 * cameraFactor;
    if(key == 's') textureMaster[textureIndex].rotate[0] -= 2.0 * cameraFactor;
    if(key == 'a') textureMaster[textureIndex].rotate[1] += 2.0 * cameraFactor;
    if(key == 'd') textureMaster[textureIndex].rotate[1] -= 2.0 * cameraFactor;
    if(key == 'e') textureMaster[textureIndex].rotate[2] += 2.0 * cameraFactor;
    if(key == 'q') textureMaster[textureIndex].rotate[2] -= 2.0 * cameraFactor;

    display();
}

void mouse(int btn, int state, int x, int y) {

    cameraAxis = state == GLUT_DOWN ? btn : -1;
    if (state == GLUT_DOWN) {
        cameraMove = y;
    }
    if (state == GLUT_UP) {
        cameraMove = -1;
    }
    display();
}

void mouseMove(int x, int y) {

    if (cameraAxis != -1) {
        float deltaMove = (y - cameraMove) * 0.1f * cameraFactor;
        cameraMove = y;
        if (cameraAxis == GLUT_LEFT_BUTTON) {
            textureMaster[textureIndex].viewer[0] += deltaMove;
        } else if (cameraAxis == GLUT_RIGHT_BUTTON) {
            textureMaster[textureIndex].viewer[1] += deltaMove;
        } else if (cameraAxis == GLUT_MIDDLE_BUTTON) {
            textureMaster[textureIndex].viewer[2] += deltaMove;
        }
        display();
    }
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

void loadLightMapTexture(Model_IMG* model, string file) {

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

    model->Load(file);

    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, 0x812D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, 0x812D);
    glTexParameterfv(GL_TEXTURE_2D,GL_TEXTURE_BORDER_COLOR,borderColor);
    gluBuild2DMipmaps(GL_TEXTURE_2D,GL_RGB,model->Width,model->Height,GL_BGR,GL_UNSIGNED_BYTE,model->Pixels);
}

vector<string> getImageFiles() {
    vector<string> files;
    ofDirectory directory("C:\\of_v0073_win_cb_release\\apps\\myApps\\reproductorUnido\\img");
    directory.allowExt("png");
    directory.allowExt("bmp");
    directory.allowExt("jpg");
    directory.listDir();
    for(int i = 0; i < directory.numFiles(); i++) {
        files.push_back(directory.getPath(i));
    }
    return files;
}

vector<string> getMeshFiles() {
    vector<string> files;
    ofDirectory directory("C:\\of_v0073_win_cb_release\\apps\\myApps\\reproductorUnido\\mesh");
    directory.allowExt("ply");
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

    /* Mesh */
    textureModel = new Model_PLY();
    vector<string> files3D = getMeshFiles();
    textureModel->Load(files3D[0]);

    /* Texture */
    vector<string> files2D = getImageFiles();
    textureCount = files2D.size();
    textureMaster = new MasterTexture[textureCount + 1];
    faces = new int*[textureCount + 1];
    for (int i = 0; i <= textureCount; i++) {
        for (int j = 0; j < 3; j++) {
            textureMaster[i].viewer[j] = 0.0;
            textureMaster[i].rotate[j] = 0.0;
        }
        faces[i] = new int[facesCount];
    }

    /* Settings and files */
    settings = new MasterSettings(textureCount, textureMaster, 0, NULL);

    /* Texture config */
    glGenTextures(textureCount, textures);
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
    textureImage = new Model_IMG[textureCount];
    for (int i = 0; i < textureCount; i++) {
        glActiveTextureARB(GL_TEXTURE0 + i);
        glBindTexture(GL_TEXTURE_2D, textures[i]);
        glDisable(GL_TEXTURE_2D);
        textureIndex = i + 1;
        loadLightMapTexture(&textureImage[i], files2D[i]);
    }
    textureIndex = 0;

    /* Start windows */
    writeText();
    glutMainLoop();
}
