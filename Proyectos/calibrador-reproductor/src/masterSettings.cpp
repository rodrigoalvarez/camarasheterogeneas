#include "masterSettings.h"
#include <stdlib.h>
#include <string>
#include <fstream>
#include <sstream>

using namespace std;


void MasterSettings::loadMeshCalibration () {

    for (int i = 1; i <= meshCount; i++) {
        MasterMesh* masterNow = &meshMaster[i];
        std::stringstream fileName;
        fileName << "mesh" << i << ".txt";
        std::ifstream in(fileName.str().c_str());
        std::stringstream buffer;
        buffer << in.rdbuf();

        string line;
        if (getline(buffer, line, '\n')) {
            istringstream subBuffer(line);
            string value;
            for (int i = 0; i < 16 && getline(subBuffer, value, ' '); i++) {
                masterNow->matrix[i] = ::atof(value.c_str());
            }
        }
        in.close();
    }
}

void MasterSettings::saveMeshCalibration () {

    for (int i = 1; i <= meshCount; i++) {
        MasterMesh* masterNow = &meshMaster[i];
        std::stringstream fileName;
        fileName << "mesh" << i << ".txt";
        std::ofstream out(fileName.str().c_str());
        GLdouble m[16];
        CalculateMatrix(*masterNow, m);
        out << m[0] << " " << m[1] << " " << m[2] << " " << m[3] << " ";
        out << m[4] << " " << m[5] << " " << m[6] << " " << m[7] << " ";
        out << m[8] << " " << m[9] << " " << m[10] << " " << m[11] << " ";
        out << m[12] << " " << m[13] << " " << m[14] << " " << m[15];
        out.close();
    }
}

void MasterSettings::loadTextureCalibration () {

    for (int i = 1; i <= textureCount; i++) {
        MasterTexture* masterNow = &textureMaster[i];
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
}

void MasterSettings::saveTextureCalibration () {

    for (int i = 1; i < textureCount; i++) {
        MasterTexture* masterNow = &textureMaster[i];
        std::stringstream fileName;
        fileName << "texture" << i << ".txt";
        std::ofstream out(fileName.str().c_str());
        out << masterNow->viewer[0] << " " << masterNow->viewer[1] << " " << masterNow->viewer[2] << endl;
        out << masterNow->rotate[0] << " " << masterNow->rotate[1] << " " << masterNow->rotate[2];
        out.close();
    }
}

void MasterSettings::CalculateMatrix(MasterMesh master, GLdouble* m) {
    glPushMatrix();
    glLoadIdentity();
    glTranslatef(master.viewer[0], master.viewer[1], master.viewer[2]);
    glRotatef(master.rotate[2], 0.0f,0.0f,1.0f);
    glRotatef(master.rotate[1], 0.0f,1.0f,0.0f);
    glRotatef(master.rotate[0], 1.0f,0.0f,0.0f);
    glGetDoublev(GL_MODELVIEW_MATRIX, m);
    glPopMatrix();
}

MasterSettings::MasterSettings(int _textureCount, MasterTexture* _textureMaster, int _meshCount, MasterMesh* _meshMaster) {
    textureCount = _textureCount;
    textureMaster = _textureMaster;
    meshCount = _meshCount;
    meshMaster = _meshMaster;
}
