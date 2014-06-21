#include "meshFromONIVideo.h"

#include "ofxSimpleGuiToo.h"



float rotx;
float roty;
float rotz;
int toggle = 0;

void meshFromONIVideo::setup() {
    numONIFiles         = 3;
    oniData             = new chONIData[numONIFiles];

//    oniData[0].oniName  = "onivideo_2.oni";
//    oniData[1].oniName  = "onivideo_3.oni";
//    oniData[2].oniName  = "onivideo_4.oni";
    oniData[0].setup("oni/onivideo_2.oni");
    oniData[1].setup("oni/onivideo_3.oni");
    oniData[2].setup("oni/onivideo_4.oni");
    ofSetLogLevel(OF_LOG_VERBOSE);

    //verdana.loadFont(ofToDataPath("ARIAL.ttf"), 24);

    gui.addTitle("GENERAL VIEW");

    gui.addSlider("rot - x:",   gvrotx,     0, 360);
    gui.addSlider("rot - y:",   gvroty,     0, 360);
    gui.addSlider("rot - z:",   gvrotz,     0, 360);
    gui.addSlider("trans - x:", gvtransx,   0, 1000);
    gui.addSlider("trans - y:", gvtransy,   0, 1000);

    gui.addTitle("ONI File 1");
    gui.addSlider("f1_rot_x:",       oniData[0].rotX,   0, 360);
    gui.addSlider("f1_rot_y:",       oniData[0].rotY,   0, 360);
    gui.addSlider("f1_rot_z:",       oniData[0].rotZ,   0, 360);
    gui.addSlider("f1_trans_x:",     oniData[0].transX, -2000, 2000);
    gui.addSlider("f1_trans_y:",     oniData[0].transY, -2000, 2000);
    gui.addSlider("f1_trans_z:",     oniData[0].transZ, -2000, 2000);
    gui.addToggle("f1_playing:",     oniData[0].playing);
    gui.addToggle("f1_isOn:",        oniData[0].isOn);
    gui.addColorPicker("f1_Pix color", oniData[0].color);

    gui.addTitle("ONI File 2");
    gui.addSlider("f2_rot_x:",       oniData[1].rotX,   0, 360);
    gui.addSlider("f2_rot_y:",       oniData[1].rotY,   0, 360);
    gui.addSlider("f2_rot_z:",       oniData[1].rotZ,   0, 360);
    gui.addSlider("f2_trans_x:",     oniData[1].transX, -2000, 2000);
    gui.addSlider("f2_trans_y:",     oniData[1].transY, -2000, 2000);
    gui.addSlider("f2_trans_z:",     oniData[1].transZ, -2000, 2000);
    gui.addToggle("f2_playing:",       oniData[1].playing);
    gui.addToggle("f2_isOn:",        oniData[1].isOn);
    gui.addColorPicker("f2_Pix color", oniData[1].color);

    gui.addTitle("ONI File 3");
    gui.addSlider("f3_rot_x:",       oniData[2].rotX,   0, 360);
    gui.addSlider("f3_rot_y:",       oniData[2].rotY,   0, 360);
    gui.addSlider("f3_rot_z:",       oniData[2].rotZ,   0, 360);
    gui.addSlider("f3_rot_x:",       oniData[2].transX, -2000, 2000);
    gui.addSlider("f3_rot_y:",       oniData[2].transY, -2000, 2000);
    gui.addSlider("f3_rot_z:",       oniData[2].transZ, -2000, 2000);
    gui.addToggle("f3_playing:",       oniData[2].playing);
    gui.addToggle("f3_isOn:",            oniData[2].isOn);
    gui.addColorPicker("f3 - Pix color", oniData[2].color);

    gui.addFPSCounter();

    gui.loadFromXML();

    gui.show();

    utils3d = new of3dUtils();

    oniData[0].playing = true;
    oniData[1].playing = true;
    oniData[2].playing = true;
}

//--------------------------------------------------------------
void meshFromONIVideo::update() {
    ofMesh mesh;
    for(int i=0; i<numONIFiles; i++) {
        oniData[i].update();
        if(oniData[i].isOn) {
            oniData[i].getMesh(mesh);
        }
    }

    vbo.setMesh(mesh, GL_STATIC_DRAW);
}

//--------------------------------------------------------------
void meshFromONIVideo::draw() {

    ofBackground(0,0,0);
    gui.draw();

    //Muevo el la visión global
    ofTranslate(gvtransx, gvtransy);
    ofRotateX(gvrotx);
    ofRotateY(gvroty);
    ofRotateZ(gvrotz);

    //Dibujo el eje 3D
    ofSetColor(255,0,0,127);
//    utils3d->ofDrawArrow(ofVec3f(0,0,0), ofVec3f(0,0,50), 10);
//    ofSetColor(0,255,0,127);
//    utils3d->ofDrawArrow(ofVec3f(0,0,0), ofVec3f(0,50,0), 10);
//    ofSetColor(0,0,255,127);
//    utils3d->ofDrawArrow(ofVec3f(0,0,0), ofVec3f(50,0,0), 10);
    utils3d->ofDrawAxis(50);
    utils3d->ofDrawGridPlane(300, 5, true);
    ofScale(.2, .2, .2);
    //Dibujo el mesh
    vbo.draw(GL_POINTS, 0,640*480*numONIFiles);

}

//--------------------------------------------------------------
void meshFromONIVideo::exit(){
    for(int i = 0; i<numONIFiles; i++) {
        oniData[i].stop();
    }
    delete[] oniData;
}

//--------------------------------------------------------------
void meshFromONIVideo::keyPressed(int key){

   switch (key) {
        case 'p':
            if(gui.isOn()) {
                gui.hide();
            } else {
                gui.show();
            }
            break;

    }

}

//--------------------------------------------------------------
void meshFromONIVideo::keyReleased(int key){

}

//--------------------------------------------------------------
void meshFromONIVideo::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void meshFromONIVideo::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void meshFromONIVideo::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void meshFromONIVideo::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void meshFromONIVideo::windowResized(int w, int h){

}
