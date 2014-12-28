#include "ofxSharedMemory.h"
#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <cmath>
#include <string>
#include "FreeImage.h"

class Model_SET
{
	private:
        ofxSharedMemory<float*> memoryMappedValues;
        float* values;
	    ofxSharedMemory<int*> memoryMappedNValues;
        int* nValues;

        bool isConnectedValues;
        bool isConnectedNValues;

	public:
		Model_SET();
		bool MemoryLoad();

		float* Values;
		int NValues;
};

