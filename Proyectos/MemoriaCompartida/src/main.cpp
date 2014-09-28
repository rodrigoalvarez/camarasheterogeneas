#include "main.h"

// a sample exported function
void DLL_EXPORT SomeFunction(const LPCSTR sometext)
{
    MessageBoxA(0, sometext, "DLL Message", MB_OK | MB_ICONINFORMATION);
}

ofxSharedMemory<int*> nFacesMemoryMappedFile;
int* numberFaces;

ofxSharedMemory<FaceStruct*> facesMemoryMappedFile;
FaceStruct* faces;

bool isConnected;

string facesMemoryKey = "Faces";
string nFacesMemoryKey = "NumberFaces";
int nFacesMemorySize = sizeof(int);
int facesMemorySize;
// a sample exported function
void DLL_EXPORT compartirMemoria(int numberFaces, FaceStruct* faces)
{
    nFacesMemoryMappedFile.setup(nFacesMemoryKey, nFacesMemorySize, true);
    isConnected = nFacesMemoryMappedFile.connect();

    if(isConnected){
        nFacesMemoryMappedFile.setData(&numberFaces);
        facesMemorySize = sizeof(FaceStruct)* (numberFaces);

        facesMemoryMappedFile.setup(facesMemoryKey, facesMemorySize, true);
        isConnected = facesMemoryMappedFile.connect();
        if(isConnected){
            facesMemoryMappedFile.setData(faces);
        }
    }
}

void DLL_EXPORT leerMemoria(int* numberFaces, FaceStruct** faces)
{
    nFacesMemoryMappedFile.setup(nFacesMemoryKey, nFacesMemorySize, false);
    isConnected = nFacesMemoryMappedFile.connect();

    int* aux;
    if(isConnected){
        aux = nFacesMemoryMappedFile.getData();
        *numberFaces =  *aux;

        if (*numberFaces > 0){

            facesMemorySize = sizeof(FaceStruct)* (*numberFaces);

            facesMemoryMappedFile.setup(facesMemoryKey, facesMemorySize, false);
            isConnected = facesMemoryMappedFile.connect();

            if(isConnected)
                *faces = facesMemoryMappedFile.getData();
            else
                *numberFaces = 0;
        }
    }
}

extern "C" DLL_EXPORT BOOL APIENTRY DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
    switch (fdwReason)
    {
        case DLL_PROCESS_ATTACH:
            // attach to process
            // return FALSE to fail DLL load
            break;

        case DLL_PROCESS_DETACH:
            // detach from process
            break;

        case DLL_THREAD_ATTACH:
            // attach to thread
            break;

        case DLL_THREAD_DETACH:
            // detach from thread
            break;
    }
    return TRUE; // succesful
}
