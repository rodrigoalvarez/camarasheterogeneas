#include "modelPly.h"
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

#include "matrix4x4.h"

using namespace std;

GLfloat colors[][3] = { {1.0,1.0,1.0},
                        {1.0,0.0,0.0},
                        {0.0,1.0,0.0},
                        {0.0,0.0,1.0},
                        {1.0,1.0,0.0},
                        {1.0,0.0,1.0},
                        {0.0,1.0,1.0} };

MasterSettings* settings = NULL;

/* Cloud */

Model_XYZ** cloudModel = NULL;
MasterMesh* cloudMaster = NULL;

int meshCount = 3;
int meshIndex = 0;

/* Camera */

int cameraAxis = -1;
int cameraMove = -1;
bool cameraAll = false;


void writeText() {
    system("cls");
    cout << "3D CALIBRATION" << endl;
    cout << "Mode: " << (meshIndex == 0 ? "View" : "Calibration") << endl << endl;
    for (int i = 0; i <= meshCount; i++) {
        MasterMesh* masterNow = &cloudMaster[i];
        cout << "Mesh :: " << i << endl;
        cout << "Points... " << cloudModel[i]->TotalPoints << endl;
        cout << "Origin position..." << endl << masterNow->viewer[0]  << " " << masterNow->viewer[1]  << " " << masterNow->viewer[2] << endl;
        cout << "Object rotate..." << endl << masterNow->rotate[0]  << " " << masterNow->rotate[1]  << " " << masterNow->rotate[2] << endl;
        cout << endl;
    }
}

void setPointVertex(int index) {
    GLfloat vert[3] = { cloudModel[meshIndex]->Points[index * 3], cloudModel[meshIndex]->Points[index * 3 + 1], cloudModel[meshIndex]->Points[index * 3 + 2] };
    glVertex3fv(vert);
}

void draw3D() {
    glPointSize(0.2);
    glColor3fv(colors[meshIndex]);
    for (int i = 0; i < cloudModel[meshIndex]->TotalPoints; i += 10) {
        glBegin(GL_POINTS);
            setPointVertex(i);
        glEnd();
    }
}

void IncludeMesh (Model_XYZ* model, Model_XYZ* newModel, MasterMesh master) {
    GLdouble m[16];
    MasterSettings::CalculateMatrix(master, m);
    model->Include(newModel, m);
}

void display(void) {
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

    glLoadIdentity();
    glTranslatef(cloudMaster[0].viewer[0], cloudMaster[0].viewer[1], cloudMaster[0].viewer[2] - 10);
    glRotatef(cloudMaster[0].rotate[0], -1.0f,0.0f,0.0f);
    glRotatef(cloudMaster[0].rotate[1], 0.0f,-1.0f,0.0f);
    glRotatef(cloudMaster[0].rotate[2], 0.0f,0.0f,-1.0f);

    if (meshIndex != 0) {
        glTranslatef(cloudMaster[meshIndex].viewer[0], cloudMaster[meshIndex].viewer[1], cloudMaster[meshIndex].viewer[2]);
        glRotatef(cloudMaster[meshIndex].rotate[0], -1.0f,0.0f,0.0f);
        glRotatef(cloudMaster[meshIndex].rotate[1], 0.0f,-1.0f,0.0f);
        glRotatef(cloudMaster[meshIndex].rotate[2], 0.0f,0.0f,-1.0f);
    }

    draw3D();

    if (meshIndex != 0) {
        int meshIndexOld = meshIndex;
        meshIndex = 0;

        glLoadIdentity();
        glTranslatef(cloudMaster[0].viewer[0], cloudMaster[0].viewer[1], cloudMaster[0].viewer[2] - 10);
        glRotatef(cloudMaster[0].rotate[0], -1.0f,0.0f,0.0f);
        glRotatef(cloudMaster[0].rotate[1], 0.0f,-1.0f,0.0f);
        glRotatef(cloudMaster[0].rotate[2], 0.0f,0.0f,-1.0f);
        draw3D();

        meshIndex = meshIndexOld;
    }

    glFlush();
    glutSwapBuffers();

    writeText();
}

