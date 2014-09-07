#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <string>
#include <vector>
#include <iostream>

using namespace std;

class Model_XYZ
{
	public:
		Model_XYZ();
		int Load(string fileName);
		int Include(Model_XYZ* model);
		int Clear();

		vector<float> Points;

		int TotalPoints;

		float MinCoord;
		float MaxCoord;
		float AlfaCoord;
};

