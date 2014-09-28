#include "shareImage.h"

int masterIndex = 1;

void ShareImage::Load(string filename) {

    ofImage auxImg;
    auxImg.loadImage(filename);

    ofBuffer imageBuffer;
    ofSaveImage(auxImg.getPixelsRef(), imageBuffer, OF_IMAGE_FORMAT_JPEG);

    FIMEMORY* stream = FreeImage_OpenMemory((unsigned char*) imageBuffer.getBinaryBuffer(), imageBuffer.size());

    FREE_IMAGE_FORMAT fif   = FreeImage_GetFileTypeFromMemory( stream, 0 );

    FIBITMAP *dib(0);
    dib = FreeImage_LoadFromMemory(fif, stream);

    pixels = (unsigned char*)FreeImage_GetBits(dib);

    *wPixels = FreeImage_GetWidth(dib);
    *hPixels = FreeImage_GetHeight(dib);
}

ShareImage::ShareImage(string filename) {

    imageId = new int;
    wPixels = new int;
    hPixels = new int;
    masterIndex += 100000;
    *imageId = masterIndex;

    Load(filename);

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
	memoryMappedImage.setup(key4.str(), sizeof(char) * (*wPixels) * (*hPixels) * 3, true);
    isConnectedPixels = memoryMappedImage.connect();
    memoryMappedImage.setData(pixels);
}
