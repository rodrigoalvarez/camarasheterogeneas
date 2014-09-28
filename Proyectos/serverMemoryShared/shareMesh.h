#ifndef SHAREMESH_H
#define SHAREMESH_H

#include <time.h>
#include <vector>
#include "MasterPly.h"
#include "ofxSharedMemory.h"

using namespace std;


class ShareMesh
{
    public:
        ShareMesh(string filename);

        bool isConnectedMeshId;
        bool isConnectedFaces;
        bool isConnectedNFaces;

	    ofxSharedMemory<int*> memoryMappedMeshId;
        int* meshId;
        ofxSharedMemory<FaceStruct*> memoryMappedMesh;
        FaceStruct* faces;
        ofxSharedMemory<int*> memoryMappedMeshSize;
        int* numberFaces;

    private:
        MasterPly* mPly;
};

#endif // SHAREMESH_H
