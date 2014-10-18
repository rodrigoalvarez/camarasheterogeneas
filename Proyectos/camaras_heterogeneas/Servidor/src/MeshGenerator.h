#pragma once

#include "ofMain.h"
#include "ofxNetwork.h"
#include "Constants.h"
#include "ThreadData.h"
#include "FrameUtils.h"
#include "MainBuffer/MainBuffer.h"
#include "ServerGlobalData.h"
#include "ofxSharedMemory.h"

struct FaceStruct
{
    float p1[3];
    float p2[3];
    float p3[3];
};

class MeshGenerator : public ofThread {
	public:
        MeshGenerator() {
            buffer = NULL;
            char* dllName = "C:\\CamarasHeterogeneas\\Proyecto\\camarasheterogeneas\\Proyectos\\GenerarMallas\\bin\\Release\\GenerarMallas.dll";
            generateMeshLibrary =  LoadLibraryA(dllName);
            if (!generateMeshLibrary) {
                std::cout << "No se pudo cargar la libreria: " << dllName << std::endl;
            }

            dllName = "C:\\CamarasHeterogeneas\\Proyecto\\camarasheterogeneas\\Proyectos\\MemoriaCompartida\\bin\\MemoriaCompartida.dll";
            memorySharedLibrary =  LoadLibraryA(dllName);
            if (!memorySharedLibrary) {
                std::cout << "No se pudo cargar la libreria: " << dllName << std::endl;
            }
        }

        HINSTANCE generateMeshLibrary;
        HINSTANCE memorySharedLibrary;
		void threadedFunction();
		void processFrame();
		MainBuffer * buffer;
		t_data * sys_data;

        ofxSharedMemory<int*> nFacesMemoryMappedFile;
        int* numberFaces;

        ofxSharedMemory<FaceStruct*> facesMemoryMappedFile;
        FaceStruct* faces;

        bool isConnected;
};
