#include "MeshGenerator.h"

#include <iostream>
#include <fstream>
#include <string>
#include <ctime>
#include <windows.h>
#include <iostream>
#include <sstream>

#include <stdio.h>

using namespace std;

struct NubePuntos{
    float* x;
    float* y;
    float* z;
    int largo;
};

string facesMemoryKey = "Faces";
string nFacesMemoryKey = "NumberFaces";
int nFacesMemorySize = sizeof(int);
int facesMemorySize;

typedef void (*f_generarMalla)(NubePuntos* nbIN, FaceStruct** faces, int* numberFaces, int nroFrame);
typedef void (*f_compartirMalla)(int numberFaces, FaceStruct* faces);
typedef void (*f_ShareImage) (unsigned char** pixels, int* wPixels, int* hPixels);


int i = 0;
void MeshGenerator::threadedFunction() {
    if(buffer == NULL) return;

    while(isThreadRunning()) {
        ofSleepMillis(1000/sys_data->fps);
        processFrame();
    }
}

void MeshGenerator::processFrame(){

    std::pair <ThreadData *, ThreadData *> frame = buffer->getNextFrame();
    if(frame.first != NULL) { // En first viene un ThreadData con la nube de puntos.
        ThreadData* td = ((ThreadData *) frame.first);
        time_t now = time(0);
        tm *ltm = localtime(&now);

        ///GENERAR MALLA
        f_generarMalla generarMalla = (f_generarMalla)GetProcAddress(generateMeshLibrary, "generarMalla");

        NubePuntos* nbIN = new NubePuntos;
        nbIN->largo = td->nubeLength;
        nbIN->x = td->xpix;
        nbIN->y = td->ypix;
        nbIN->z = td->zpix;
        faces = new FaceStruct;
        numberFaces = new int;
        generarMalla(nbIN, &faces, numberFaces, i);

        ///FIN GENERAR MALLA

        ///MEMORIA COMPARTIDA
        cout<< "entro1" << endl;
        f_compartirMalla compartirMalla = (f_compartirMalla)GetProcAddress(memorySharedLibrary, "compartirMemoria");

        cout<< "entro2" << endl;
        compartirMalla(*numberFaces, faces);

        cout<< "entro3" << endl;

        /*
        nFacesMemoryMappedFile.setup(nFacesMemoryKey, nFacesMemorySize, true);
        isConnected = nFacesMemoryMappedFile.connect();

        if(isConnected){
            nFacesMemoryMappedFile.setData(numberFaces);
            facesMemorySize = sizeof(FaceStruct)* (*numberFaces);

            facesMemoryMappedFile.setup(facesMemoryKey, facesMemorySize, true);
            isConnected = facesMemoryMappedFile.connect();
            if(isConnected){
                facesMemoryMappedFile.setData(faces);
            }
        }*/
        ///FIN MEMORIA COMPARTIDA

        delete nbIN;
        delete numberFaces;
        delete [] faces;
        delete nbIN;
    }
    if(frame.second != NULL) { // En first viene un array de ThreadData con las texturas.
        int i=1;
        ThreadData * iter = (ThreadData *) frame.second;
        f_ShareImage shareImage = (f_ShareImage)GetProcAddress(memorySharedLibrary, "ShareImage");
        do {
            ofBuffer imageBuffer;
            ofSaveImage(iter->img.getPixelsRef(), imageBuffer, OF_IMAGE_FORMAT_JPEG);
            unsigned char* pixels = (unsigned char*) imageBuffer.getBinaryBuffer();
            int width = iter->img.width;
            int height = iter->img.height;
            shareImage( &pixels, &width, &height);
            iter = iter->sig;
            i++;
        } while(iter != NULL);
    }
}
