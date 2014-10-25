#include "modelPly.h"
#include "ofxXmlSettings.h"
#include <stdlib.h>
#include <windows.h>
#include <GL/glut.h>
#include "modelXYZ.h"
#include "masterPly.h"
#include "masterSettings.h"
#include "GlobalData.h"
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

vector<MasterClient*> clients;
int doubleClickTime = 500;
int clickCount = 0;
float cameraFactor = 1.0;
int cameraAxis = -1;
int cameraMove = -1;
bool cameraAll = false;
bool cameraLight = true;


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
    glNormal3fv(vert);
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

    if (cameraLight) {
        glEnable(GL_LIGHTING);
        glEnable(GL_LIGHT0);

        glPushMatrix();
        glLoadIdentity();
        GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 0.5 };
        GLfloat mat_shininess[] = { 50.0 };
        GLfloat light_color[] = { 1., 1., 1., 0.5 };
        GLfloat light_position[] = { 0.0, 0.0, 1.0, 0.0 };

        glShadeModel (GL_SMOOTH);
        glMaterialfv(GL_FRONT, GL_SPECULAR, colors[meshIndex]);
        glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
        glLightfv(GL_LIGHT0, GL_DIFFUSE, colors[meshIndex]);
        glLightfv(GL_LIGHT0, GL_POSITION, light_position);
        glPopMatrix();
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

        if (cameraLight) {
            glPushMatrix();
            glLoadIdentity();
            GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 0.5 };
            GLfloat mat_shininess[] = { 50.0 };
            GLfloat light_color[] = { 1., 1., 1., 0.5 };
            GLfloat light_position[] = { 0.0, 0.0, 1.0, 0.0 };

            glShadeModel (GL_SMOOTH);
            glMaterialfv(GL_FRONT, GL_SPECULAR, colors[meshIndex]);
            glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
            glLightfv(GL_LIGHT0, GL_DIFFUSE, colors[meshIndex]);
            glLightfv(GL_LIGHT0, GL_POSITION, light_position);
            glPopMatrix();/**/
        }

        draw3D();

        meshIndex = meshIndexOld;
    }

    if (cameraLight) {
        glDisable(GL_LIGHT0);
        glDisable(GL_LIGHTING);
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
    if (key == 'n') {
        cameraLight = !cameraLight;
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
    if(key == '+') cameraFactor *= 1.25;
    if(key == '-') cameraFactor *= 0.8;

    if (cameraAll) {
        if(key == 'W' || key == 'w') cloudMaster[0].rotate[0] += 2.0 * cameraFactor;
        if(key == 'S' || key == 's') cloudMaster[0].rotate[0] -= 2.0 * cameraFactor;
        if(key == 'A' || key == 'a') cloudMaster[0].rotate[1] += 2.0 * cameraFactor;
        if(key == 'D' || key == 'd') cloudMaster[0].rotate[1] -= 2.0 * cameraFactor;
        if(key == 'E' || key == 'e') cloudMaster[0].rotate[2] += 2.0 * cameraFactor;
        if(key == 'Q' || key == 'q') cloudMaster[0].rotate[2] -= 2.0 * cameraFactor;
    } else {
        if(key == 'w') cloudMaster[meshIndex].rotate[0] += 2.0 * cameraFactor;
        if(key == 's') cloudMaster[meshIndex].rotate[0] -= 2.0 * cameraFactor;
        if(key == 'a') cloudMaster[meshIndex].rotate[1] += 2.0 * cameraFactor;
        if(key == 'd') cloudMaster[meshIndex].rotate[1] -= 2.0 * cameraFactor;
        if(key == 'e') cloudMaster[meshIndex].rotate[2] += 2.0 * cameraFactor;
        if(key == 'q') cloudMaster[meshIndex].rotate[2] -= 2.0 * cameraFactor;

        if(key == 'W') cloudMaster[0].rotate[0] += 2.0 * cameraFactor;
        if(key == 'S') cloudMaster[0].rotate[0] -= 2.0 * cameraFactor;
        if(key == 'A') cloudMaster[0].rotate[1] += 2.0 * cameraFactor;
        if(key == 'D') cloudMaster[0].rotate[1] -= 2.0 * cameraFactor;
        if(key == 'E') cloudMaster[0].rotate[2] += 2.0 * cameraFactor;
        if(key == 'Q') cloudMaster[0].rotate[2] -= 2.0 * cameraFactor;
    }

    display();
}

void mouse(int btn, int state, int x, int y) {
    cameraAxis = state == GLUT_DOWN ? btn : -1;
    if (state == GLUT_DOWN) {
        cameraMove = y;
        clickCount++;
    }
    if (state == GLUT_UP) {
        cameraMove = -1;
    }
    display();
}

void mouseMove(int x, int y) {
    if (cameraAxis != -1) {
        float deltaMove = (y - cameraMove) * 0.1f * cameraFactor;;
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

bool REAL_TIME = true;
int REAL_TIME_FPS = 10;
int REAL_TIME_PORT = 3232;

void saveXmlClient(ofxXmlSettings* pSettings, MasterClient* client) {

    ofxXmlSettings settings = *pSettings;
    settings.setValue("realTime", REAL_TIME);
    settings.setValue("realTimeFPS", REAL_TIME_FPS);
    settings.setValue("realTimePort", REAL_TIME_PORT);

    settings.addTag("cameras");
    settings.pushTag("cameras");

    for(int i = 0; i < client->cameras.size(); i++) {
        MasterCamera* camera = client->cameras[i];
        settings.addTag("camera");
        settings.pushTag("camera", i);

        settings.addValue("id", camera->idCamera);
        settings.addValue("resolutionX", 800);//800
        settings.addValue("resolutionY", 600);//600
        settings.addValue("resolutionDownSample", 1);
        settings.addValue("FPS", 30);// camera i fps
        settings.addValue("colorRGB", true);// camera i color rgb
        settings.addValue("use2D", camera->is2D);// camera i use2d
        settings.addValue("use3D", camera->is3D);// camera i use 3d
        settings.addValue("dataContext", "");// data context

        settings.addTag("depthSettings");
        settings.pushTag("depthSettings");
        settings.addValue("near", 10);
        settings.addValue("far", 100);
        settings.addValue("pointsDownSample", 1);
        settings.popTag();

        if (camera->is2D) {
            settings.addTag("matrix2D");
            settings.pushTag("matrix2D");
            MasterTexture* master = camera->masterTexture;
            GLdouble m[16];
            MasterSettings::CalculateMatrix(master->history, m);
            for(int j = 0; j < 16; j++) {
                std::stringstream cellM;
                cellM << "m" << j / 4 << j % 4;
                settings.addValue(cellM.str(), (float)m[j]);
            }
            settings.popTag();
        }
        if (camera->is3D) {
            settings.addTag("matrix3D");
            settings.pushTag("matrix3D");
            MasterMesh* master = camera->masterMesh;
            GLdouble m[16];
            MasterSettings::CalculateMatrix(*master, m);
            for(int j = 0; j < 16; j++) {
                std::stringstream cellM;
                cellM << "m" << j / 4 << j % 4;
                settings.addValue(cellM.str(), (float)m[j]);
            }
            settings.popTag();
        }

        settings.popTag();
    }

    settings.popTag();
}

void saveXmlFiles(vector<MasterClient*> clients) {

    if (clients.size() > 0) {
        ofxXmlSettings settings;
        settings.addTag("settings");
        settings.pushTag("settings");
        for (int k = 0; k < clients.size(); k++) {
            settings.addTag("client");
            settings.pushTag("client", k);
            saveXmlClient(&settings, clients[k]);
            settings.popTag();
        }
        settings.popTag();

        std::stringstream fileName;
        fileName << "settings.xml";
        settings.saveFile(fileName.str());
    }
    for (int k = 0; k < clients.size(); k++) {
        ofxXmlSettings settings;
        settings.addTag("settings");
        settings.pushTag("settings");
        saveXmlClient(&settings, clients[k]);
        settings.popTag();

        std::stringstream fileName;
        fileName << "settings" << k << ".xml";
        settings.saveFile(fileName.str());
    }
}

int nClientes = 0;
int total2D = 0;
int total3D = 0;
t_data* sys_data = NULL;

void loadXmlFile() {
    ofxXmlSettings settings;
    settings.loadFile("settings.xml");

    total2D = 0;
    total3D = 0;
    if (settings.pushTag("settings")) {
        nClientes = settings.getNumTags("client");
        sys_data = new t_data[nClientes];

        for (int k = 0; k < nClientes; k++) {
            if (settings.pushTag("client", k)) {
                sys_data[k].cliId           = settings.getValue("cliId",          0);
                sys_data[k].cliPort         = settings.getValue("cliPort",        15000);
                sys_data[k].serverIp        = settings.getValue("serverIp",       "127.0.0.1");
                sys_data[k].serverPort      = settings.getValue("serverPort",     11969);
                sys_data[k].goLive          = settings.getValue("realTime",       0);
                sys_data[k].persistence     = settings.getValue("persistence",    1);
                sys_data[k].logLevel        = settings.getValue("logLevel",       0);
                sys_data[k].fps             = settings.getValue("fps",            10);
                sys_data[k].maxPackageSize  = settings.getValue("maxPackageSize", 60000);

                if(settings.pushTag("cameras")) {
                    sys_data[k].nCamaras = settings.getNumTags("camera");
                    sys_data[k].camera = new t_camera[sys_data[k].nCamaras];
                    for (int i = 0; i < sys_data[k].nCamaras; i++) {
                        settings.pushTag("camera", i);

                        sys_data[k].camera[i].id                     = settings.getValue("id", 0);
                        sys_data[k].camera[i].resolutionX            = settings.getValue("resolutionX", 640);
                        sys_data[k].camera[i].resolutionY            = settings.getValue("resolutionY", 480);
                        sys_data[k].camera[i].resolutionDownSample   = settings.getValue("resolutionDownSample", 1);
                        sys_data[k].camera[i].fps                    = settings.getValue("FPS", 24);
                        sys_data[k].camera[i].use2D                  = settings.getValue("use2D", true);
                        sys_data[k].camera[i].use3D                  = settings.getValue("use3D", true);

                        if(settings.pushTag("depthSettings")) {
                            sys_data[k].camera[i].near3D                 = settings.getValue("near", 0);
                            sys_data[k].camera[i].far3D                  = settings.getValue("far", 1);
                            sys_data[k].camera[i].points3DDownSample     = settings.getValue("pointsDownSample", 0.5);
                            settings.popTag();
                        }
                        if(settings.pushTag("matrix2D")) {
                            sys_data[k].camera[i].imgrow1.set(settings.getValue("m00", 1.0f), settings.getValue("m01", 1.0f), settings.getValue("m02", 1.0f), settings.getValue("m03", 1.0f));
                            sys_data[k].camera[i].imgrow2.set(settings.getValue("m10", 1.0f), settings.getValue("m11", 1.0f), settings.getValue("m12", 1.0f), settings.getValue("m13", 1.0f));
                            sys_data[k].camera[i].imgrow3.set(settings.getValue("m20", 1.0f), settings.getValue("m21", 1.0f), settings.getValue("m22", 1.0f), settings.getValue("m23", 1.0f));
                            sys_data[k].camera[i].imgrow4.set(settings.getValue("m30", 1.0f), settings.getValue("m31", 1.0f), settings.getValue("m32", 1.0f), settings.getValue("m33", 1.0f));
                            settings.popTag();
                        }
                        if(settings.pushTag("matrix3D")) {
                            sys_data[k].camera[i].row1.set(settings.getValue("m00", 1.0f), settings.getValue("m01", 1.0f), settings.getValue("m02", 1.0f), settings.getValue("m03", 1.0f));
                            sys_data[k].camera[i].row2.set(settings.getValue("m10", 1.0f), settings.getValue("m11", 1.0f), settings.getValue("m12", 1.0f), settings.getValue("m13", 1.0f));
                            sys_data[k].camera[i].row3.set(settings.getValue("m20", 1.0f), settings.getValue("m21", 1.0f), settings.getValue("m22", 1.0f), settings.getValue("m23", 1.0f));
                            sys_data[k].camera[i].row4.set(settings.getValue("m30", 1.0f), settings.getValue("m31", 1.0f), settings.getValue("m32", 1.0f), settings.getValue("m33", 1.0f));
                            settings.popTag();
                        }

                        if (sys_data[k].camera[i].use2D)
                            total2D ++;
                        if (sys_data[k].camera[i].use3D)
                            total3D ++;

                        settings.popTag();
                    }
                    settings.popTag();
                }
                settings.popTag();
            }
        }
    }
}

vector<string> getCloudFiles() {
    vector<string> files;
    ofDirectory directory("C:\\of_v0073_win_cb_release\\apps\\myApps\\reproductorUnido\\bin\\data");

    directory.listDir();
    vector<ofFile> allFiles = directory.getFiles();
    for(int i = 0; i < allFiles.size(); i++){
        if (allFiles[i].isDirectory()){

            ofDirectory clientDirectory(allFiles[i].path());
            clientDirectory.listDir();
            vector<ofFile> allClientFiles = clientDirectory.getFiles();

            for(int j = 0; j < allClientFiles.size(); j++){
                if (allClientFiles[j].isDirectory()){

                    ofDirectory cameraDirectory(allClientFiles[j].path());
                    cameraDirectory.allowExt("xyz");
                    cameraDirectory.listDir();
                    if (cameraDirectory.numFiles() > 0){
                        cout << cameraDirectory.getPath(0) << endl;
                        files.push_back(cameraDirectory.getPath(0));
                    }
                }
            }
        }
    }
    return files;
}

vector<string> getImageFiles() {
    vector<string> files;
    ofDirectory directory("C:\\of_v0073_win_cb_release\\apps\\myApps\\reproductorUnido\\bin\\data");

    directory.listDir();
    vector<ofFile> allFiles = directory.getFiles();
    for(int i = 0; i < allFiles.size(); i++){
        if (allFiles[i].isDirectory()){

            ofDirectory clientDirectory(allFiles[i].path());
            clientDirectory.listDir();
            vector<ofFile> allClientFiles = clientDirectory.getFiles();

            for(int j = 0; j < allClientFiles.size(); j++){
                if (allClientFiles[j].isDirectory()){

                    ofDirectory cameraDirectory(allClientFiles[j].path());
                    cameraDirectory.allowExt("png");
                    cameraDirectory.allowExt("bmp");
                    cameraDirectory.allowExt("jpg");
                    cameraDirectory.listDir();
                    if (cameraDirectory.numFiles() > 0){
                        cout << cameraDirectory.getPath(0) << endl;
                        files.push_back(cameraDirectory.getPath(0));
                    }
                }
            }
        }
    }

    return files;
}

vector<MasterClient*> getClients(int textureCount, MasterTexture* textureMaster, int meshCount, MasterMesh* cloudMaster) {
    vector<MasterClient*> clients;
    int idClient = 0;
    int idCamera = 0;
    ofDirectory directory("C:\\of_v0073_win_cb_release\\apps\\myApps\\reproductorUnido\\bin\\data");

    directory.listDir();
    vector<ofFile> allFiles = directory.getFiles();
    for(int i = 0; i < allFiles.size(); i++){
        if (allFiles[i].isDirectory()){

            MasterClient* client = new MasterClient();

            ofDirectory clientDirectory(allFiles[i].path());
            clientDirectory.listDir();
            vector<ofFile> allClientFiles = clientDirectory.getFiles();

            for(int j = 0; j < allClientFiles.size(); j++){
                if (allClientFiles[j].isDirectory()){

                    MasterCamera* camera = new MasterCamera();

                    ofDirectory cludDirectory(allClientFiles[j].path());
                    cludDirectory.allowExt("xyz");
                    cludDirectory.listDir();
                    camera->is3D = cludDirectory.numFiles() > 0 && idCamera < meshCount;
                    if (camera->is3D) {
                        camera->masterMesh = &cloudMaster[idCamera];
                    }

                    ofDirectory imageDirectory(allClientFiles[j].path());
                    imageDirectory.allowExt("png");
                    imageDirectory.allowExt("bmp");
                    imageDirectory.allowExt("jpg");
                    imageDirectory.listDir();
                    camera->is2D = imageDirectory.numFiles() > 0 && idCamera < textureCount;
                    if (camera->is2D) {
                        camera->masterTexture = &textureMaster[idCamera];
                    }

                    if (camera->is2D || camera->is3D) {
                        camera->idCamera = idCamera++;
                        client->cameras.push_back(camera);
                    }
                }
            }

            if (client->cameras.size() > 0) {
                client->idClient = idClient++;
                clients.push_back(client);
            }
        }
    }
    return clients;
}

void timerFunction(int arg) {
    glutTimerFunc(doubleClickTime, timerFunction, 0);
    if (clickCount > 1) {
        cameraAll = !cameraAll;
    }
    clickCount = 0;
}

int main(int argc, char **argv) {

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE|GLUT_RGB|GLUT_DEPTH);
    glutInitWindowSize(500,500);
    glutInitWindowPosition(300, 300);
    glutCreateWindow("Calibration project");
    glutTimerFunc(doubleClickTime,timerFunction,0);
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
    clients = getClients(0, NULL, meshCount, cloudMaster);
    settings = new MasterSettings(0, NULL, meshCount, cloudMaster);

    /* Test */
    /*for (int i = 0; i < clients.size(); i++) {
        cout << "Client " << clients[i]->idClient << endl;
        for (int j = 0; j < clients[i]->cameras.size(); j++) {
            cout << "Cameras " << clients[i]->cameras[j]->idCamera << " Is2D " << clients[i]->cameras[j]->is2D << " Is3D " << clients[i]->cameras[j]->is3D << endl;
        }
    }*/
    saveXmlFiles(clients);
    loadXmlFile();

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
