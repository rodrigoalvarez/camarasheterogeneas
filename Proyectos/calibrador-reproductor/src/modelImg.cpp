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
        Pixels = new char[Width * Height * 3];
        memcpy(Pixels, pixels, sizeof(char) * Width * Height * 3);
    }
}

void Model_IMG::Load(char* filename) {

    FREE_IMAGE_FORMAT fif = FIF_UNKNOWN;
    FIBITMAP *dib(0);
    BYTE* bits(0);
    unsigned int width(0), height(0);
    GLuint gl_texID;

    fif = FreeImage_GetFileType(filename, 0);
    if(fif == FIF_UNKNOWN)
        fif = FreeImage_GetFIFFromFilename(filename);
    if(fif == FIF_UNKNOWN)
        cout << "Formato de imagen no reconocido." << endl;
    if(FreeImage_FIFSupportsReading(fif))
        dib = FreeImage_Load(fif, filename);
    if(!dib)
        cout << "Error al cargar la imagen." << endl;

    bits = FreeImage_GetBits(dib);
    width = FreeImage_GetWidth(dib);
    height = FreeImage_GetHeight(dib);
    if((bits == 0) || (width == 0) || (height == 0))
        cout << "Error en la imagen." << endl;


    // esto esta mal falta un x 3

    pixels = new char[width * height];
    for (int i = 0; i < width * height; i++) {
        pixels[i] = bits[i];
    }
    FreeImage_Unload(dib);
    width = width;
    Height = height;
    Pixels = pixels;
    Id++;
}
