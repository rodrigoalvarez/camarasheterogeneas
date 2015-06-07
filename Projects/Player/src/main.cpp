#include "modelImg.h"
#include "modelPly.h"
//#include <GL/glew.h>
#include <stdlib.h>
#include <windows.h>
#include <GL/glut.h>
#include "modelXYZ.h"
#include "masterPly.h"
#include "masterSettings.h"
#include <vector>
#include <fstream>
#include <sstream>
#include <iostream>
#include "ModelSetting.h"

#include "FreeImage.h"
#include "matrix4x4.h"

using namespace std;

GLuint *textures = new GLuint[3];

MasterSettings* settings = NULL;

/* Texture */

Model_SET* textureSetting = NULL;
Model_IMG* textureImage = NULL;
Model_PLY* textureModel = NULL;
MasterTexture* textureMaster = NULL;
bool drawFast = false;
bool textureWire = true;

int textureCount = 1;
int textureIndex = 0;

int reDrawRate = 200;
int facesCount = 200000;
int** faces;
float frustum[6][4];

/* Camera */

int cameraAxis = -1;
int cameraMove = -1;

float drawXmin;
float drawXmax;
float drawYmin;
float drawYmax;

HINSTANCE occlusionLibrary;

typedef void (*f_OcclusionCulling)(int textureIndex,int TotalFaces, float* Faces_Triangles, int*** faces, float drawXmin, float drawXmax, float drawYmin, float drawYmax);

void drawText(const char* text, int length, int x, int y) {
    glMatrixMode(GL_PROJECTION);
    double* matrix = new double[16];
    glGetDoublev(GL_PROJECTION_MATRIX, matrix);
    glLoadIdentity();
    int winW = glutGet(GLUT_WINDOW_WIDTH);
    int winH = glutGet(GLUT_WINDOW_HEIGHT);
    glOrtho(0, winW, 0, winH, -5, 5);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glPushMatrix();
    glLoadIdentity();
    glRasterPos2i(x, y);
    for (int i = 0; i < length; i++) {
        glutBitmapCharacter(GLUT_BITMAP_9_BY_15, (int)text[i]);
    }
    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glLoadMatrixd(matrix);
    glMatrixMode(GL_MODELVIEW);
}

void drawAllText() {

    int positionY = 10;
    for (int i = 0; i <= textureCount; i++) {
        MasterTexture* masterNow = &textureMaster[i];
        std::ostringstream intIndex; intIndex << i;
        std::ostringstream intX; intX << masterNow->viewer[0];
        std::ostringstream intY; intY << masterNow->viewer[1];
        std::ostringstream intZ; intZ << masterNow->viewer[2];
        std::ostringstream intA; intA << masterNow->rotate[0];
        std::ostringstream intB; intB << masterNow->rotate[1];
        std::ostringstream intC; intC << masterNow->rotate[2];

        string textTexture = "Texture ";
        textTexture = textTexture + intIndex.str() + " :: ";
        textTexture = textTexture + " [ Position: " + intX.str() + " | " + intY.str() + " | " + intZ.str() + " ]";
        textTexture = textTexture + " [ Rotation: " + intA.str() + " | " + intB.str() + " | " + intC.str() + " ]";
        drawText(textTexture.data(), textTexture.size(), 10, positionY);
        positionY += 20;
    }

    std::ostringstream intPoints; intPoints << textureModel->TotalPoints;
    std::ostringstream intFaces; intFaces << textureModel->TotalFaces;
    std::ostringstream intCoordMin; intCoordMin << textureModel->MinCoord;
    std::ostringstream intCoordMax; intCoordMax << textureModel->MaxCoord;
    std::ostringstream intCoordAlfa; intCoordAlfa << textureModel->AlfaCoord;
    string textMesh = "Mesh: ";
    textMesh = textMesh + " [ Points: " + intPoints.str() + " ]";
    textMesh = textMesh + " [ Faces: " + intFaces.str() + " ]";
    textMesh = textMesh + " [ Coordinates: " + intCoordMin.str() + " | " + intCoordMax.str() + " | " + intCoordAlfa.str() + " ]";
    drawText(textMesh.data(), textMesh.size(), 10, positionY);
    positionY += 20;

    string textTitle = "PLAYER";
    drawText(textTitle.data(), textTitle.size(), 10, positionY);
    positionY += 20;
}

