#include "modelImg.h"
#include <vector>
#include <limits>
#include <cmath>


using namespace std;
int masterImageIndex = 0;

typedef void (*f_ReadSharedImage)(int* Id, int* Width, int* Height, unsigned char** Pixels);

Model_IMG::Model_IMG() {
    //masterImageIndex += 100000;
    //Id = masterImageIndex;

    char* dllName = "C:\\CamarasHeterogeneas\\Proyecto\\camarasheterogeneas\\Proyectos\\MemoriaCompartida\\bin\\MemoriaCompartida.dll";
    shareImageLibrary =  LoadLibraryA(dllName);
    if (!shareImageLibrary) {
        std::cout << "No se pudo cargar la libreria: " << dllName << std::endl;
    }
    primeraVez = true;
}

bool Model_IMG::MemoryLoad() {

    std::stringstream key1;
    key1 << "ImageId" << Id / 10000;

    f_ReadSharedImage readImage = (f_ReadSharedImage)GetProcAddress(shareImageLibrary, "ReadSharedImage");
    int* wPixels = new int;
    int* hPixels = new int;
    *wPixels = 44;
    *hPixels = 0;
    //cout << "Id1++++++  "<< Id <<endl;

    int idAux = Id;
    readImage(&Id, wPixels, hPixels, &pixels);

            //cout << "Id2++++++  "<< Id <<endl;
    if (Id >= 0){
            //cout << "Paso2++++++"<< endl;
        if (*wPixels > 0 && *hPixels > 0) {
            //cout << "Paso3++++++"<< endl;

            Width = *wPixels;
            Height = *hPixels;
            Pixels = new unsigned char[Width * Height * 3];
//            char* nombre = new char[20];
//            sprintf(nombre,"imagenReproductor%d.png",Id);
//            FIBITMAP* bitmap = FreeImage_Allocate(Width, Height, 3);
//            FreeImage_
//            FreeImage_Save(FIF_PNG, bitmap , nombre , 0 )


            memcpy(Pixels, pixels, sizeof(unsigned char) * Width * Height * 3);

//            ofImage image;
//            image.setFromPixels(Pixels,Width,Height,OF_IMAGE_COLOR);
//            image.saveImage(nombre);
            cout << "OBTUVO IMAGEN++++++"<< endl;
            //delete pixels;
            return true;
        }
        else
            return false;
    }
    else{
        Id = idAux;
        return false;
    }
}

void Model_IMG::Load(string filename) {

    if (primeraVez){
        ofImage auxImg;
        auxImg.loadImage(filename);

        ofBuffer imageBuffer;
        ofSaveImage(auxImg.getPixelsRef(), imageBuffer, OF_IMAGE_FORMAT_JPEG);

        FIMEMORY* stream = FreeImage_OpenMemory((unsigned char*) imageBuffer.getBinaryBuffer(), imageBuffer.size());

        FREE_IMAGE_FORMAT fif   = FreeImage_GetFileTypeFromMemory( stream, 0 );

        FIBITMAP *dib(0);
        dib = FreeImage_LoadFromMemory(fif, stream);

        Pixels = (unsigned char*)FreeImage_GetBits(dib);

        Width = FreeImage_GetWidth(dib);
        Height = FreeImage_GetHeight(dib);
        Id++;
        primeraVez = false;
        cout << "DISCOOOOO!!!!!" << endl;
    }
    else
        cout << "MEMORIA" << endl;
}

int sizeToPowerTwoLessInt(int n) {
    int res = 1;
    while (res < n) {
        res *= 2;
    }
    return res / 2;
}
int sizeToPowerTwoMoreInt(int n) {
    int res = 1;
    while (res < n) {
        res *= 2;
    }
    return res;
}

int sizeToPowerFourLessInt(int n) {
    int res = n;
    while (res % 4 != 0) {
        res -= 2;
    }
    return res;
}
int sizeToPowerFourMoreInt(int n) {
    int res = n;
    while (res % 4 != 0) {
        res += 2;
    }
    return res;
}
