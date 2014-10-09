#include "main.h"

// a sample exported function
void DLL_EXPORT SomeFunction(const LPCSTR sometext)
{
    MessageBoxA(0, sometext, "DLL Message", MB_OK | MB_ICONINFORMATION);
}

ofxSharedMemory<int*> nFacesMemoryMappedFile;
ofxSharedMemory<FaceStruct*> facesMemoryMappedFile;

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

ofxSharedMemory<int*> memoryMappedImageId;
int* id;
ofxSharedMemory<unsigned char*> memoryMappedImage;
unsigned char* pixels;
ofxSharedMemory<int*> memoryMappedImageSizeW;
int* wPixels;
ofxSharedMemory<int*> memoryMappedImageSizeH;
int* hPixels;

bool isConnectedId;
bool isConnectedPixels;
bool isConnectedWPixels;
bool isConnectedHPixels;

void DLL_EXPORT ReadSharedImage(int* Id, unsigned char** pixels, int* wPixels, int* hPixels) {

    cout << "test2" << endl;
    std::stringstream key1;
    key1 << "ImageId" << *Id / 100000;
    memoryMappedImageId.setup(key1.str(), sizeof(int), false);
    isConnectedId = memoryMappedImageId.connect();
    if (isConnectedId) {
        id = memoryMappedImageId.getData();
    }

    std::stringstream key2;
    key2 << "ImagePixelsW" << *Id / 100000;
    memoryMappedImageSizeW.setup(key2.str(), sizeof(int), false);
    isConnectedWPixels = memoryMappedImageSizeW.connect();
    if (isConnectedWPixels) {
        wPixels = memoryMappedImageSizeW.getData();
    }
    else
        *wPixels = 0;

    std::stringstream key3;
    key3 << "ImagePixelsH" << *Id / 100000;
    memoryMappedImageSizeH.setup(key3.str(), sizeof(int), false);
    isConnectedHPixels = memoryMappedImageSizeH.connect();
    if (isConnectedHPixels) {
        hPixels = memoryMappedImageSizeH.getData();
    }
    else
        *hPixels = 0;

    std::stringstream key4;
    key4 << "ImagePixels" << *Id / 100000;
    memoryMappedImage.setup(key4.str(), sizeof(unsigned char) * (*wPixels) * (*hPixels) * 3, false);
    isConnectedPixels = memoryMappedImage.connect();
    if (isConnectedPixels) {
        *pixels = memoryMappedImage.getData();
    }
    else{
        *wPixels = 0;
        *hPixels = 0;
    }

    if (isConnectedId && isConnectedWPixels && isConnectedHPixels && isConnectedPixels &&
        *id > *Id && *wPixels > 0 && *hPixels > 0) {
        *Id = *id;
    }
}

bool isConnectedImageId;

void DLL_EXPORT ShareImage(int* imageId, unsigned char** pixels, int* wPixels, int* hPixels) {

    std::stringstream key1;
    key1 << "ImageId" << *imageId / 100000;
	memoryMappedImageId.setup(key1.str(), sizeof(int), true);
    isConnectedImageId = memoryMappedImageId.connect();
    memoryMappedImageId.setData(imageId);

    std::stringstream key2;
    key2 << "ImagePixelsW" << *imageId / 100000;
	memoryMappedImageSizeW.setup(key2.str(), sizeof(int), true);
    isConnectedWPixels = memoryMappedImageSizeW.connect();
    memoryMappedImageSizeW.setData(wPixels);

    std::stringstream key3;
    key3 << "ImagePixelsH" << *imageId / 100000;
	memoryMappedImageSizeH.setup(key3.str(), sizeof(int), true);
    isConnectedHPixels = memoryMappedImageSizeH.connect();
    memoryMappedImageSizeH.setData(hPixels);

    std::stringstream key4;
    key4 << "ImagePixels" << *imageId / 100000;
	memoryMappedImage.setup(key4.str(), sizeof(unsigned char) * (*wPixels) * (*hPixels) * 3, true);
    isConnectedPixels = memoryMappedImage.connect();
    memoryMappedImage.setData(*pixels);
}

bool isConnectedMeshId;
bool isConnectedFaces;
bool isConnectedNFaces;

ofxSharedMemory<int*> memoryMappedMeshId;
ofxSharedMemory<FaceStruct*> memoryMappedMesh;
ofxSharedMemory<int*> memoryMappedMeshSize;

void DLL_EXPORT ShareMesh(int meshId, int numberFaces, FaceStruct* faces) {

    std::stringstream key1;
    key1 << "MeshId" << meshId / 100000;
	memoryMappedMeshId.setup(key1.str(), sizeof(int), true);
    isConnectedMeshId = memoryMappedMeshId.connect();
    memoryMappedMeshId.setData(&meshId);

    std::stringstream key2;
    key2 << "MeshNumberFaces" << meshId / 100000;
	memoryMappedMeshSize.setup(key2.str(), sizeof(int), true);
    isConnectedNFaces = memoryMappedMeshSize.connect();
    memoryMappedMeshSize.setData(&numberFaces);

    std::stringstream key3;
    key3 << "MeshFaces" << meshId / 100000;
	memoryMappedMesh.setup(key3.str(), sizeof(FaceStruct) * numberFaces, true);
    isConnectedFaces = memoryMappedMesh.connect();
    memoryMappedMesh.setData(faces);
}

void DLL_EXPORT ReadSharedMesh(int* Id, int* numberFaces, FaceStruct** faces)
{
    std::stringstream key1;
    key1 << "MeshId" << *Id / 100000;
    cout << key1.str() << endl;

    memoryMappedMeshId.setup(key1.str(), sizeof(int), false);
    isConnectedId = memoryMappedMeshId.connect();
    if (isConnectedId) {
        id = memoryMappedMeshId.getData();
        cout << *Id << "c1" << endl;
    }

    std::stringstream key2;
    key2 << "MeshNumberFaces" << *Id / 100000;
    cout << key2.str() << endl;
    memoryMappedMeshSize.setup(key2.str(), sizeof(int), false);
    isConnectedNFaces = memoryMappedMeshSize.connect();
    if (isConnectedNFaces) {
        numberFaces = memoryMappedMeshSize.getData();
        cout << Id << "c2" << endl;
    }

    std::stringstream key3;
    key3 << "MeshFaces" << *Id / 100000;
    cout << key3.str() << endl;
    memoryMappedMesh.setup(key3.str(), sizeof(FaceStruct) * (*numberFaces), false);
    isConnectedFaces = memoryMappedMesh.connect();
    if (isConnectedFaces) {
        *faces = memoryMappedMesh.getData();
        cout << Id << "c3" << endl;
    }
    else
        *numberFaces = 0;

    if (*id > *Id && *numberFaces > 0) {
        cout << Id << "x1" << endl;
        *Id = *id;
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
