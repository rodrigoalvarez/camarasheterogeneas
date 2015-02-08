#pragma once

#include "ofMain.h"
#include "ofxOpenNI.h"
#include "ofxXmlSettings.h"

class testApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();
        testApp();
        ~testApp();
		void keyPressed  (int key);

        ofxOpenNI * openNIRecorder;
        int numDevices;
        int currDevices;
        ofVideoGrabber vidGrabber;

        bool deviceInited;
        bool readyToSave;
        ofShortPixels  spix;
        ofImage         img;
        int w, h;
        int step;
        bool keyP;
        int numRGBDevices;
        int currRGBDevice;
        ofTrueTypeFont myfont;

};
