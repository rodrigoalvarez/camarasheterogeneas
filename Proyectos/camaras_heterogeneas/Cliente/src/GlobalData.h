#pragma once

#include "ofxXmlSettings.h"
#include "ofMain.h"

struct t_translation {
    int translationX;
    int translationY;
    int translationZ;
};

struct t_rotation {
    int rotation00;
    int rotation01;
    int rotation02;
    int rotation10;
    int rotation11;
    int rotation12;
    int rotation20;
    int rotation21;
    int rotation22;
};

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
    //String dataContext;
    t_translation * translation;
    t_rotation * rotation;
    t_camera * sig;
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
        bool     goLive;
        int      realtimeFPS;
        int      realtimePORT;
        t_camera * camera;

};
