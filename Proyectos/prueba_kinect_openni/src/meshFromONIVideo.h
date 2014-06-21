#ifndef _MESH_FROM_ONI_VIDEOS
#define _MESH_FROM_ONI_VIDEOS

#include "ofxOpenNI.h"
#include "ofMain.h"
#include "of3dUtils.h"
#include "chONIData.h"

class meshFromONIVideo : public ofBaseApp {
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
        char * nomVideo;
        char buf[12];
        ofVbo vbo;
        ofxOpenNI openNIPlayer;
        bool playStarted;
        ofTrueTypeFont verdana;

        float gvrotx;
        float gvroty;
        float gvrotz;
        float gvtransx;
        float gvtransy;
        int numONIFiles;
        chONIData * oniData;
        of3dUtils * utils3d;

    protected:
    private:
};

#endif // RECORDONIVIDEOS_H
