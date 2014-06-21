#ifndef _TEST_APP_HAND_TRACKING
#define _TEST_APP_HAND_TRACKING

#include "ofxOpenNI.h"
#include "ofMain.h"

#define MAX_DEVICES 2

class testHandTrackingApp : public ofBaseApp{

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

private:

    void handEvent(ofxOpenNIHandEvent & event);

	ofxOpenNI openNIDevice;
    ofTrueTypeFont verdana;

};

#endif
