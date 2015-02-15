#include "main.h"

ofxSharedMemory<int*> nFacesMemoryMappedFile;
ofxSharedMemory<FaceStruct*> facesMemoryMappedFile;

bool isConnected;

string facesMemoryKey = "Faces";
string nFacesMemoryKey = "NumberFaces";
int nFacesMemorySize = sizeof(int);
int facesMemorySize;

void DLL_EXPORT ReadSharedImage(int* Id, int* wPixels, int* hPixels , unsigned char** pixels) {

    ofxSharedMemory<int*> memoryMappedImageId;
    ofxSharedMemory<int*> memoryMappedImageSizeW;
    ofxSharedMemory<int*> memoryMappedImageSizeH;
    ofxSharedMemory<unsigned char*> memoryMappedImage;

    bool isConnectedImageId;
    bool isConnectedPixels;
    bool isConnectedWPixels;
    bool isConnectedHPixels;

    std::stringstream key1;
    key1 << "ImageId" << *Id / 10000;
    int* idMomentoNuevo;
    memoryMappedImageId.setup(key1.str(), sizeof(int), false);
    isConnectedImageId = memoryMappedImageId.connect();
    if (isConnectedImageId) {
        idMomentoNuevo = memoryMappedImageId.getData();
        std::stringstream key2;
        key2 << "ImagePixelsW" << *Id / 10000;
        memoryMappedImageSizeW.setup(key2.str(), sizeof(int), false);
        isConnectedWPixels = memoryMappedImageSizeW.connect();
        if (isConnectedWPixels) {
            *wPixels = *memoryMappedImageSizeW.getData();

            std::stringstream key3;
            key3 << "ImagePixelsH" << *Id / 10000;
            memoryMappedImageSizeH.setup(key3.str(), sizeof(int), false);
            isConnectedHPixels = memoryMappedImageSizeH.connect();
            if (isConnectedHPixels) {
                *hPixels = *memoryMappedImageSizeH.getData();
                std::stringstream key4;
                key4 << "ImagePixels" << *Id / 10000;
                memoryMappedImage.setup(key4.str(), sizeof(unsigned char) * (*wPixels) * (*hPixels) * 3, false);
                isConnectedPixels = memoryMappedImage.connect();
                if (isConnectedPixels) {

                    *pixels = memoryMappedImage.getData();
                    *Id = *idMomentoNuevo;
                }
                else{
                    *wPixels = 0;
                    *hPixels = 0;
                    *Id = -1;
                }
            }
            else{
                *hPixels = 0;
                *Id = -1;
            }
        }
        else{
            *wPixels = 0;
            *Id = -1;
        }
    }
    else
        *Id = -1;
}

void MemoryCheck(int* Id) {

    ofxSharedMemory<int*> memoryMappedImageId;

    bool isConnectedImageId;

    std::stringstream key1;
    key1 << "ImageId" << *Id / 10000;
    int* idMomentoNuevo;

    memoryMappedImageId.setup(key1.str(), sizeof(int), false);

    isConnectedImageId = memoryMappedImageId.connect();

    if (isConnectedImageId) {
        idMomentoNuevo = memoryMappedImageId.getData();
        if (*idMomentoNuevo > *Id)
            *Id = *idMomentoNuevo;
    }
    else
        *Id = -1;
}

void DLL_EXPORT ShareImage(int* imageId, unsigned char* pixels, int* wPixels, int* hPixels) {


    ofxSharedMemory<int*> memoryMappedImageId;
    ofxSharedMemory<int*> memoryMappedImageSizeW;
    ofxSharedMemory<int*> memoryMappedImageSizeH;
    ofxSharedMemory<unsigned char*> memoryMappedImage;
    bool isConnectedImageId;
    bool isConnectedPixels;
    bool isConnectedWPixels;
    bool isConnectedHPixels;

    std::stringstream key1;
    key1 << "ImageId" << *imageId / 10000;
    std::stringstream key2;
    key2 << "ImagePixelsW" << *imageId / 10000;
    std::stringstream key3;
    key3 << "ImagePixelsH" << *imageId / 10000;
    std::stringstream key4;
    key4 << "ImagePixels" << *imageId / 10000;

    bool shareImage = false;
    while (!shareImage){
        memoryMappedImageId.setup(key1.str(), sizeof(int), true);
        isConnectedImageId = memoryMappedImageId.connect();
        if (isConnectedImageId){
            memoryMappedImageId.setData(imageId);

            memoryMappedImageSizeW.setup(key2.str(), sizeof(int), true);
            isConnectedWPixels = memoryMappedImageSizeW.connect();
            if (isConnectedWPixels){
                memoryMappedImageSizeW.setData(wPixels);

                memoryMappedImageSizeH.setup(key3.str(), sizeof(int), true);
                isConnectedHPixels = memoryMappedImageSizeH.connect();
                if (isConnectedHPixels){
                    memoryMappedImageSizeH.setData(hPixels);

                    memoryMappedImage.setup(key4.str(), sizeof(unsigned char) * (*wPixels) * (*hPixels) * 3, true);
                    isConnectedPixels = memoryMappedImage.connect();
                    if (isConnectedPixels){
                        char* nombre = new char[20];
                        sprintf(nombre,"imagen%d.png",*imageId);


                        ofImage image;
                        image.setFromPixels(pixels,*wPixels,*hPixels,OF_IMAGE_COLOR);
                        image.saveImage(nombre);

                        memoryMappedImage.setData(pixels);
                        shareImage = true;
                    }
                }
            }
        }
    }
}

