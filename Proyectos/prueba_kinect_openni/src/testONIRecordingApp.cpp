#include "testONIRecordingApp.h"

class myUser : public ofxOpenNIUser {
public:
    void test(){
        cout << "test" << endl;
    }
};

//--------------------------------------------------------------
void testONIRecordingApp::setup() {


    ofSetLogLevel(OF_LOG_VERBOSE);

    openNIRecorder.setup();
    openNIRecorder.addDepthGenerator();
    openNIRecorder.addImageGenerator();
    openNIRecorder.setRegister(true);
    openNIRecorder.setMirror(true);
    openNIRecorder.addUserGenerator();
    openNIRecorder.setMaxNumUsers(2);
    openNIRecorder.start();

    openNIPlayer.setup();
	openNIPlayer.start();

    verdana.loadFont(ofToDataPath("ARLRDBD.ttf"), 24);
}

//--------------------------------------------------------------
void testONIRecordingApp::update(){
    openNIRecorder.update();
    openNIPlayer.update();
}

//--------------------------------------------------------------
void testONIRecordingApp::draw(){
	ofSetColor(255, 255, 255);

    ofPushMatrix();

    openNIRecorder.drawDebug(0, 0);
    openNIPlayer.drawDebug(0, 480);

    ofPushMatrix();

	ofSetColor(0, 255, 0);
	string msg = " MILLIS: " + ofToString(ofGetElapsedTimeMillis()) + " FPS: " + ofToString(ofGetFrameRate());
	verdana.drawString(msg, 20, 2 * 480 - 20);
}

//--------------------------------------------------------------
void testONIRecordingApp::userEvent(ofxOpenNIUserEvent & event){
    ofLogNotice() << getUserStatusAsString(event.userStatus) << "for user" << event.id << "from device" << event.deviceID;
}

//--------------------------------------------------------------
void testONIRecordingApp::gestureEvent(ofxOpenNIGestureEvent & event){
    ofLogNotice() << event.gestureName << getGestureStatusAsString(event.gestureStatus) << "from device" << event.deviceID << "at" << event.timestampMillis;
}

//--------------------------------------------------------------
void testONIRecordingApp::exit(){
    openNIRecorder.stop();
    openNIPlayer.stop();
}

//--------------------------------------------------------------
void testONIRecordingApp::keyPressed(int key){

    int cloudRes = -1;
    switch (key) {
        case ' ':
            if(!openNIRecorder.isRecording()){
                openNIRecorder.startRecording(ofToDataPath("test.oni"));
            }else{
                openNIRecorder.stopRecording();
            }
            break;
        case 'p':
            openNIPlayer.startPlayer("test.oni");
            break;
        case '/':
            openNIPlayer.setPaused(!openNIPlayer.isPaused());
            break;
        case 'm':
            openNIPlayer.firstFrame();
            break;
        case '<':
        case ',':
            openNIPlayer.previousFrame();
            break;
        case '>':
        case '.':
            openNIPlayer.nextFrame();
            break;
        case 'x':
            openNIRecorder.stop();
            openNIPlayer.stop();
            break;
        case 't':
            openNIRecorder.toggleRegister();
            break;
    }

}

//--------------------------------------------------------------
void testONIRecordingApp::keyReleased(int key){

}

//--------------------------------------------------------------
void testONIRecordingApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void testONIRecordingApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void testONIRecordingApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void testONIRecordingApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void testONIRecordingApp::windowResized(int w, int h){

}
