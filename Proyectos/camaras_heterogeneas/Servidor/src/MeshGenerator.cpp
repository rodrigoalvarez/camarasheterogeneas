#include "MeshGenerator.h"

#include <iostream>
#include <fstream>
#include "masterPly.h"
#include <string>
#include <ctime>

using namespace std;
int i = 0;

int debugIterFrame = 0;

void MeshGenerator::threadedFunction() {
    if(buffer == NULL) return;

    while(isThreadRunning()) {
        ofSleepMillis(1000/sys_data->fps);
        processFrame();
        debugIterFrame++;
    }
}


void MeshGenerator::debug(ThreadData * td, int type) {
    if(!ServerGlobalData::debug) return;
    string path = "frames/" + ofToString(debugIterFrame);
    ofDirectory::createDirectory(path, true, true);
    if(type == 2) { //Nube de puntos
        char nombre2[50];
        string framePath = "data/" + path + "/frame_" + ofToString(td->cliId) + "_" + ofToString(td->camId) + ".xyz";

        FILE * pFile;
        pFile = fopen (framePath.c_str(),"w");
        for(int i=0; i < td->nubeLength; i ++) {
            fprintf (pFile, "%f %f %f\n", td->xpix[i], td->ypix[i], td->zpix[i]);
        }
        fclose (pFile);
    } else if(type == 1) { //Imágen RGB
        string framePath = "data/" + path + "/img_" + ofToString(td->cliId) + "_" + ofToString(td->camId) + "_" + ofToString(td->cameraType) + ".jpg";
        td->img.saveImage(framePath);
    }
}

void MeshGenerator::processFrame() {
    std::pair <ThreadData *, ThreadData *> frame = buffer->getNextFrame();

    if(frame.first != NULL) { // En first viene un ThreadData con la nube de puntos.

        time_t now = time(0);
        tm *ltm = localtime(&now);

        ofLogVerbose() << " [MeshGenerator::processFrame] - Obtuve un frame mergeado con nube de puntos largo: " << ((ThreadData *) frame.first)->nubeLength;
        ThreadData* td = ((ThreadData *) frame.first);

        debug(td, 2);

        MasterPly* mply = new MasterPly();

        mply->loadMesh(td->xpix,td->ypix,td->zpix,td->nubeLength);
        ofLogVerbose() << " [MeshGenerator::processFrame] - Nube cargada nubeLength " << td->nubeLength;
        mply->poissonDiskSampling(td->nubeLength/4);
        ofLogVerbose() << " [MeshGenerator::processFrame] - Poisson disk sampling aplicado, cantidad de vertices: " <<  mply->totalVertex();
        mply->calculateNormalsVertex();

        now = time(0);
        ltm = localtime(&now);
        ofLogVerbose() << " [MeshGenerator::processFrame] - Calculadas normales de los vertices " << 1 + ltm->tm_min << ":" << 1 + ltm->tm_sec;
        mply->buildMeshBallPivoting();

        now = time(0);
        ltm = localtime(&now);
        ofLogVerbose() << " [MeshGenerator::processFrame] - Ball Pivoting terminado " << 1 + ltm->tm_min << ":" << 1 + ltm->tm_sec;

        if(sys_data->persistToPly == 1) {
            char nombre[50];
            sprintf(nombre, "frame%d.ply", i);
            mply->savePLY(nombre,true);
        }

        now = time(0);
        ltm = localtime(&now);
        ofLogVerbose() << " [MeshGenerator::processFrame] - Malla guardada " << 1 + ltm->tm_min << ":" << 1 + ltm->tm_sec;
        i++;
    }

    if(frame.second != NULL) { // En first viene un array de ThreadData con las texturas.
        int i=1;
        ThreadData * iter = (ThreadData *) frame.second;
        do {
            cout << "[MeshGenerator::processFrame] cli: " << iter->cliId << ", camId: " << iter->camId << ", cameraType: " << iter->cameraType << endl;
            debug(iter, 1);
            //
            //iter->img.saveImage("enIter" + ofToString(i) + ".jpg");
            iter = iter->sig;
            i++;
        } while(iter != NULL);
    }
}
