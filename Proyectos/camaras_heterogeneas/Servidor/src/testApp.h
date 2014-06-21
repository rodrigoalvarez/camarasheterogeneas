#pragma once

#include "ofMain.h"
#include "ofxNetwork.h"
#include "ThreadData.h"
#include "FrameUtils.h"

struct t_translation {
    int translationX;
    int translationY;
    int translationZ;
};

struct t_rotation {
    int rotation00;
    int rotation01;
    int rotation02;
    int rotation10;
    int rotation11;
    int rotation12;
    int rotation20;
    int rotation21;
    int rotation22;
};

struct t_completeFrame {
    int totTrans;
    int totRot;
    t_translation * arrTranslation;
    t_rotation * arrRotation;
};

class testApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();

		void keyPressed  (int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);

		ofxUDPManager udpConnection;
		ofxTCPServer TCP;
        ThreadData * tData2;
};
