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

int Model_XYZ::Load(string fileName) {

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
    AlfaCoord = std::max(std::abs(MinCoord), std::abs(MaxCoord));
    for (int i = 0; i < TotalPoints * 3; i++) {
        Points[i] = (Points[i] / AlfaCoord) * 10;
    }

	return TotalPoints;
}

int Model_XYZ::Include(Model_XYZ* model) {

    for (int i = 0; i < model->TotalPoints * 3; i++) {
        Points.push_back(model->Points[i]);
    }
    TotalPoints += model->TotalPoints;
	return TotalPoints;
}

int Model_XYZ::Clear() {

    TotalPoints = 0;
    Points.clear();
	return 0;
}
