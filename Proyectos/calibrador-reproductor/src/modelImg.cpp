#include "modelImg.h"
#include <vector>
#include <limits>
#include <cmath>


using namespace std;
int masterImageIndex = 0;

typedef void (*f_ReadSharedImage)(int* Id, unsigned char** Pixels, int* Width, int* Height);

Model_IMG::Model_IMG() {
    masterImageIndex += 100000;
    Id = masterImageIndex;

    char* dllName = "C:\\CamarasHeterogeneas\\Proyecto\\camarasheterogeneas\\Proyectos\\MemoriaCompartida\\bin\\MemoriaCompartida.dll";
    shareImageLibrary =  LoadLibraryA(dllName);
    if (!shareImageLibrary) {
        std::cout << "No se pudo cargar la libreria: " << dllName << std::endl;
    }
}

void Model_IMG::MemoryLoad() {

    cout << "test2" << endl;
    std::stringstream key1;
    key1 << "ImageId" << Id / 100000;

    f_ReadSharedImage readImage = (f_ReadSharedImage)GetProcAddress(shareImageLibrary, "ReadSharedImage");

    readImage(&Id, &pixels, wPixels, hPixels);

    if (*wPixels > 0 && *hPixels > 0) {

        Width = *wPixels;
        Height = *hPixels;
        Pixels = new unsigned char[Width * Height * 3];
        memcpy(Pixels, pixels, sizeof(unsigned char) * Width * Height * 3);
    }
}

void Model_IMG::Load(string filename) {

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