void keys(unsigned char key, int x, int y) {

    if (key == 'l') {
        settings->loadMeshCalibration();
    }
    if (key == 'k') {
        settings->saveMeshCalibration();
    }
    if(key == 'v') {
        meshIndex = 0;
        cloudModel[0]->Clear();
        for (int i = 1; i <= meshCount; i++) {
            IncludeMesh(cloudModel[0], cloudModel[i], cloudMaster[i]);
        }
    }
    if(key >= '1' && key <= '9' && (key - 48 <= meshCount)) {
        meshIndex = key - 48;
        cloudModel[0]->Clear();
        for (int i = 1; i <= meshCount; i++) {
            if (i != meshIndex) {
                IncludeMesh(cloudModel[0], cloudModel[i], cloudMaster[i]);
            }
        }
    }

    if(key == 'w') cloudMaster[meshIndex].rotate[0] += 2.0;
    if(key == 's') cloudMaster[meshIndex].rotate[0] -= 2.0;
    if(key == 'a') cloudMaster[meshIndex].rotate[1] += 2.0;
    if(key == 'd') cloudMaster[meshIndex].rotate[1] -= 2.0;
    if(key == 'e') cloudMaster[meshIndex].rotate[2] += 2.0;
    if(key == 'q') cloudMaster[meshIndex].rotate[2] -= 2.0;

    if(key == 'W') cloudMaster[0].rotate[0] += 2.0;
    if(key == 'S') cloudMaster[0].rotate[0] -= 2.0;
    if(key == 'A') cloudMaster[0].rotate[1] += 2.0;
    if(key == 'D') cloudMaster[0].rotate[1] -= 2.0;
    if(key == 'E') cloudMaster[0].rotate[2] += 2.0;
    if(key == 'Q') cloudMaster[0].rotate[2] -= 2.0;

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
        if (cameraAll) {
            if (cameraAxis == GLUT_LEFT_BUTTON) {
                cloudMaster[0].viewer[0] += deltaMove;
            } else if (cameraAxis == GLUT_RIGHT_BUTTON) {
                cloudMaster[0].viewer[1] += deltaMove;
            } else if (cameraAxis == GLUT_MIDDLE_BUTTON) {
                cloudMaster[0].viewer[2] += deltaMove;
            }
        } else {
            if (cameraAxis == GLUT_LEFT_BUTTON) {
                cloudMaster[meshIndex].viewer[0] += deltaMove;
            } else if (cameraAxis == GLUT_RIGHT_BUTTON) {
                cloudMaster[meshIndex].viewer[1] += deltaMove;
            } else if (cameraAxis == GLUT_MIDDLE_BUTTON) {
                cloudMaster[meshIndex].viewer[2] += deltaMove;
            }
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

vector<string> getCloudFiles() {
    vector<string> files;
    ofDirectory directory("C:\\of_v0073_win_cb_release\\apps\\myApps\\reproductorUnido\\mesh");
    directory.allowExt("xyz");
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
    glEnable(GL_DEPTH_TEST);

    /* Cloud */
    vector<string> files3D = getCloudFiles();
    meshCount = files3D.size();
    cloudMaster = new MasterMesh[meshCount + 1];
    cloudModel = new Model_XYZ*[meshCount];
    for (int i = 0; i <= meshCount; i++) {
        for (int j = 0; j < 3; j++) {
            cloudMaster[i].viewer[j] = 0.0;
            cloudMaster[i].rotate[j] = 0.0;
        }
        cloudModel[i] = new Model_XYZ();
    }

    /* Settings and files */
    settings = new MasterSettings(0, NULL, meshCount, cloudMaster);

    /* LoadClouds */
    for (int i = 0; i < meshCount; i++) {
        if (i == 0) {
            cloudModel[i+1]->Load(files3D[i].c_str(), 0);
        } else {
            cloudModel[i+1]->Load(files3D[i].c_str(), cloudModel[1]->AlfaCoord);
        }
    }
    cloudModel[0]->Clear();
    for (int i = 1; i <= meshCount; i++) {
        IncludeMesh(cloudModel[0], cloudModel[i], cloudMaster[i]);
    }
    meshIndex = 0;

    /* Start windows */
    writeText();
    glClearColor( 0.0f, 0.0f, 0.0f, 1.0f );
    glutMainLoop();
}
