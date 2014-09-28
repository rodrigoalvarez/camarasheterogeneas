#include "modelImg.h"
#include <vector>
#include <limits>
#include <cmath>


using namespace std;


Model_IMG::Model_IMG() {
    Id = 0;
}

void Model_IMG::MemoryLoad() {

    memoryMappedImageId.setup("ImageId", sizeof(int), false);
    isConnectedId = memoryMappedImageId.connect();
    if (isConnectedId) {
        id = memoryMappedImageId.getData();
    }
    memoryMappedImageSizeW.setup("ImagePixelsW", sizeof(int), false);
    isConnectedWPixels = memoryMappedImageSizeW.connect();
    if (isConnectedWPixels) {
        wPixels = memoryMappedImageSizeW.getData();
    }
    memoryMappedImageSizeH.setup("ImagePixelsH", sizeof(int), false);
    isConnectedHPixels = memoryMappedImageSizeH.connect();
    if (isConnectedHPixels) {
        hPixels = memoryMappedImageSizeH.getData();
    }
    memoryMappedImage.setup("ImagePixels", sizeof(char) * (*wPixels) * (*hPixels) * 3, false);
    isConnectedPixels = memoryMappedImage.connect();
    if (isConnectedPixels) {
        pixels = memoryMappedImage.getData();
    }

    if (isConnectedId && isConnectedWPixels && isConnectedHPixels && isConnectedPixels &&
        *id > 0 && *id != Id) {

        Id = *id;
        Width = *wPixels;
        Height = *hPixels;
        Pixels = new unsigned char[Width * Height * 3];
        memcpy(Pixels, pixels, sizeof(char) * Width * Height * 3);
    }
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
