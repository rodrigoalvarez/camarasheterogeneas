#pragma once

#include "ofMain.h"
#include "ofxQTVideoSaver.h"

class testApp : public ofBaseApp{
	
public:

    void setup();
    void update();
    void draw();
    void keyPressed(int key);
	void createFileName(void);
		
    ofVideoGrabber vidGrabber;
    unsigned char * videoInverted;
    ofTexture videoTexture;
    int camWidth;
    int camHeight;
	
	ofImage	mColorImage;
    ofxQtVideoSaver mVidSaver;
	string mFileName;
	float mTimestamp;

};
