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

//ofxSharedMemory<int*> memoryMappedImageId;
//int* id;
//ofxSharedMemory<unsigned char*> memoryMappedImage;
//unsigned char* pixels;
//ofxSharedMemory<int*> memoryMappedImageSizeW;
////ofxSharedMemory<int*> memoryMappedImageSizeH;
//    bool isConnectedId;
//    bool isConnectedPixels;
//    bool isConnectedWPixels;
//    bool isConnectedHPixels;


void DLL_EXPORT ReadSharedImage(int* Id/*Id del momento osea camara mas frame*/, int* wPixels, int* hPixels , unsigned char** pixels) {

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
    cout << "1  " << key1.str()  <<endl;
    memoryMappedImageId.setup(key1.str(), sizeof(int), false);
    cout << "2" << endl;
    isConnectedImageId = memoryMappedImageId.connect();
    cout << "3" << endl;
    if (isConnectedImageId) {
        idMomentoNuevo = memoryMappedImageId.getData();
    cout << "4" << endl;

        std::stringstream key2;
        key2 << "ImagePixelsW" << *Id / 10000;
    cout << "5" << endl;
        memoryMappedImageSizeW.setup(key2.str(), sizeof(int), false);
    cout << "6" << endl;
        isConnectedWPixels = memoryMappedImageSizeW.connect();
    cout << "7" << endl;
        if (isConnectedWPixels) {
            *wPixels = *memoryMappedImageSizeW.getData();
    cout << "8" << endl;

            std::stringstream key3;
            key3 << "ImagePixelsH" << *Id / 10000;
            memoryMappedImageSizeH.setup(key3.str(), sizeof(int), false);
    cout << "9" << endl;
            isConnectedHPixels = memoryMappedImageSizeH.connect();
    cout << "10" << endl;
            if (isConnectedHPixels) {
                *hPixels = *memoryMappedImageSizeH.getData();
    cout << "11" << endl;
                std::stringstream key4;
                key4 << "ImagePixels" << *Id / 10000;
    cout << "12" << endl;
                memoryMappedImage.setup(key4.str(), sizeof(unsigned char) * (*wPixels) * (*hPixels) * 3, false);
    cout << "13" << endl;
                isConnectedPixels = memoryMappedImage.connect();
    cout << "14" << endl;
                if (isConnectedPixels) {

                    *pixels = memoryMappedImage.getData();
    cout << "15" << endl;

                        cout << "Guardando imagen" << endl;
                        char* nombre = new char[20];
                        sprintf(nombre,"imagen%d.png",*Id);
                        cout << "Guardando imagen1" << endl;


                        ofImage image;
                        cout << "Guardando imagen2" << endl;
                        image.setFromPixels(*pixels,*wPixels,*hPixels,OF_IMAGE_COLOR);
                        cout << "Guardando imagen3" << endl;
                        image.saveImage(nombre);
                        cout << "Guardando imagen4" << endl;

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

//bool isConnectedImageId;

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
                        cout << "Guardando imagen" << endl;
                        char* nombre = new char[20];
                        sprintf(nombre,"imagen%d.png",*imageId);
                        cout << "Guardando imagen1" << endl;


                        ofImage image;
                        cout << "Guardando imagen2" << endl;
                        image.setFromPixels(pixels,*wPixels,*hPixels,OF_IMAGE_COLOR);
                        cout << "Guardando imagen3" << endl;
                        image.saveImage(nombre);

                        cout << "Compartiendo imagen" << endl;
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

//    ofxSharedMemory<int*> memoryMappedMeshId;
//    ofxSharedMemory<FaceStruct*> memoryMappedMesh;
//    ofxSharedMemory<int*> memoryMappedMeshSize;
//
//    bool isConnectedMeshId;
//    bool isConnectedFaces;
//    bool isConnectedNFaces;

    std::stringstream key1;
    key1 << "MeshId" << idMesh / 10000;
	memoryMappedMeshId.setup(key1.str(), sizeof(int), true);

    cout << "***************** " << key1.str() << "****************"<<endl;
    cout << "idMesh " << idMesh <<endl;
    isConnectedMeshId = memoryMappedMeshId.connect();
    if (isConnectedMeshId){
//        cout << "Paso1 " <<endl;
        memoryMappedMeshId.setData(&idMesh);

        std::stringstream key2;
        key2 << "MeshNumberFaces" << idMesh / 10000;
        memoryMappedMeshSize.setup(key2.str(), sizeof(int), true);
        isConnectedNFaces = memoryMappedMeshSize.connect();
        if (isConnectedNFaces){
//            cout << "Paso2 " <<endl;
            int* nfaces = new int;
            *nfaces = numberFaces;
            memoryMappedMeshSize.setData(nfaces);
//            cout << "Paso21 " << *nfaces <<endl;

            std::stringstream key3;
            key3 << "MeshFaces" << idMesh; // 10000;
            cout << "Numero de caras: " << numberFaces<<endl;
            isConnectedFaces = false;
            while (!isConnectedFaces){
                memoryMappedMesh.setup(key3.str(), sizeof(FaceStruct) * numberFaces, true);
                isConnectedFaces = memoryMappedMesh.connect();
    //            cout << "Paso24 " <<endl;
                if (isConnectedFaces){
    //                cout << "Paso3 " <<endl;
                    memoryMappedMesh.setData(faces);
                    cout << "***************** TERMINO DLL****************"<<endl;
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
    ofxSharedMemory<int*> memoryMappedMeshId;
    ofxSharedMemory<FaceStruct*> memoryMappedMesh;
    ofxSharedMemory<int*> memoryMappedMeshSize;

    bool isConnectedMeshId;
    bool isConnectedFaces;
    bool isConnectedNFaces;
    std::stringstream key1;
    key1 << "MeshId" << *Id / 10000;
    int* id;

    cout << "*****Entro dll****** " <<  key1.str() << endl;
    memoryMappedMeshId.setup(key1.str(), sizeof(int), false);
    isConnectedMeshId = memoryMappedMeshId.connect();
    if (isConnectedMeshId) {

    cout << "Entro dll2" << endl;
        id = memoryMappedMeshId.getData();
        cout << "Id dll" << *id << endl;

        std::stringstream key2;
        key2 << "MeshNumberFaces" << *Id / 10000;

        memoryMappedMeshSize.setup(key2.str(), sizeof(int), false);
        isConnectedNFaces = memoryMappedMeshSize.connect();
        if (isConnectedNFaces) {
            *numberFaces = *memoryMappedMeshSize.getData();
            cout << "numberFaces " << *numberFaces << endl;

    cout << "Entro dll3" << endl;
            std::stringstream key3;
            key3 << "MeshFaces" << *id; // 10000;
            memoryMappedMesh.setup(key3.str(), sizeof(FaceStruct) * (*numberFaces), false);
            isConnectedFaces = memoryMappedMesh.connect();

    cout << "Entro dll4" << endl;
            if (isConnectedFaces) {
                *faces = memoryMappedMesh.getData();
    cout << "Entro dll5" << endl;
                cout << "*****LEYO MALLA COMPARTIDA******" << endl;
            }
            else
                *numberFaces = 0;
        }
    }

    if (*id > *Id && *numberFaces > 0) {
        cout << Id << "x1" << endl;
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
