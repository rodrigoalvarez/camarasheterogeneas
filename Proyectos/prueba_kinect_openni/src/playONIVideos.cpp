#include "playONIVideos.h"

int cantImgs = 0;
int guardarNube = false;
int paso = false;
void playONIVideos::setup() {
    numVideo            = 2;
    numONIFiles         = 1;
    ofSetLogLevel(OF_LOG_VERBOSE);
    oniData             = new chONIData[numONIFiles];
    oniData[0].setup("onivideo_4.oni");
    //oniData[0].setup("test.oni");
    oniData[0].isOn     = true;
    oniData[0].playing  = false;
    oniData[0].color    = ofFloatColor(256, 0, 0);
    oniData[0].setPaused(true);
    utils3d             = new of3dUtils();
}

//--------------------------------------------------------------
void playONIVideos::update() {
    if (!paso){
        oniData[0].setPosition(0.2);
        oniData[0].setPaused(true);
        oniData[0].getPointCloud("cloudPosition021.xyz", 1);
        paso = true;
    }
    /*ofMesh mesh;
    for(int i=0; i<numONIFiles; i++) {
        oniData[i].update();
        if(oniData[i].isOn) {
            oniData[i].getMesh(mesh);
            ofImage img;
            ofPixels* pix       = oniData[i].getPixels();
            int downsampling = 1;
            if (guardarNube)
                oniData[i].getPointCloud("cloud2.xyz", downsampling);
            printf("%f\n",oniData[i].getPosition());
            unsigned char * p   = pix->getPixels();
            img.setFromPixels(p, pix->getWidth(), pix->getHeight(), OF_IMAGE_COLOR, true);
            if(cantImgs < 5) {
                cantImgs++;
                char buf[50];
                sprintf(buf, "imagen_%d.jpg", cantImgs); // puts string into buffer
                img.saveImage(ofToDataPath( buf ));
            }

        //oniData[i].getMeshLab("nube_A.xyz","nube_A_meshed.ply","ScriptProyecto.mlx");
        }
    }

    vbo.setMesh(mesh, GL_STATIC_DRAW);*/
}

//--------------------------------------------------------------
void playONIVideos::draw() {
    easy.begin();
    ofTranslate(20, 20);
	ofBackground(0,0,0);
    ofSetColor(255,0,0,127);
    ofScale(.7, .7, .7);
    //ofRotateY(180);
    ofRotateX(180);
    utils3d->ofDrawAxis(50);
    vbo.draw(GL_POINTS, 0,640*480*numONIFiles);
    oniData[0].draw();
    easy.end();

}

//--------------------------------------------------------------
void playONIVideos::exit() {
    for(int i = 0; i<numONIFiles; i++) {
        oniData[i].stop();
    }
    delete[] oniData;
}

//--------------------------------------------------------------
void playONIVideos::keyPressed(int key) {
    if (key == 'q'){
           guardarNube = !guardarNube;
    }
}

//--------------------------------------------------------------
void playONIVideos::keyReleased(int key){

}

//--------------------------------------------------------------
void playONIVideos::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void playONIVideos::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void playONIVideos::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void playONIVideos::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void playONIVideos::windowResized(int w, int h){

}
