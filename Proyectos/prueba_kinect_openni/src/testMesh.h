#ifndef _TEST_MESH_APP
#define _TEST_MESH_APP

#include "ofxOpenNI.h"
#include "ofMain.h"

class testMesh : public ofBaseApp{

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

    ofTrueTypeFont verdana;

    ofVbo vbo;
    ofEasyCam camera;
    ofxOpenNI openNIDevice;
};

#endif
