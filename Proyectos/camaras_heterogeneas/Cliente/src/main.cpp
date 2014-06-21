#include "ofMain.h"
#include "Grabber.h"
#include "Thread3D.h"
#include "Thread2D.h"
#include "Constants.h"
#include "ofAppGlutWindow.h"

//========================================================================
int main( ) {

    ofAppGlutWindow window;
	ofSetupOpenGL(&window, 1024,768, OF_WINDOW);			// <-------- setup the GL context

	// this kicks off the running of my app
	// can be OF_WINDOW or OF_FULLSCREEN
	// pass in width and height too:
	ofRunApp( new Grabber() );
}
