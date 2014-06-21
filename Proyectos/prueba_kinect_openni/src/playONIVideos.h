#ifndef _PLAY_ONI_VIDEOS
#define _PLAY_ONI_VIDEOS

#include "ofxOpenNI.h"
#include "ofMain.h"
#include "of3dUtils.h"
#include "chONIData.h"

class playONIVideos : public ofBaseApp {
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
        int  numONIFiles;
        char buf[12];
        ofVbo vbo;
        chONIData * oniData;
        ofTrueTypeFont verdana;
        of3dUtils * utils3d;
        ofEasyCam   easy;
    protected:
    private:
};

#endif // RECORDONIVIDEOS_H
