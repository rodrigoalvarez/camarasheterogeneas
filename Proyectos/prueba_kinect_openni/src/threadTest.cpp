#include "recordONIVideos.h"

void recordONIVideos::setup() {
    numVideo = 1;
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

    verdana.loadFont(ofToDataPath("ARIAL.ttf"), 24);
}

//--------------------------------------------------------------
void recordONIVideos::update(){
    openNIRecorder.update();
    openNIPlayer.update();
}

//--------------------------------------------------------------
void recordONIVideos::draw(){
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
/*void recordONIVideos::userEvent(ofxOpenNIUserEvent & event){
    ofLogNotice() << getUserStatusAsString(event.userStatus) << "for user" << event.id << "from device" << event.deviceID;
}*/

//--------------------------------------------------------------
/*void recordONIVideos::gestureEvent(ofxOpenNIGestureEvent & event){
    ofLogNotice() << event.gestureName << getGestureStatusAsString(event.gestureStatus) << "from device" << event.deviceID << "at" << event.timestampMillis;
}*/

//--------------------------------------------------------------
void recordONIVideos::exit(){
    openNIRecorder.stop();
    openNIPlayer.stop();
}

//--------------------------------------------------------------
void recordONIVideos::keyPressed(int key){

    switch (key) {
        case ' ':
            if(!openNIRecorder.isRecording()) {
                numVideo ++;
                sprintf(buf, "onivideo_%d.oni", numVideo); // puts string into buffer
                openNIRecorder.startRecording( ofToDataPath( buf ) );
            } else {
                openNIRecorder.stopRecording();
            }
            break;
        case 'p':
            sprintf(buf, "onivideo_%d.oni", numVideo); // puts string into buffer
            openNIPlayer.startPlayer(buf);
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
void recordONIVideos::keyReleased(int key){

}

//--------------------------------------------------------------
void recordONIVideos::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void recordONIVideos::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void recordONIVideos::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void recordONIVideos::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void recordONIVideos::windowResized(int w, int h){

}
