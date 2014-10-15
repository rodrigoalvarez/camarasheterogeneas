#include "modelImg.h"
#include <vector>
#include <limits>
#include <cmath>


using namespace std;
int masterImageIndex = 0;

Model_IMG::Model_IMG() {
    masterImageIndex += 100000;
    Id = masterImageIndex;
}

bool Model_IMG::MemoryLoad() {

    std::stringstream key1;
    key1 << "ImageId" << Id / 100000;
    memoryMappedImageId.setup(key1.str(), sizeof(int), false);
    isConnectedId = memoryMappedImageId.connect();
    if (isConnectedId) {
        id = memoryMappedImageId.getData();
    }

    std::stringstream key2;
    key2 << "ImagePixelsW" << Id / 100000;
    memoryMappedImageSizeW.setup(key2.str(), sizeof(int), false);
    isConnectedWPixels = memoryMappedImageSizeW.connect();
    if (isConnectedWPixels) {
        wPixels = memoryMappedImageSizeW.getData();
    }

    std::stringstream key3;
    key3 << "ImagePixelsH" << Id / 100000;
    memoryMappedImageSizeH.setup(key3.str(), sizeof(int), false);
    isConnectedHPixels = memoryMappedImageSizeH.connect();
    if (isConnectedHPixels) {
        hPixels = memoryMappedImageSizeH.getData();
    }

    std::stringstream key4;
    key4 << "ImagePixels" << Id / 100000;
    memoryMappedImage.setup(key4.str(), sizeof(unsigned char) * (*wPixels) * (*hPixels) * 3, false);
    isConnectedPixels = memoryMappedImage.connect();
    if (isConnectedPixels) {
        pixels = memoryMappedImage.getData();
    }

    if (isConnectedId && isConnectedWPixels && isConnectedHPixels && isConnectedPixels &&
        *id > Id && *wPixels > 0 && *hPixels > 0) {

        Id = *id;
        Width = *wPixels;
        Height = *hPixels;
        Pixels = new unsigned char[Width * Height * 3];
        memcpy(Pixels, pixels, sizeof(unsigned char) * Width * Height * 3);
        return true;
    }
    return false;
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
