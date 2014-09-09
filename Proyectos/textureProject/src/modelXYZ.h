#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <string>
#include <vector>
#include <iostream>
#include <GL/glut.h>

using namespace std;

class Model_XYZ
{
	public:
		Model_XYZ();
		int Load(string fileName, float minCoord, float maxCoord);
		int Include(Model_XYZ* model, GLdouble* m);
		int Clear();

		vector<float> Points;

		int TotalPoints;

		float MinCoord;
		float MaxCoord;
		float AlfaCoord;
};

