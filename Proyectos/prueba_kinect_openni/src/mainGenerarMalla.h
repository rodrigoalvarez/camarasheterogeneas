#ifndef MAINGENERARMALLA_H_INCLUDED
#define MAINGENERARMALLA_H_INCLUDED

#include "ofxOpenNI.h"
#include "ofMain.h"
#include "of3dUtils.h"
#include "chONIData.h"
#include "ThreadData.h"
#include "FrameUtils.h"

class mainGenerarMalla : public ofBaseApp {
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

#endif // MAINGENERARMALLA_H_INCLUDED
