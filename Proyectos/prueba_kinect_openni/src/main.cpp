#include "ofMain.h"

#include "testApp.h"
#include "testMesh.h"
//#include "testUserAndCloudMediumApp.h"
//#include "testHandTrackingApp.h"
//#include "testONIRecordingApp.h"
#include "recordONIVideos.h"
#include "playONIVideos.h"
//#include "meshFromONIVideo.h"
#include "mainGenerarMalla.h"
#include "ofAppGlutWindow.h"

//========================================================================
int main( ) {

    ofAppGlutWindow window;
	ofSetupOpenGL(&window, 1024,768, OF_WINDOW);			// <-------- setup the GL context

	// this kicks off the running of my app
	// can be OF_WINDOW or OF_FULLSCREEN
	// pass in width and height too:
	//ofRunApp( new testApp());
	//ofRunApp( new testMesh());
    //ofRunApp( new testUserAndCloudMediumApp());
    //ofRunApp( new testHandTrackingApp());
    //ofRunApp( new testONIRecordingApp());
    //ofRunApp( new recordONIVideos());
    //ofRunApp( new playONIVideos());
    ofRunApp( new mainGenerarMalla());
    //ofRunApp( new meshFromONIVideo() );

}
