#ifndef SHAREIMAGE_H
#define SHAREIMAGE_H

#include <time.h>
#include <vector>
#include "ofxSharedMemory.h"
#include "FreeImage.h"

using namespace std;


class ShareImage
{
    public:
        ShareImage(string filename);

        bool isConnectedImageId;
        bool isConnectedPixels;
        bool isConnectedWPixels;
        bool isConnectedHPixels;

	    ofxSharedMemory<int*> memoryMappedImageId;
        int* imageId;
        ofxSharedMemory<unsigned char*> memoryMappedImage;
        unsigned char* pixels;
	    ofxSharedMemory<int*> memoryMappedImageSizeW;
        int* wPixels;
	    ofxSharedMemory<int*> memoryMappedImageSizeH;
        int* hPixels;

    private:
        void Load(string filename);
};

#endif // SHAREIMAGE_H
