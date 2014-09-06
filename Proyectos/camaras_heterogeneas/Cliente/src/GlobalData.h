#pragma once

#include "ofxXmlSettings.h"
#include "ofMain.h"
#include "ofMatrix4x4.h"

struct t_camera {
    int id;
    int resolutionX;
    int resolutionY;
    int resolutionDownSample;
    int fps;
    bool colorRGB;
    bool use2D;
    bool use3D;
    float near3D;
    float far3D;
    float points3DDownSample;
    ofMatrix4x4 matrix;
    ofVec3f xyz;
    ofVec3f abc;
    t_camera * sig;
};

class GlobalData {

	public:
		int     getTotal3D();
        int     getTotal2D();
		int     getTotalDevices();
        void    loadCalibData(char * xml);

        int      total3D;
        int      total2D;
		bool     goLive;
        int      cliId;
        int      port;
        int      realtimeFPS;
        int      realtimePORT;
        t_camera * camera;
};
