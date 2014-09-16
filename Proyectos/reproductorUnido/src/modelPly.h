#include "ofxSharedMemory.h"
#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <cmath>
#include <string>
#include "masterPly.h"

class Model_PLY
{
	public:
	    ofxSharedMemory<int*> nFacesMemoryMappedFile;
        int* numberFaces;
        string nFacesMemoryKey = "NumberFaces";
        int nFacesMemorySize = sizeof(int);

        ofxSharedMemory<FaceStruct*> facesMemoryMappedFile;
        FaceStruct* faces;
        string facesMemoryKey = "Faces";
        int facesMemorySize;


        bool isConnected;

		Model_PLY();
		void MemoryLoad(int numberFacesActual);
		int Load(char *filename);
		float* calculateNormal( float *coord1, float *coord2, float *coord3 );

		float* Faces_Quads;
		float* Vertex_Buffer;

		float* Faces_Triangles;
		float* Normals;

		int TotalConnectedTriangles;
		int TotalPoints;
		int TotalFaces;

		float MinCoord;
		float MaxCoord;
		float AlfaCoord;
};

