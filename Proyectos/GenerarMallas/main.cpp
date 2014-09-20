#include "main.h"

// a sample exported function
void DLL_EXPORT SomeFunction(const char* sometext, int* number, char** salida)
{
    MessageBoxA(0, sometext, "DLL Message", MB_OK | MB_ICONINFORMATION);
    //sometext = "chauuuu";
    //MessageBoxA(0, sometext2, "DLL Message", MB_OK | MB_ICONINFORMATION);
    *number = 125;
    *salida = "pepeeeeeee";
}

void DLL_EXPORT generarMalla(NubePuntos* nbIN, FaceStruct** faces, int* numberFaces, int nroFrame)
{
        MasterPly* mply = new MasterPly();

        mply->loadMesh(nbIN->x,nbIN->y,nbIN->z,nbIN->largo);
        mply->poissonDiskSampling(nbIN->largo/4);
        mply->calculateNormalsVertex();
        mply->buildMeshBallPivoting();
        mply->laplacianSmooth(3);
        *numberFaces = mply->totalFaces();
        *faces = mply->getFaces();

        /*char nombre[50];
        sprintf(nombre, "frame%d.ply", nroFrame);
        mply->savePLY(nombre,true);*/
}

void DLL_EXPORT generarMallaCalibrador(NubePuntos* nbIN, FaceStruct** faces, int* numberFaces)
{
        MasterPly* mply = new MasterPly();

        mply->loadMesh(nbIN->x,nbIN->y,nbIN->z,nbIN->largo);
        mply->poissonDiskSampling(nbIN->largo/2);
        mply->calculateNormalsVertex();
        mply->buildMeshBallPivoting();
        mply->laplacianSmooth(3);
        *numberFaces = mply->totalFaces();
        *faces = mply->getFaces();

        mply->savePLY("mallaUnida.ply",true);
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
