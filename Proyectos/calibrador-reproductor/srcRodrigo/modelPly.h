#include "ofxSharedMemory.h"
#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <cmath>
#include <string>
#include "masterPly.h"

class Model_PLY
{
	private:
	    ofxSharedMemory<int*> memoryMappedMeshId;
        int* id;
	    ofxSharedMemory<int*> memoryMappedMeshSize;
        int* numberFaces;
        ofxSharedMemory<FaceStruct*> memoryMappedMesh;
        FaceStruct* faces;

        bool isConnectedId;
        bool isConnectedNFaces;
        bool isConnectedFaces;

		float* Faces_Quads;
		float* Vertex_Buffer;

	public:
		Model_PLY();
		bool MemoryLoad();
		void Load(string filename);

		float* Faces_Triangles;
		float* Normals;

		int TotalConnectedTriangles;
		int TotalPoints;
		int TotalFaces;
		int Id;

		float MinCoord;
		float MaxCoord;
		float AlfaCoord;
};

