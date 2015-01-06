#pragma once

#include "ofMain.h"
#include "ServerGlobalData.h"
#include "ThreadData.h"

#define GENERATOR_IDLE 1
#define GENERATOR_LOADED 2
#define GENERATOR_BUSY 2
#define GENERATOR_COMPLETE 3

struct FaceStruct{
    float p1[3];
    float p2[3];
    float p3[3];
};

struct NubePuntos{
    float* x;
    float* y;
    float* z;
    int largo;
};

struct GeneratedResult{
    int nframe;
    int idMesh;
    NubePuntos * nube;
    /*
    int idMomento;
    unsigned char* pixels;
    int width;
    int height;*/
    ThreadData * textures;
    int * numberFaces;
    bool hasDepth;
    bool hasRGB;
    FaceStruct* faces;
};

typedef void (*f_generarMalla)(NubePuntos* nbIN, FaceStruct** faces, int* numberFaces, int nroFrame);

class MeshThreadedGenerator : public ofThread {
	public:
        MeshThreadedGenerator() {
            char* dllName = "GenerarMallas.dll";
            generateMeshLibrary =  LoadLibraryA(dllName);

            if(!generateMeshLibrary) {
                std::cout << "No se pudo cargar la libreria: " << dllName << std::endl;
            }
            state           = GENERATOR_IDLE;
        }

        HINSTANCE generateMeshLibrary;
        int state;
        int nframe;
        int nMTG;
        void threadedFunction();
		void processFrame(ofEventArgs &e);
		void setState(int state);
		int getState();
		FaceStruct* faces;
		GeneratedResult * result;
		t_data * sys_data;
		int* numberFaces;
		std::pair <ThreadData *, ThreadData *> frame;
		void processMesh(std::pair <ThreadData *, ThreadData *> frame, int id);
		f_generarMalla generarMalla;
};
