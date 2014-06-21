#include "testApp.h"
#include <string>

enum ProcessState { READY, RECORDING, ENDING };
ProcessState state = ProcessState::READY;
bool first = true;


void testApp::setup(){
	
	camWidth = 640;
	camHeight = 480;
	
	vidGrabber.setVerbose(true);
	vidGrabber.initGrabber(camWidth,camHeight);
	
	videoInverted 	= new unsigned char[camWidth*camHeight*3];
	videoTexture.allocate(camWidth,camHeight, GL_RGB);
	
	createFileName();
	mTimestamp = ofGetElapsedTimef();
	
    mVidSaver.setCodecQualityLevel(OF_QT_SAVER_CODEC_QUALITY_NORMAL);
    mVidSaver.setup(camWidth, camHeight, mFileName);

}


void testApp::update(){
	
	switch(state) {
		case RECORDING :

			ofBackground(100,100,100);

			vidGrabber.update();
	
			if (vidGrabber.isFrameNew()){

				float time = ofGetElapsedTimef() - mTimestamp;
				mVidSaver.addFrame(vidGrabber.getPixels(), first ? 0 : time);
				mTimestamp = ofGetElapsedTimef();
				first = false;
			}

			break;
	}

}


void testApp::draw(){

	switch(state) {
		case READY :

			ofSetHexColor(0xffffff);
	
			ofDrawBitmapString("Press Enter to start the record.", 0, 20);
			ofDrawBitmapString(mFileName + " will appear in your data folder.", 0, 40);

			ofPopMatrix();
			ofPopMatrix();

			break;
		case RECORDING :

			ofSetHexColor(0xffffff);
			vidGrabber.draw(0,0);
	
			ofDrawBitmapString("fps: " + ofToString(ofGetFrameRate()), 0, 20);
			ofDrawBitmapString("Press Enter to stop the record.", 0, 40);
			ofDrawBitmapString(mFileName + " will appear in your data folder.", 0, 60);

			ofPopMatrix();
			ofPopMatrix();

			break;
	}

}


void testApp::keyPressed  (int key){ 
	
	cout << "keyPressed " << key << endl;

	if (key == 13) {
		switch(state) {
			case READY :
				state = ProcessState::RECORDING;
				cout << "RECORDING" << endl;
				break;
			case RECORDING :
				state = ProcessState::ENDING;
				mVidSaver.finishMovie();
				cout << "ENDING" << endl;
				break;
		}
	}
	
}


void testApp::createFileName(void)
{
	ostringstream oss;
	oss << ofGetYear() << "-";
	oss << setw(2) << setfill('0') << ofGetMonth() << "-";
	oss << setw(2) << setfill('0') << ofGetDay() << "-";
	oss << setw(2) << setfill('0') << ofGetHours() << "-";
	oss << setw(2) << setfill('0') << ofGetMinutes() << "-";
	oss << setw(2) << setfill('0') << ofGetSeconds() << ".mov";
	mFileName = oss.str();	
}
