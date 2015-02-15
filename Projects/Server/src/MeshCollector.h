#pragma once

#include "ofMain.h"
#include "ofxNetwork.h"
#include "Constants.h"
#include "ThreadData.h"
#include "FrameUtils.h"
#include "ServerGlobalData.h"
#include "MeshThreadedGenerator.h"
#include "FreeImage.h"
#include <iostream>
#include <list>
#include <string>
#include <cctype>

typedef void (*f_compartirMalla)(int idMesh, int numberFaces, FaceStruct* faces);
typedef void (*f_ShareImage)(int* idImage, unsigned char* pixels, int* wPixels, int* hPixels);

class MeshCollector : public ofThread {
	public:

        MeshCollector() {
            currProc   = 0;
            currFrame  = -1;
            char* dllName = "GenerarMallas.dll";
            generateMeshLibrary =  LoadLibraryA(dllName);
            if (!generateMeshLibrary) {
                std::cout << "No se pudo cargar la libreria: " << dllName << std::endl;
            }

            dllName = "MemoriaCompartida.dll";
            memorySharedLibrary =  LoadLibraryA(dllName);
            if (!memorySharedLibrary) {
                std::cout << "No se pudo cargar la libreria: " << dllName << std::endl;
            }
        }

        ~MeshCollector() {
        }

        HINSTANCE generateMeshLibrary;
        HINSTANCE memorySharedLibrary;

        t_data * sys_data;
        MeshThreadedGenerator * threads;
        int currProc;
        int currFrame;
        f_compartirMalla ShareMesh;
        f_ShareImage shareImage;
        std::list<GeneratedResult *> list;
        std::list<GeneratedResult *>::iterator it;

		void threadedFunction();
		void processFrame(ofEventArgs &e);
		void shareNextCompleteFrame();
		void shareFrame(GeneratedResult *);
};
