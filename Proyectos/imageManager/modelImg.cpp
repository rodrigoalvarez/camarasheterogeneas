#include "modelImg.h"
#include "FreeImage.h"
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
    Width = auxImg.getWidth();
    Height = auxImg.getHeight();
    unsigned char* imgPixels = auxImg.getPixels();
    //int powWidth = sizeToPowerTwoLessInt(Width);
    //int powHeight = sizeToPowerTwoLessInt(Height);
    int powWidth = 256;
    int powHeight = 128;
    ofImage img;
    img.allocate(powWidth, powHeight, OF_IMAGE_COLOR);

    int h = 0;
    while ( h < img.getPixelsRef().size() ) {
        img.getPixelsRef()[h] = 255.f;
        h++;
    }
    for (int i = 0; i < powHeight; i++) {
        for (int j = 0; j < powWidth; j++) {
            for (int k = 0; k < 3; k++) {
                unsigned char pp = imgPixels[3 * (i * Width + j) + k];
                int pn = (int)pp;
                img.getPixelsRef()[3 * (i * powWidth + j) + k] = pn;
            }
        }
    }
    cout << filename << endl;
    img.reloadTexture();
    img.saveImage("image.png");
    Pixels = (unsigned char*)auxImg.getPixels();
    cout << Width << " - " << Height << endl;
    Width = powWidth;//auxImg.getWidth();
    Height = powHeight;//auxImg.getHeight();
    cout << powWidth << " - " << powHeight << endl;
    Id++;
}
