#include "ofMain.h"
#include "testApp.h"
#include "ofAppGlutWindow.h"


int main() {
	
    ofAppGlutWindow window;
	ofSetupOpenGL(&window, 800, 480, OF_WINDOW);
	ofRunApp( new testApp());

}