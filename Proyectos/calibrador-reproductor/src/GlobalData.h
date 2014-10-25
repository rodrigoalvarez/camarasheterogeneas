#pragma once

#include "ofxXmlSettings.h"
#include "ofMain.h"
//#include "Constants.h"
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

    ofVec4f row1;
    ofVec4f row2;
    ofVec4f row3;
    ofVec4f row4;

    ofVec4f imgrow1;
    ofVec4f imgrow2;
    ofVec4f imgrow3;
    ofVec4f imgrow4;

};


struct t_data {
    bool     goLive;
    bool     persistence;
    int      logLevel;
    int      cliId;
    int      cliPort;
    string   serverIp;
    int      serverPort;
    int      fps;
    int      maxPackageSize;
    int      nCamaras;
    t_camera    * camera;
};

class GlobalData {

	public:
		int     getFPS();
		bool    getGoLive();
		int     getTotal3D();
        int     getTotal2D();
		int     getTotalDevices();
        void    loadCalibData(char * xml);

        int      total3D;
        int      total2D;

        int nClientes;

        t_data      * sys_data;

};
