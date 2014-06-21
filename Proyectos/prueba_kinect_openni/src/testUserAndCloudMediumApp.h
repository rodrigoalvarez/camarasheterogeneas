#ifndef _TEST_APP_USER_AND_CLOUD
#define _TEST_APP_USER_AND_CLOUD

#include "ofxOpenNI.h"
#include "ofMain.h"

#define MAX_DEVICES 2

class testUserAndCloudMediumApp : public ofBaseApp{

public:

	void setup();
	void update();
	void draw();
    void exit();

	void keyPressed  (int key);
	void keyReleased(int key);
	void mouseMoved(int x, int y );
	void mouseDragged(int x, int y, int button);
	void mousePressed(int x, int y, int button);
	void mouseReleased(int x, int y, int button);
	void windowResized(int w, int h);

    int numDevices;
	ofxOpenNI openNIDevices[MAX_DEVICES];

    ofTrueTypeFont verdana;

    void userEvent(ofxOpenNIUserEvent & event);

};

#endif
