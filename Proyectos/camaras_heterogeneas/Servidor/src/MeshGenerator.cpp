#include "MeshGenerator.h"

#include <iostream>
#include <fstream>
#include "masterPly.h"
#include <string>
#include <ctime>

using namespace std;
int i = 0;
void MeshGenerator::threadedFunction() {
    if(buffer == NULL) return;

    while(isThreadRunning()) {
        ofSleepMillis(1000/sys_data->fps);
        processFrame();
    }
}

void MeshGenerator::processFrame() {
    std::pair <ThreadData *, ThreadData *> frame = buffer->getNextFrame();
    if(frame.first != NULL) { // En first viene un ThreadData con la nube de puntos.

        time_t now = time(0);
        tm *ltm = localtime(&now);

        ofLogVerbose() << " [MeshGenerator::processFrame] - Obtuve un frame mergeado con nube de puntos largo: " << ((ThreadData *) frame.first)->nubeLength;
        ThreadData* td = ((ThreadData *) frame.first);
        MasterPly* mply = new MasterPly();



        char nombre2[50];
        sprintf(nombre2, "frame%d.xyz", i);
        //char* fileName      = "frameeee_i.xyz";

        //Creo el archivo b,n de la nube unida
        FILE * pFile;
        pFile = fopen (nombre2,"w");
        //Recorro los frames de cada camara y me quedo solo con los 3D
        for(int i=0; i < td->nubeLength; i ++) {
            fprintf (pFile, "%f %f %f\n", td->xpix[i], td->ypix[i], td->zpix[i]);
        }

        fclose (pFile);

        /**/

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
        char nombre[50];
        sprintf(nombre, "frame%d.ply", i);
        mply->savePLY(nombre,true);

        now = time(0);
        ltm = localtime(&now);
        ofLogVerbose() << " [MeshGenerator::processFrame] - Malla guardada " << 1 + ltm->tm_min << ":" << 1 + ltm->tm_sec;
        i++;
    }

    if(frame.second != NULL) { // En first viene un array de ThreadData con las texturas.

    }
}
