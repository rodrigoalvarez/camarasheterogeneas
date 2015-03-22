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

int Model_XYZ::Load(string fileName, float alfa) {

	TotalPoints = 0;
    std::ifstream in(fileName.c_str());
    std::stringstream buffer;
    buffer << in.rdbuf();

    string line;
    hasColor = false;
    while (getline(buffer, line, '\n')) {
        istringstream subBuffer(line);
        string point;
        for (int i = 0; i < 6 && getline(subBuffer, point, ' '); i++) {

            if (i < 3){
               float value = ::atof(point.c_str());
               Points.push_back(value);
           }
           else{
           //cout << "entro load" << endl;
               hasColor = true;
               float value = ::atof(point.c_str());
               ColorPoints.push_back(value);
           }
        }
        TotalPoints++;
    }
    in.close();

    float minX = std::numeric_limits<float>::max();
    float minY = std::numeric_limits<float>::max();
    float minZ = std::numeric_limits<float>::max();
    float maxX = std::numeric_limits<float>::min();
    float maxY = std::numeric_limits<float>::min();
    float maxZ = std::numeric_limits<float>::min();

    MinCoord = std::numeric_limits<float>::max();
    MaxCoord = std::numeric_limits<float>::min();
    for (int i = 0; i < TotalPoints * 3; i++) {
        if (Points[i] < MinCoord) {
            MinCoord = Points[i];
        }
        if (Points[i] > MaxCoord) {
            MaxCoord = Points[i];
        }
        if (i % 3 == 0) {
            if (Points[i] < minX) {
                minX = Points[i];
            }
            if (Points[i] > maxX) {
                maxX = Points[i];
            }
        }
        if (i % 3 == 1) {
            if (Points[i] < minY) {
                minY = Points[i];
            }
            if (Points[i] > maxY) {
                maxY = Points[i];
            }
        }
        if (i % 3 == 2) {
            if (Points[i] < minZ) {
                minZ = Points[i];
            }
            if (Points[i] > maxZ) {
                maxZ = Points[i];
            }
        }
    }

    if (alfa == 0) {
        AlfaCoord = std::max(std::abs(MinCoord), std::abs(MaxCoord));
    } else {
        AlfaCoord = alfa;
    }

    float deltaX = (maxX + minX) / 2;
    float deltaY = (maxY + minY) / 2;
    float deltaZ = (maxZ + minZ) / 2;
    for (int i = 0; i < TotalPoints * 3; i++) {
//        if (i % 3 == 0) {
//            Points[i] = Points[i] - deltaX;
//        }
//        if (i % 3 == 1) {
//            Points[i] = Points[i] - deltaY;
//        }
//        if (i % 3 == 2) {
//            Points[i] = Points[i] - deltaZ;
//        }
        Points[i] = (Points[i] / AlfaCoord) * 10;
    }

	return TotalPoints;
}

int Model_XYZ::Include(Model_XYZ* model, GLdouble* m) {

    for (int i = 0; i < model->TotalPoints * 3; i += 3) {
       float x = model->Points[i];
       float y = model->Points[i+1];
       float z = model->Points[i+2];

       float w = 1;
       Points.push_back(m[0]*x + m[1]*y + m[2]*z + m[12]);
       Points.push_back(m[4]*x + m[5]*y + m[6]*z + m[13]);
       Points.push_back(m[8]*x + m[9]*y + m[10]*z + m[14]);

       if(model->hasColor)
       {
           float r = model->ColorPoints[i];
           float g = model->ColorPoints[i+1];
           float b = model->ColorPoints[i+2];
           ColorPoints.push_back(m[0]*r + m[1]*g + m[2]*b + m[12]);
           ColorPoints.push_back(m[4]*r + m[5]*g + m[6]*b + m[13]);
           ColorPoints.push_back(m[8]*r + m[9]*g + m[10]*b + m[14]);
       }
       else
       {
           float r = 1;
           float g = 1;
           float b = 1;
           ColorPoints.push_back(m[0]*r + m[1]*g + m[2]*b + m[12]);
           ColorPoints.push_back(m[4]*r + m[5]*g + m[6]*b + m[13]);
           ColorPoints.push_back(m[8]*r + m[9]*g + m[10]*b + m[14]);
       }
    }
    TotalPoints += model->TotalPoints;
	return TotalPoints;
}

int Model_XYZ::Clear() {

    TotalPoints = 0;
    Points.clear();
    ColorPoints.clear();
    hasColor = true;
	return 0;
}
