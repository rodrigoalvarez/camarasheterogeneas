#pragma once

#include "ofMain.h"
#include "Thread2D.h"
#include "Thread3D.h"
#include "Transmitter.h"
#include "ThreadData.h"
#include "Constants.h"
#include "GlobalData.h"

class Grabber : public IGrabber {

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
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);
		void updateThreadData();

        Thread2D * t2D;
		Thread3D * t3D;
		GlobalData * gdata;
        Transmitter  transmitter;

        bool goLive;
};
