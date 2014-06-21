#include "testMesh.h"

//--------------------------------------------------------------
void testMesh::setup() {

    openNIDevice.setup();
    openNIDevice.setResolution(640,480, 30);
    openNIDevice.addDepthGenerator();
    openNIDevice.addImageGenerator();
    openNIDevice.setUseDepthRawPixels(true);
    //openNIDevice.setRegister(true);
    //openNIDevice.setMirror(true);
    openNIDevice.start();

    ofBackground(0);
    glEnable(GL_DEPTH_TEST);
    ofSetLogLevel(OF_LOG_ERROR);

}

//--------------------------------------------------------------
void testMesh::update() {

    openNIDevice.update();
    ofMesh mesh;

    ofShortPixels& pixels   = openNIDevice.getDepthRawPixels();
    XnDepthPixel* rawPix    = pixels.getPixels();

    ofPixels cpixels        = openNIDevice.getImagePixels();
    ofPixels rpix           = cpixels.getChannel(0);
    ofPixels gpix           = cpixels.getChannel(1);
    ofPixels bpix           = cpixels.getChannel(2);

    int step = 1;
    for(int y=0; y < 480; y += step) {
        for(int x=0; x < 640; x += step) {
            float d = rawPix[y * 640 + x];
            //glVertex3f(x, y, (float)d/10.0);
            //mesh.addColor(ofFloatColor(rpix[480*y + x],gpix[480*y + x],bpix[480*y + x]));
            //mesh.addColor(ofFloatColor(50,50,50));
            mesh.addVertex(ofVec3f( x, y, (float)d));
            mesh.addColor(ofFloatColor(rpix[y * 640 + x],gpix[y * 640 + x],bpix[y * 640 + x]));
        }
    }


    vbo.setMesh(mesh, GL_STATIC_DRAW);
}

//--------------------------------------------------------------
void testMesh::draw() {
    camera.begin();
    ofScale(.5,.5,.5);
    vbo.draw(GL_POINTS, 0,640*480);

}

//--------------------------------------------------------------
void testMesh::exit(){

}

//--------------------------------------------------------------
void testMesh::keyPressed(int key){
}

//--------------------------------------------------------------
void testMesh::keyReleased(int key){

}

//--------------------------------------------------------------
void testMesh::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void testMesh::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void testMesh::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void testMesh::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void testMesh::windowResized(int w, int h){

}

