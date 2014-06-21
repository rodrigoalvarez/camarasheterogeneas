#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup() {

    ofSetLogLevel(OF_LOG_VERBOSE);

    openNIDevice.setup();
    openNIDevice.setLogLevel(OF_LOG_VERBOSE);
    openNIDevice.addDepthGenerator();
    openNIDevice.setUseDepthRawPixels(true);
    openNIDevice.addDepthThreshold(1, 2);
    ofVec2f *ctw = new ofVec2f(10,20);
    openNIDevice.cameraToWorld(*ctw);
    openNIDevice.start();

    verdana.loadFont(ofToDataPath("ARLRDBD.ttf"), 14);

    ofSetLogLevel(OF_LOG_VERBOSE);
    shadow.allocate(640, 480, OF_IMAGE_COLOR);
    //mesh = new ofMesh();
}

//--------------------------------------------------------------
void testApp::update() {

    openNIDevice.update();
    ofMesh mesh2;
    //mesh.clear();

    unsigned char *sdata    = shadow.getPixels();
    ofPixels data           = openNIDevice.getDepthPixels();
    ofPixels rpix           = data.getChannel(0);
    ofPixels gpix           = data.getChannel(1);
    ofPixels bpix           = data.getChannel(2);

    for (int k = 0; k < 640*480; k++) {
        if (data[k]) {
           // ofColor c = data[k]->getColor();
            ofVec3f v((int)rpix[k], (int)gpix[k], (int)bpix[k]);
            //printf("vertex %d, %d, %d", (int)rpix[k], (int)gpix[k], (int)bpix[k]);
            mesh2.addColor(ofFloatColor(155,155,155));
            mesh2.addVertex(v);
//            sdata[k*3]      = 0;
//            sdata[k*3+1]    = 0;
//            sdata[k*3+2]    = 100;
        } else {
//            sdata[k*3]      = 255;
//            sdata[k*3+1]    = 255;
//            sdata[k*3+2]    = 255;
        }
    }
    vbo.setMesh(mesh2, GL_STATIC_DRAW);

}

//--------------------------------------------------------------
void testApp::draw() {
    //shadow.draw(0, 0, 640, 480);
    ofBackground(30,10,30);
    ofSetColor(ofColor::blue);
    ofScale(100,100,100);
    vbo.draw(GL_QUADS,0,24);
    /*
    ofSetColor(255, 255, 255);

    //openNIDevice.drawDebug(); // draws all generators
    //openNIDevice.drawSkeletons();

    ofPixels pix = openNIDevice.getDepthPixels();

    // put some stuff in the pixels
    int i = 0;
    while( i < pix.size()) {
        char c = pix[i];
        i++;
    }

    ofSetColor(0, 255, 0);
	string msg = " MILLIS: " + ofToString(ofGetElapsedTimeMillis()) + " FPS: " + ofToString(ofGetFrameRate());
	verdana.drawString(msg, 20, ofGetHeight() - 26);*/
}

//--------------------------------------------------------------
void testApp::exit(){
    openNIDevice.stop();
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){

    switch (key) {
        case 'i':
            if (openNIDevice.isImageOn()){
                openNIDevice.removeImageGenerator();
                openNIDevice.addInfraGenerator();
                break;
            }
            if (openNIDevice.isInfraOn()){
                openNIDevice.removeInfraGenerator();
                openNIDevice.addImageGenerator();
                break;
            }
            break;
        default:
            break;
    }
}

//--------------------------------------------------------------
void testApp::keyReleased(int key){

}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h){

}

