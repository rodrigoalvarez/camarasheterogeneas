#include "modelXYZ.h"
#include <stdlib.h>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <limits>
#include <cmath>


Model_XYZ::Model_XYZ() {

}

int Model_XYZ::Load(string fileName, float minCoord, float maxCoord) {

	TotalPoints = 0;
    std::ifstream in(fileName.c_str());
    std::stringstream buffer;
    buffer << in.rdbuf();

    string line;
    while (getline(buffer, line, '\n')) {
        istringstream subBuffer(line);
        string point;
        for (int i = 0; i < 3 && getline(subBuffer, point, ' '); i++) {
            float value = ::atof(point.c_str());
            Points.push_back(value);
        }
        TotalPoints++;
    }
    in.close();

    if (minCoord == 0 && maxCoord == 0) {
        MinCoord = std::numeric_limits<float>::max();
        MaxCoord = std::numeric_limits<float>::min();
        for (int i = 0; i < TotalPoints * 3; i++) {
            if (Points[i] < MinCoord) {
                MinCoord = Points[i];
            }
            if (Points[i] > MaxCoord) {
                MaxCoord = Points[i];
            }
        }
    } else {
        MinCoord = minCoord;
        MaxCoord = maxCoord;
    }
    AlfaCoord = std::max(std::abs(MinCoord), std::abs(MaxCoord));
    for (int i = 0; i < TotalPoints * 3; i++) {
        Points[i] = (Points[i] / AlfaCoord) * 10;
    }

	return TotalPoints;
}

int Model_XYZ::Include(Model_XYZ* model, GLdouble* m) {

    for (int i = 0; i < model->TotalPoints * 3; i += 3) {
        float x = model->Points[i];
        float y = model->Points[i+1];
        float z = model->Points[i+2];
        Points.push_back(m[0]*x + m[1]*y + m[2]*z);
        Points.push_back(m[4]*x + m[5]*y + m[6]*z);
        Points.push_back(m[8]*x + m[9]*y + m[10]*z);
    }
    TotalPoints += model->TotalPoints;
	return TotalPoints;
}

int Model_XYZ::Clear() {

    TotalPoints = 0;
    Points.clear();
	return 0;
}