float isFrontFacePoints(float* points) {
    GLfloat p1[3] = { points[0], points[1], points[2] };
    GLfloat p2[3] = { points[3], points[4], points[5] };
    GLfloat p3[3] = { points[6], points[7], points[8] };

    float va[3], vb[3], vr[3], val;
	va[0] = p2[0] - p1[0];
	va[1] = p2[1] - p1[1];
	va[2] = p2[2] - p1[2];

	vb[0] = p3[0] - p1[0];
	vb[1] = p3[1] - p1[1];
	vb[2] = p3[2] - p1[2];

	/* cross product */
	vr[0] = va[1] * vb[2] - vb[1] * va[2];
	vr[1] = vb[0] * va[2] - va[0] * vb[2];
	vr[2] = va[0] * vb[1] - vb[0] * va[1];

	return vr[2];
}

bool isFrontFace(int index) {
    float points[9] = { textureModel->Faces_Triangles[index * 9], textureModel->Faces_Triangles[index * 9 + 1], textureModel->Faces_Triangles[index * 9 + 2],
                        textureModel->Faces_Triangles[index * 9 + 3], textureModel->Faces_Triangles[index * 9 + 4], textureModel->Faces_Triangles[index * 9 + 5],
                        textureModel->Faces_Triangles[index * 9 + 6], textureModel->Faces_Triangles[index * 9 + 7], textureModel->Faces_Triangles[index * 9 + 8] };
    return isFrontFacePoints(points) <= 0;
}

void calcBackground(GLfloat* vert) {
    GLdouble model_view[16];
    glGetDoublev(GL_MODELVIEW_MATRIX, model_view);
    GLdouble projection[16];
    glGetDoublev(GL_PROJECTION_MATRIX, projection);
    GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);

    //cout << "MV " << model_view[0] << " " << model_view[1] << " " << model_view[2] << endl;
    //cout << "PJ " << projection[0] << " " << projection[1] << " " << projection[2] << endl;
    //cout << "VP " << viewport[0] << " " << viewport[1] << " " << viewport[2] << endl;

    GLdouble pos3D_x, pos3D_y, pos3D_z;
    pos3D_x = vert[0];
    pos3D_y = vert[1];
    pos3D_z = vert[2];
    GLdouble winX, winY, winZ;
    gluProject(pos3D_x, pos3D_y, pos3D_z,
        model_view, projection, viewport,
        &winX, &winY, &winZ);

    if (winX > drawXmax) {
        drawXmax = winX;
    }
    if (winX < drawXmin) {
        drawXmin = winX;
    }
    if (winY > drawYmax) {
        drawYmax = winY;
    }
    if (winY < drawYmin) {
        drawYmin = winY;
    }
    //cout << pos3D_x << " " << pos3D_y << " " << pos3D_z << " " << winX << " " << winY << " " << winZ << endl;
}

 void draw2DBackground() {
    glColor3f(1.0f, 1.0f, 1.0f);
    float wImg = textureImage[textureIndex-1].Width / 58.0;
    float hImg = textureImage[textureIndex-1].Height / 58.0;
    GLfloat vert1[3] = { -wImg, -hImg, -19.0 };
    GLfloat vert2[3] = { -wImg, hImg, -19.0 };
    GLfloat vert3[3] = { wImg, hImg, -19.0 };
    GLfloat vert4[3] = { wImg, -hImg, -19.0 };
    calcBackground(vert1);
    calcBackground(vert2);
    calcBackground(vert3);
    calcBackground(vert4);
}

void setFaceVertex(int index) {
    GLfloat vert[3] = { textureModel->Faces_Triangles[index * 3], textureModel->Faces_Triangles[index * 3 + 1], textureModel->Faces_Triangles[index * 3 + 2] };
    glVertex3fv(vert);
    //glNormal3f(textureModel->Normals[index * 3], textureModel->Normals[index * 3 + 1], textureModel->Normals[index * 3 + 2]);
}

