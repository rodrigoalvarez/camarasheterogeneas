#include "MeshGenerator.h"

#include <iostream>
#include <fstream>
#include <string>
#include <ctime>
#include <windows.h>
#include <iostream>
#include <sstream>

#include <stdio.h>
#include "FreeImage.h"

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
typedef void (*f_compartirMalla)(int idMesh, int numberFaces, FaceStruct* faces);
typedef void (*f_ShareImage) (int* idImage, unsigned char* pixels, int* wPixels, int* hPixels);


int nframe = 0;
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

        cout<< "entro!!!" << endl;
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

        generarMalla(nbIN, &faces, numberFaces, nframe);

        ///FIN GENERAR MALLA

        ///MEMORIA COMPARTIDA
        cout<< "entro1" << endl;
        f_compartirMalla ShareMesh = (f_compartirMalla)GetProcAddress(memorySharedLibrary, "ShareMesh");

        cout<< "entro2" << endl;
        int idMesh = 10000 + nframe;
        ShareMesh(idMesh, *numberFaces, faces);

        cout<< "entro3" << endl;

        ///FIN MEMORIA COMPARTIDA

//        delete nbIN;
//        delete numberFaces;
//        delete [] faces;
//        delete nbIN;
    }
    if(frame.second != NULL) { // En first viene un array de ThreadData con las texturas.
        cout<< "Procesamiento de imagenes" << endl;
        int j=1;
        ThreadData * iter = (ThreadData *) frame.second;
        f_ShareImage shareImage = (f_ShareImage)GetProcAddress(memorySharedLibrary, "ShareImage");
        do {
            ofBuffer imageBuffer;
            ofSaveImage(iter->img.getPixelsRef(), imageBuffer, OF_IMAGE_FORMAT_JPEG);

            FIMEMORY* stream = FreeImage_OpenMemory((unsigned char*) imageBuffer.getBinaryBuffer(), imageBuffer.size());

            FREE_IMAGE_FORMAT fif   = FreeImage_GetFileTypeFromMemory( stream, 0 );

            FIBITMAP *dib(0);
            dib = FreeImage_LoadFromMemory(fif, stream);

            unsigned char* pixels = (unsigned char*)FreeImage_GetBits(dib);

            int width = FreeImage_GetWidth(dib);
            int height = FreeImage_GetHeight(dib);
            int bpp = FreeImage_GetBPP(dib);

            int idMomento = iter->cliId;
            idMomento = idMomento*10 + iter->cameraType;
            idMomento = idMomento*10 + iter->camId;
            idMomento = idMomento*10000 + nframe % 10000;

            cout<< "Id momento: " << idMomento << endl;
            cout<< "bytepp: " << bpp/8 << endl;
            shareImage(&idMomento, pixels, &width, &height);
            iter = iter->sig;
            j++;
        } while(iter != NULL);
    }
    nframe++;
}
