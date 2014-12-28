#include "ofxSharedMemory.h"
#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <cmath>
#include <string>
#include "FreeImage.h"

class Model_IMG
{
	private:
	    ofxSharedMemory<int*> memoryMappedImageId;
        int* id;
        ofxSharedMemory<unsigned char*> memoryMappedImage;
        unsigned char* pixels;
	    ofxSharedMemory<int*> memoryMappedImageSizeW;
        int* wPixels;
	    ofxSharedMemory<int*> memoryMappedImageSizeH;
        int* hPixels;

        bool isConnectedId;
        bool isConnectedPixels;
        bool isConnectedWPixels;
        bool isConnectedHPixels;

	public:
		Model_IMG();
		void MemoryLoad();
		bool MemoryCheck();
		void Load(string filename);

        int Id;
		unsigned char* Pixels;
		int Width;
		int Height;
};