void draw2DElement(int index) {
    glColor3f(1.0f, 1.0f, 1.0f);
    glBegin(GL_POLYGON);
        if (isFrontFace(index)) {
            setFaceVertex(index * 3);
            setFaceVertex(index * 3 + 1);
            setFaceVertex(index * 3 + 2);
        } else {
            setFaceVertex(index * 3);
            setFaceVertex(index * 3 + 2);
            setFaceVertex(index * 3 + 1);
        }
    glEnd();
    /*if (textureWire) {
        glColor3f(0.5f, 0.5f, 0.5f);
        glBegin(GL_LINE_LOOP);
            setFaceVertex(index * 3);
            setFaceVertex(index * 3 + 1);
            setFaceVertex(index * 3 + 2);
        glEnd();
    }*/
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


void draw2DPlayerFull() {

    drawXmin = std::numeric_limits<float>::max();
    drawXmax = std::numeric_limits<float>::min();
    drawYmin = std::numeric_limits<float>::max();
    drawYmax = std::numeric_limits<float>::min();
    glPushMatrix();
    glLoadIdentity();
    draw2DBackground();
    glPopMatrix();

    //cout << "*" << drawXmin << " " << drawXmax << " " << drawYmin << " " << drawYmax << endl;

    f_OcclusionCulling occlusionCulling = (f_OcclusionCulling)GetProcAddress(occlusionLibrary, "OcclusionCulling");

    occlusionCulling(textureIndex,textureModel->TotalFaces, textureModel->Faces_Triangles, &faces, drawXmin, drawXmax, drawYmin, drawYmax);

//    ExtractFrustum();
//    GLuint* queries = new GLuint[textureModel->TotalFaces];
//    //GLuint queries = *queriesP;
//    GLuint sampleCount;
//    glGenQueriesARB(textureModel->TotalFaces, queries);
//    glDisable(GL_BLEND);
//    glDepthFunc(GL_LESS);
//    glDepthMask(GL_TRUE);
//    for (int i = 0; i < textureModel->TotalFaces; i++) {
//        int index = i * 3;
//        if (PointInFrustum(textureModel->Faces_Triangles[index * 3], textureModel->Faces_Triangles[index * 3 + 1], textureModel->Faces_Triangles[index * 3 + 2])) {
//            glBeginQueryARB(GL_SAMPLES_PASSED_ARB, queries[i]);
//            draw2DElement(i);
//            glEndQueryARB(GL_SAMPLES_PASSED_ARB);
//        }
//    }
//    glEnable(GL_BLEND);
//    glDepthFunc(GL_EQUAL);
//    glDepthMask(GL_FALSE);
//
//    for (int i = 0; i < textureModel->TotalFaces; i++) {
//        int index = i * 3;
//        if (PointInFrustum(textureModel->Faces_Triangles[index * 3], textureModel->Faces_Triangles[index * 3 + 1], textureModel->Faces_Triangles[index * 3 + 2])) {
//            glGetQueryObjectuivARB(queries[i], GL_QUERY_RESULT_ARB, &sampleCount);
//            if (sampleCount > 0) {
//                if (textureIndex > 0) {
//                    faces[textureIndex][i] = sampleCount;
//                }
//                //glEnable(GL_CULL_FACE);
//                draw2DElement(i);
//            }
//        }
//    }
//    glDisable(GL_BLEND);
//    glDepthFunc(GL_LESS);
//    glDepthMask(GL_TRUE);
//    delete [] queries;
}

void draw2DPlayerFast() {
    for (int i = 0; i < textureModel->TotalFaces; i++) {
        int hits = 0;
        for (int k = 1; k <= textureCount; k++) {
            hits = max(hits, faces[k][i]);
        }
        /*if (hits > 0 && faces[textureIndex][i] == hits) {
            glEnable(GL_CULL_FACE);
            glFrontFace(GL_CW);
            glCullFace(GL_FRONT);
            draw2DElement(i);
        }*/
        glEnable(GL_CULL_FACE);
        glFrontFace(GL_CW);
        glCullFace(GL_FRONT);
        draw2DElement(i);
    }
}

void applyTransformations(vector<MasterTransform*> history) {
	for (int i = 0; i < history.size(); i++) {
        MasterTransform* trans = history[i];
        if (trans->type == 0) { glTranslatef(trans->value, 0, 0); }
        if (trans->type == 1) { glTranslatef(0, trans->value, 0); }
        if (trans->type == 2) { glTranslatef(0, 0, trans->value); }
        if (trans->type == 3) { glRotatef(trans->value, 1.0f,0.0f,0.0f); }
        if (trans->type == 4) { glRotatef(trans->value, 0.0f,1.0f,0.0f); }
        if (trans->type == 5) { glRotatef(trans->value, 0.0f,0.0f,1.0f); }
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
        glScalef(1.0f,(1.f*wImg)/hImg,3.0f);
    } else {
        glScalef((1.f*hImg)/wImg,1.0f,3.0f);
    }
    glFrustum(-0.035,0.035,-0.035,0.035,0.04,2.0);
    glMultMatrixf(inverseMV.getMatrix());
    glMatrixMode(GL_MODELVIEW);
}


void stepTransformTexture() {
    glTranslatef(0, 0, -20);
    applyTransformations(textureMaster[0].history);
    glTranslatef(0, 0, 20);

    glTranslatef(0, 0, -20);
    //GLdouble m[16];
    //MasterSettings::CalculateMatrix(textureMaster[textureIndex].history, m);
    glMultMatrixd(textureMaster[textureIndex].matrixB);
    //applyTransformations(textureMaster[textureIndex].history, false);
    glTranslatef(0, 0, 20);
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

void display(void) {
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

    for (int i = 1; i <= textureCount; i++) {
        textureIndex = i;
        stepTexture();
        glLoadIdentity();
        glTranslatef(0, 0, -20);
        if (drawFast) {
            applyTransformations(textureMaster[0].history);
            draw2DPlayerFast();
        } else {

            glMultMatrixd(textureMaster[textureIndex].matrixA);
            draw2DPlayerFull();
        }
        stepClearTexture();
    }
    textureIndex = 0;
    if (drawFast) {
        glFlush();
        glutSwapBuffers();
    }

    drawAllText();

    if (!drawFast) {
       drawFast = true;
       display();
   }
}


void CalculateRotateMatrix(vector<MasterTransform*> history, GLdouble* m) {
    glPushMatrix();
    glLoadIdentity();
    for (int i = 0; i < history.size(); i++) {
        MasterTransform* trans = history[i];
        if (trans->type == 3) { glRotatef(trans->value, 1.0f,0.0f,0.0f); }
        if (trans->type == 4) { glRotatef(trans->value, 0.0f,1.0f,0.0f); }
        if (trans->type == 5) { glRotatef(trans->value, 0.0f,0.0f,1.0f); }
    }
    glGetDoublev(GL_MODELVIEW_MATRIX, m);
    glPopMatrix();
}

void CalculateTranslationX(vector<MasterTransform*> history, float v, float &rx, float &ry, float &rz) {
    GLdouble m[16];
    CalculateRotateMatrix(history, m);
    float rw = 1;//m[12] * v + m[15]
    rx = (m[0] * v + m[3]) / rw;
    ry = (m[4] * v + m[7]) / rw;
    rz = (m[8] * v + m[11]) /rw;
}

void CalculateTranslationY(vector<MasterTransform*> history, float v, float &rx, float &ry, float &rz) {
    GLdouble m[16];
    CalculateRotateMatrix(history, m);
    float rw = 1;//m[13] * v + m[15]
    rx = (m[1] * v + m[3]) / rw;
    ry = (m[5] * v + m[7]) / rw;
    rz = (m[9] * v + m[11]) /rw;
}

void CalculateTranslationZ(vector<MasterTransform*> history, float v, float &rx, float &ry, float &rz) {
    GLdouble m[16];
    CalculateRotateMatrix(history, m);
    float rw = 1;//m[114] * v + m[15]
    rx = (m[2] * v + m[3]) / rw;
    ry = (m[6] * v + m[7]) / rw;
    rz = (m[10] * v + m[11]) / rw;
}

void UpdateHistory (int id) {
    float valueX = 0;
    float valueY = 0;
    float valueZ = 0;
    float valueA = 0;
    float valueB = 0;
    float valueC = 0;

    if (textureMaster[id].viewer[0] != 0) {
        CalculateTranslationX(textureMaster[id].history, textureMaster[id].viewer[0], valueX, valueY, valueZ);

    } else if (textureMaster[id].viewer[1] != 0) {
        CalculateTranslationY(textureMaster[id].history, textureMaster[id].viewer[1], valueX, valueY, valueZ);

    } else if (textureMaster[id].viewer[2] != 0) {
        CalculateTranslationZ(textureMaster[id].history, textureMaster[id].viewer[2], valueX, valueY, valueZ);

    }
    if (textureMaster[id].rotate[0] != 0) {
        CalculateTranslationX(textureMaster[id].history, textureMaster[id].rotate[0], valueA, valueB, valueC);

    } else if (textureMaster[id].rotate[1] != 0) {
        CalculateTranslationY(textureMaster[id].history, textureMaster[id].rotate[1], valueA, valueB, valueC);

    } else if (textureMaster[id].rotate[2] != 0) {
        CalculateTranslationZ(textureMaster[id].history, textureMaster[id].rotate[2], valueA, valueB, valueC);

    }

    textureMaster[id].viewer[0] = 0;
    textureMaster[id].viewer[1] = 0;
    textureMaster[id].viewer[2] = 0;
    textureMaster[id].rotate[0] = 0;
    textureMaster[id].rotate[1] = 0;
    textureMaster[id].rotate[2] = 0;

    float values[6] = { valueX, valueY, valueZ, valueA, valueB, valueC };

    for (int i = 0; i < 6; i++) {
        if (values[i] != 0) {
            MasterTransform* trans = NULL;
            if (textureMaster[id].history.size() == 0 || textureMaster[id].history.back()->type != i) {
                trans = new MasterTransform();
                trans->value = values[i];
                trans->type = i;
                textureMaster[id].history.push_back(trans);
            } else {
                trans = textureMaster[id].history.back();
                trans->value += values[i];
            }
        }
    }
    /*int type = 0;
    float value = 0;
    if (textureMaster[id].viewer[0] != 0) {
        type = 0;
        value = textureMaster[id].viewer[0];
    }
    if (textureMaster[id].viewer[1] != 0) {
        type = 1;
        value = textureMaster[id].viewer[1];
    }
    if (textureMaster[id].viewer[2] != 0) {
        type = 2;
        value = textureMaster[id].viewer[2];
    }
    if (textureMaster[id].rotate[0] != 0) {
        type = 3;
        value = textureMaster[id].rotate[0];
    }
    if (textureMaster[id].rotate[1] != 0) {
        type = 4;
        value = textureMaster[id].rotate[1];
    }
    if (textureMaster[id].rotate[2] != 0) {
        type = 5;
        value = textureMaster[id].rotate[2];
    }

    textureMaster[id].viewer[0] = 0;
    textureMaster[id].viewer[1] = 0;
    textureMaster[id].viewer[2] = 0;
    textureMaster[id].rotate[0] = 0;
    textureMaster[id].rotate[1] = 0;
    textureMaster[id].rotate[2] = 0;

    MasterTransform* trans = NULL;
    if (textureMaster[id].history.size() == 0 || textureMaster[id].history.back()->type != type) {
        trans = new MasterTransform();
        trans->value = value;
        trans->type = type;
        textureMaster[id].history.push_back(trans);
    } else {
        trans = textureMaster[id].history.back();
        trans->value += value;
    }*/
}

void keys(unsigned char key, int x, int y) {

    if (key == 'p') {
        textureWire = !textureWire;
    }
    if (key == 'w') textureMaster[0].rotate[0] += 2.0;
    if (key == 's') textureMaster[0].rotate[0] -= 2.0;
    if (key == 'a') textureMaster[0].rotate[1] += 2.0;
    if (key == 'd') textureMaster[0].rotate[1] -= 2.0;
    if (key == 'e') textureMaster[0].rotate[2] += 2.0;
    if (key == 'q') textureMaster[0].rotate[2] -= 2.0;

    if(key == 'm') textureMaster[0].viewer[0] += 0.2;
    if(key == 'b') textureMaster[0].viewer[0] -= 0.2;
    if(key == 'h') textureMaster[0].viewer[1] += 0.2;
    if(key == 'n') textureMaster[0].viewer[1] -= 0.2;
    if(key == 'j') textureMaster[0].viewer[2] += 0.2;
    if(key == 'g') textureMaster[0].viewer[2] -= 0.2;

    if (key == 'w' || key == 's' || key == 'a' || key == 'd' || key == 'e' || key == 'q' ||
        key == 'm' || key == 'b' || key == 'h' || key == 'n' || key == 'j' || key == 'g') {
        UpdateHistory(textureIndex);
    }

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
        float deltaMove = (y - cameraMove) * 0.1f;
        cameraMove = y;
        if (cameraAxis == GLUT_LEFT_BUTTON) {
            textureMaster[0].viewer[0] += deltaMove;
        } else if (cameraAxis == GLUT_RIGHT_BUTTON) {
            textureMaster[0].viewer[1] += deltaMove;
        } else if (cameraAxis == GLUT_MIDDLE_BUTTON) {
            textureMaster[0].viewer[2] += deltaMove;
        }
        if (cameraAxis == GLUT_LEFT_BUTTON || cameraAxis == GLUT_RIGHT_BUTTON || cameraAxis == GLUT_MIDDLE_BUTTON) {
            UpdateHistory(textureIndex);
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

bool loadLightMapTexture(Model_IMG* model) {

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

    bool shouldRedraw = true;
    shouldRedraw = model->MemoryLoad();

    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, 0x812D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, 0x812D);
    glTexParameterfv(GL_TEXTURE_2D,GL_TEXTURE_BORDER_COLOR,borderColor);
    //glTexImage2D(GL_TEXTURE_2D,GL_RGB,model->Width,model->Height,GL_BGR,GL_UNSIGNED_BYTE,model->Pixels);
    gluBuild2DMipmaps(GL_TEXTURE_2D,GL_RGB,model->Width,model->Height,GL_BGR,GL_UNSIGNED_BYTE,model->Pixels);

    return shouldRedraw;
}

void timerFunction(int arg) {
    glutTimerFunc(reDrawRate, timerFunction, 0);
    bool shouldRedraw = true;
    shouldRedraw = textureModel->MemoryLoad();

    //textureModel->Load("mallaUnida.ply");
    for (int i = 0; !shouldRedraw && i < textureCount; i++) {
        shouldRedraw = shouldRedraw || textureImage[i].MemoryCheck();
    }
    if (shouldRedraw) {

        glPushMatrix();
        glLoadIdentity();
        for (int i = 0; i < textureCount; i++) {
            glActiveTextureARB(GL_TEXTURE0 + i);
            glBindTexture(GL_TEXTURE_2D, textures[i]);
            glDisable(GL_TEXTURE_2D);
            textureIndex = i + 1;
            loadLightMapTexture(&textureImage[i]);
        }
        textureIndex = 0;
        glPopMatrix();

        drawFast = false;
        display();
    }
}

int main(int argc, char **argv) {

    char* dllName = "OcclusionDLL.dll";
    occlusionLibrary =  LoadLibraryA(dllName);
    if (!occlusionLibrary) {
        std::cout << "Failed to load the library" << std::endl;
    }

    drawXmin = std::numeric_limits<float>::max();
    drawXmax = std::numeric_limits<float>::min();
    drawYmin = std::numeric_limits<float>::max();
    drawYmax = std::numeric_limits<float>::min();

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE|GLUT_RGB|GLUT_DEPTH);
    glutInitWindowSize(500,500);
    glutInitWindowPosition(300, 300);
    glutCreateWindow("Player project");
    glutTimerFunc(reDrawRate,timerFunction,0);
    glutReshapeFunc(myReshape);
    glutDisplayFunc(display);
    glutMouseFunc(mouse);
    glutMotionFunc(mouseMove);
    glutKeyboardFunc(keys);
    glEnable(GL_DEPTH_TEST);

    /* Settings */
    textureSetting = new Model_SET();
    textureSetting->FileLoad();
    textureCount = textureSetting->NValues;

    textureModel = new Model_PLY();
    textureModel->AlfaCoord = textureSetting->alfaCoord;
    /* Mesh */
    textureModel->MemoryLoad();
    //textureModel->Load("mallaUnida.ply");

    /* Texture */
    textureMaster = new MasterTexture[textureCount + 1];
    faces = new int*[textureCount + 1];
    for (int i = 1; i <= textureCount; i++) {
        for (int j = 0; j < 3; j++) {
            textureMaster[0].viewer[j] = 0.0f;
            textureMaster[0].rotate[j] = 0.0f;
        }
        for (int p = 0; p < 16; p++) {
           textureMaster[i].matrixA[p] = textureSetting->ValuesA[i * 16 + p];
           textureMaster[i].matrixB[p] = textureSetting->ValuesB[i * 16 + p];
        }
        faces[i] = new int[facesCount];
        for (int k = 0; k < facesCount; k++) {
            faces[i][k] = 0;
        }
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
        textureImage[i].Id = textureSetting->IdsValues[i+1] * 10000;
        loadLightMapTexture(&textureImage[i]);
    }
    textureIndex = 0;

    /* Start windows */
    glClearColor( 0.0f, 0.0f, 0.0f, 1.0f );
    glutMainLoop();
}
