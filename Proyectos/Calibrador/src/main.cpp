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

#include "FreeImage.h"
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

/* Mesh */

Model_XYZ** meshModel = NULL;
MasterMesh* meshMaster = NULL;

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
        MasterMesh* masterNow = &meshMaster[i];
        cout << "Mesh :: " << i << endl;
        cout << "Points... " << meshModel[i]->TotalPoints << endl;
        cout << "Origin position..." << endl << masterNow->viewer[0]  << " " << masterNow->viewer[1]  << " " << masterNow->viewer[2] << endl;
        cout << "Object rotate..." << endl << masterNow->rotate[0]  << " " << masterNow->rotate[1]  << " " << masterNow->rotate[2] << endl;
        cout << endl;
    }
}

void setPointVertex(int index) {
    GLfloat vert[3] = { meshModel[meshIndex]->Points[index * 3], meshModel[meshIndex]->Points[index * 3 + 1], meshModel[meshIndex]->Points[index * 3 + 2] };
    glVertex3fv(vert);
}

void draw3D() {
    glPointSize(0.2);
    glColor3fv(colors[meshIndex]);
    for (int i = 0; i < meshModel[meshIndex]->TotalPoints; i += 10) {
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
    glTranslatef(meshMaster[0].viewer[0], meshMaster[0].viewer[1], meshMaster[0].viewer[2] - 10);
    glRotatef(meshMaster[0].rotate[0], -1.0f,0.0f,0.0f);
    glRotatef(meshMaster[0].rotate[1], 0.0f,-1.0f,0.0f);
    glRotatef(meshMaster[0].rotate[2], 0.0f,0.0f,-1.0f);

    if (meshIndex != 0) {
        glTranslatef(meshMaster[meshIndex].viewer[0], meshMaster[meshIndex].viewer[1], meshMaster[meshIndex].viewer[2]);
        glRotatef(meshMaster[meshIndex].rotate[0], -1.0f,0.0f,0.0f);
        glRotatef(meshMaster[meshIndex].rotate[1], 0.0f,-1.0f,0.0f);
        glRotatef(meshMaster[meshIndex].rotate[2], 0.0f,0.0f,-1.0f);
    }

    draw3D();

    if (meshIndex != 0) {
        int meshIndexOld = meshIndex;
        meshIndex = 0;

        glLoadIdentity();
        glTranslatef(meshMaster[0].viewer[0], meshMaster[0].viewer[1], meshMaster[0].viewer[2] - 10);
        glRotatef(meshMaster[0].rotate[0], -1.0f,0.0f,0.0f);
        glRotatef(meshMaster[0].rotate[1], 0.0f,-1.0f,0.0f);
        glRotatef(meshMaster[0].rotate[2], 0.0f,0.0f,-1.0f);
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
        meshModel[0]->Clear();
        for (int i = 1; i <= meshCount; i++) {
            IncludeMesh(meshModel[0], meshModel[i], meshMaster[i]);
        }
    }
    if(key >= '1' && key <= '9' && (key - 48 <= meshCount)) {
        meshIndex = key - 48;
        meshModel[0]->Clear();
        for (int i = 1; i <= meshCount; i++) {
            if (i != meshIndex) {
                IncludeMesh(meshModel[0], meshModel[i], meshMaster[i]);
            }
        }
    }

    if(key == 'w') meshMaster[meshIndex].rotate[0] += 2.0;
    if(key == 's') meshMaster[meshIndex].rotate[0] -= 2.0;
    if(key == 'a') meshMaster[meshIndex].rotate[1] += 2.0;
    if(key == 'd') meshMaster[meshIndex].rotate[1] -= 2.0;
    if(key == 'e') meshMaster[meshIndex].rotate[2] += 2.0;
    if(key == 'q') meshMaster[meshIndex].rotate[2] -= 2.0;

    if(key == 'W') meshMaster[0].rotate[0] += 2.0;
    if(key == 'S') meshMaster[0].rotate[0] -= 2.0;
    if(key == 'A') meshMaster[0].rotate[1] += 2.0;
    if(key == 'D') meshMaster[0].rotate[1] -= 2.0;
    if(key == 'E') meshMaster[0].rotate[2] += 2.0;
    if(key == 'Q') meshMaster[0].rotate[2] -= 2.0;

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
                meshMaster[0].viewer[0] += deltaMove;
            } else if (cameraAxis == GLUT_RIGHT_BUTTON) {
                meshMaster[0].viewer[1] += deltaMove;
            } else if (cameraAxis == GLUT_MIDDLE_BUTTON) {
                meshMaster[0].viewer[2] += deltaMove;
            }
        } else {
            if (cameraAxis == GLUT_LEFT_BUTTON) {
                meshMaster[meshIndex].viewer[0] += deltaMove;
            } else if (cameraAxis == GLUT_RIGHT_BUTTON) {
                meshMaster[meshIndex].viewer[1] += deltaMove;
            } else if (cameraAxis == GLUT_MIDDLE_BUTTON) {
                meshMaster[meshIndex].viewer[2] += deltaMove;
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

int main(int argc, char **argv) {

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
    glutInitWindowPosition(300, 300);
	glutCreateWindow("Calibration project");
	glutReshapeFunc(myReshape);
	glutDisplayFunc(display);
	glutMouseFunc(mouse);
	glutMotionFunc(mouseMove);
	glutKeyboardFunc(keys);
	glEnable(GL_DEPTH_TEST);

    /* Settings and files */
    settings = new MasterSettings(0, NULL, meshCount, meshMaster);

    vector<string> meshFiles;
    meshFiles.push_back("mesh/3DMesh1.xyz");
    meshFiles.push_back("mesh/3DMesh2.xyz");
    meshFiles.push_back("mesh/3DMesh3.xyz");
    for (int i = 0; i < meshCount; i++) {
        if (i == 0) {
            meshModel[i+1]->Load(meshFiles[i].c_str(), 0);
        } else {
            meshModel[i+1]->Load(meshFiles[i].c_str(), meshModel[1]->AlfaCoord);
        }
    }
    meshModel[0]->Clear();
    for (int i = 1; i <= meshCount; i++) {
        IncludeMesh(meshModel[0], meshModel[i], meshMaster[i]);
    }
    meshIndex = 0;

    writeText();
	glClearColor( 0.0f, 0.0f, 0.0f, 1.0f );
	glutMainLoop();
}
