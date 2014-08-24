#pragma once

#include "ofxXmlSettings.h"
#include "ofMain.h"
#include <array>


struct DepthSettings {
	double Near;
	double Far;
	double PointsDownSample;
};

struct Camera {
	int Id;
	int ResolutionX;
	int ResolutionY;
	double ResolutionDownSample;
	int FPS;
	bool ColorRGB;
	bool Use2D;
	bool Use3D;
	double Matrix[16];
	string DataContext;
	DepthSettings DepthSettings;
};


class GlobalData {

	public:
		int     getTotal3D();
        int     getTotal2D();
		int     getTotalDevices();
        void    loadCalibData(char * xml);

        int      total3D;
        int      total2D;
        array<Camera,3> cameras;
		bool REAL_TIME;
		int REAL_TIME_FPS;
		int REAL_TIME_PORT;
};