bool isConnectedMeshId;
bool isConnectedFaces;
bool isConnectedNFaces;

ofxSharedMemory<int*> memoryMappedMeshId;
ofxSharedMemory<FaceStruct*> memoryMappedMesh;
ofxSharedMemory<int*> memoryMappedMeshSize;


void DLL_EXPORT ShareMesh(int idMesh, int numberFaces, FaceStruct* faces) {

    std::stringstream key1;
    key1 << "MeshId" << idMesh / 10000;
	memoryMappedMeshId.setup(key1.str(), sizeof(int), true);
    isConnectedMeshId = memoryMappedMeshId.connect();
    if (isConnectedMeshId){
        memoryMappedMeshId.setData(&idMesh);

        std::stringstream key2;
        key2 << "MeshNumberFaces" << idMesh / 10000;
        memoryMappedMeshSize.setup(key2.str(), sizeof(int), true);
        isConnectedNFaces = memoryMappedMeshSize.connect();
        if (isConnectedNFaces){
            int* nfaces = new int;
            *nfaces = numberFaces;
            memoryMappedMeshSize.setData(nfaces);

            std::stringstream key3;
            key3 << "MeshFaces" << idMesh;
            isConnectedFaces = false;
            while (!isConnectedFaces){
                memoryMappedMesh.setup(key3.str(), sizeof(FaceStruct) * numberFaces, true);
                isConnectedFaces = memoryMappedMesh.connect();
                if (isConnectedFaces){
                    memoryMappedMesh.setData(faces);
                }
            }
        }
    }

    isConnectedMeshId = false;
    isConnectedNFaces = false;
    isConnectedFaces = false;

}

void DLL_EXPORT ReadSharedMesh(int* Id, int* numberFaces, FaceStruct** faces)
{
    std::stringstream key1;
    key1 << "MeshId" << *Id / 10000;
    int* id;

    memoryMappedMeshId.setup(key1.str(), sizeof(int), false);
    isConnectedMeshId = memoryMappedMeshId.connect();
    if (isConnectedMeshId) {

        id = memoryMappedMeshId.getData();

        std::stringstream key2;
        key2 << "MeshNumberFaces" << *Id / 10000;

        memoryMappedMeshSize.setup(key2.str(), sizeof(int), false);
        isConnectedNFaces = memoryMappedMeshSize.connect();
        if (isConnectedNFaces) {
            *numberFaces = *memoryMappedMeshSize.getData();
            std::stringstream key3;
            key3 << "MeshFaces" << *id;
            memoryMappedMesh.setup(key3.str(), sizeof(FaceStruct) * (*numberFaces), false);
            isConnectedFaces = memoryMappedMesh.connect();

            if (isConnectedFaces) {
                *faces = memoryMappedMesh.getData();
            }
            else
                *numberFaces = 0;
        }
    }

    if (*id > *Id && *numberFaces > 0) {
        *Id = *id;
    }
    else
        *Id = -1;

}

ofxSharedMemory<float*> memoryMappedCameras;
ofxSharedMemory<int*> memoryMappedCamerasIds;
ofxSharedMemory<int*> memoryMappedCamerasSize;
bool isConnectedNValues;
bool isConnectedIdsValues;
bool isConnectedValues;

void DLL_EXPORT ShareSetting(int numberValues, int* idsValues, float* values){

	memoryMappedCamerasSize.setup("SettingsNumberValues", sizeof(int), true);
    isConnectedNValues = memoryMappedCamerasSize.connect();
    while(!(isConnectedNValues && isConnectedIdsValues && isConnectedValues)){
        if (isConnectedNValues){
            memoryMappedCamerasSize.setData(&numberValues);

            memoryMappedCamerasIds.setup("SettingsIdsValues", sizeof(int)*numberValues, true);
            isConnectedIdsValues = memoryMappedCamerasIds.connect();
            if (isConnectedIdsValues){
                memoryMappedCamerasIds.setData(idsValues);

                memoryMappedCameras.setup("SettingsValues", sizeof(float) * numberValues * 16, true);
                isConnectedValues = memoryMappedCameras.connect();
                if (isConnectedValues){
                    memoryMappedCameras.setData(values);
                }
            }
        }
    }
    isConnectedNValues = false;
    isConnectedIdsValues = false;
    isConnectedValues = false;
}

ofxSharedMemory<float*> memoryMappedValues;
ofxSharedMemory<int*> memoryMappedIds;
ofxSharedMemory<int*> memoryMappedNValues;

void DLL_EXPORT ReadSharedSetting(int* numberValues, int** idsValues, float** values){

    memoryMappedNValues.setup("SettingsNumberValues", sizeof(int), false);
    isConnectedNValues = memoryMappedNValues.connect();
    while(!(isConnectedNValues && isConnectedIdsValues && isConnectedValues)){
        if (isConnectedNValues) {
            numberValues = memoryMappedNValues.getData();

            memoryMappedIds.setup("SettingsIdsValues", sizeof(int) * (*numberValues), true);
            isConnectedIdsValues = memoryMappedIds.connect();
            if (isConnectedIdsValues){
                *idsValues = memoryMappedIds.getData();

                memoryMappedValues.setup("SettingsValues", sizeof(float) * (*numberValues) * 16, false);
                isConnectedValues = memoryMappedValues.connect();
                if (isConnectedValues) {
                    *values = memoryMappedValues.getData();

                }
            }
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
