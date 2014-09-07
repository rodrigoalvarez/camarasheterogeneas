#pragma once

#include "GlobalData.h"
#include "ofxQTVideoSaver.h"
#include "ofMain.h"


class Thread2D : public ofThread {

	public:
		void threadedFunction();
		bool isDeviceInitted();
		bool isDataAllocated();

        int  device;
        ofImage img;
        int camWidth;
        int camHeight;
        bool isAllocated;
        ofVideoGrabber vidGrabber;
        t_camera  * context;
        t_data * sys_data;
        ofxQtVideoSaver video;
};
