#pragma once

#include "GlobalData.h"
#include "ofxQTVideoSaver.h"
#include "ofMain.h"


class Thread2D : public ofThread {

	public:
		void threadedFunction();
		void process(ofEventArgs &e);
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
        bool started;
        bool idle;
        float mTimestamp;
        bool first;
        int snapCounter;

        Thread2D() {
            idle = true;
            started = false;
        }

        ~Thread2D() {
            if(!started) return;

            ofRemoveListener(ofEvents().update, this, &Thread2D::process);
            if(sys_data->goLive == 1) {
                img.clear();
            }

            if(sys_data->persistence == 1) {
                video.finishMovie();
            }
        }
};
