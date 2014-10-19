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
typedef void (*f_compartirMalla)(int numberFaces, FaceStruct* faces);
typedef void (*f_ShareImage) (unsigned char* pixels, int* wPixels, int* hPixels);


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

        cout<< "entro!!!" << endl;
        ThreadData* td = ((ThreadData *) frame.first);
        time_t now = time(0);
        tm *ltm = localtime(&now);

        /*char nombre2[50];
        sprintf(nombre2, "frame%d.xyz", i);
        //char* fileName      = "frameeee_i.xyz";

        //Creo el archivo b,n de la nube unida
        FILE * pFile;
        pFile = fopen (nombre2,"w");
        //Recorro los frames de cada camara y me quedo solo con los 3D
        for(int i=0; i < td->nubeLength; i ++) {
            fprintf (pFile, "%f %f %f\n", td->xpix[i], td->ypix[i], td->zpix[i]);
        }

        fclose (pFile);*/

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
        i++;
    }
    if(frame.second != NULL) { // En first viene un array de ThreadData con las texturas.
        int j=1;
        ThreadData * iter = (ThreadData *) frame.second;
        f_ShareImage shareImage = (f_ShareImage)GetProcAddress(memorySharedLibrary, "ShareImage");
        do {
            cout<< "Imagen" << j << endl;
            ofBuffer imageBuffer;
            cout<< "Paso1" << j << endl;
            ofSaveImage(iter->img.getPixelsRef(), imageBuffer, OF_IMAGE_FORMAT_JPEG);

            FIMEMORY* stream = FreeImage_OpenMemory((unsigned char*) imageBuffer.getBinaryBuffer(), imageBuffer.size());

            FREE_IMAGE_FORMAT fif   = FreeImage_GetFileTypeFromMemory( stream, 0 );

            FIBITMAP *dib(0);
            dib = FreeImage_LoadFromMemory(fif, stream);

            unsigned char* pixels = (unsigned char*)FreeImage_GetBits(dib);

            int width = FreeImage_GetWidth(dib);
            int height = FreeImage_GetHeight(dib);

            shareImage( pixels, &width, &height);
            iter = iter->sig;
            j++;
        } while(iter != NULL);
    }
}
