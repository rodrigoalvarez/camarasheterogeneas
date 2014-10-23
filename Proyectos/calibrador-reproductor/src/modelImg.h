//#include "ofxSharedMemory.h"
#include "ofMain.h"
#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <cmath>
#include <string>
#include "FreeImage.h"

class Model_IMG
{
	private:
        unsigned char* pixels;
        int* wPixels;
        int* hPixels;

        bool isConnectedId;
        bool isConnectedPixels;
        bool isConnectedWPixels;
        bool isConnectedHPixels;

        HINSTANCE shareImageLibrary;

	public:
		Model_IMG();
		bool MemoryLoad();
		void Load(string filename);

        int Id;
		unsigned char* Pixels;
		int Width;
		int Height;
};

