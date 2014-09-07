#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <string>
#include <vector>

using namespace std;

class Model_XYZ
{
	public:
		Model_XYZ();
		int Load(string fileName);

		vector<float> Points;

		int TotalPoints;

		float MinCoord;
		float MaxCoord;
		float AlfaCoord;
};

