#ifndef _RECORD_ONI_VIDEOS
#define _RECORD_ONI_VIDEOS

#include "ofxOpenNI.h"
#include "ofMain.h"


class recordONIVideos : public ofBaseApp {
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
        int  numVideo;
        char buf[12];

        ofxOpenNI openNIRecorder;
        ofxOpenNI openNIPlayer;

        ofTrueTypeFont verdana;

//        void userEvent(ofxOpenNIUserEvent & event);
//        void gestureEvent(ofxOpenNIGestureEvent & event);

    protected:
    private:
};

#endif // RECORDONIVIDEOS_H
