#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <cmath>
#include <string>


class Model_PLY
{
	public:
		Model_PLY();
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

