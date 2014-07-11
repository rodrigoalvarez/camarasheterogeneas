#pragma once

#include "ofMain.h"
#include "ofxUI.h"

class ofApp : public ofBaseApp{

	static const int PLAYER_CANT = 10;

	public:
		void setup();
		void update();
		void draw();

		void keyPressed(int key);
		void keyReleased(int key);
		
		ofxUISuperCanvas *gui;
		void guiEvent(ofxUIEventArgs &e);

		ofVideoPlayer movPlayer[PLAYER_CANT];
		int movCant;
		ofVideoPlayer oniPlayer[PLAYER_CANT];
		int oniCant;
		
};
