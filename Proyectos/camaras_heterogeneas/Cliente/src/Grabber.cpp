#include "Grabber.h"
#include "Thread2D.h"



//--------------------------------------------------------------
void Grabber::setup() {



    gdata       = new GlobalData();
    gdata->loadCalibData("settings.xml");

    total2D     = gdata->total2D;    //Hacer que se cargue dinámico.
    total3D     = gdata->total3D;    //Hacer que se cargue dinámico.
    goLive      = gdata->goLive;     //Hacer que se cargue dinámico.


    if((gdata->total2D + gdata->total3D) > 0) {
        tData = new ThreadData[gdata->total2D + gdata->total3D];
    } else {
        tData = NULL;
    }

    printf("total2D, %d \n", gdata->total2D);

    t_camera * camera = gdata->camera;

    if(gdata->total2D > 0) {
        t2D   = new Thread2D[gdata->total2D];
    }

    if(gdata->total3D > 0) {
        t3D   = new Thread3D[gdata->total3D];
    }

    int i2D = 0;
    int i3D = 0;

    while(camera != NULL) {
        if(camera->use3D == 1) {
            t3D[i3D].context                    = camera;
            t3D[i3D].startThread(true, false);
            i3D ++;
        } else {
            t2D[i2D].context                    = camera;
            t2D[i2D].startThread(true, false);
            i2D ++;
        }
        camera = camera->sig;
    }

    if(goLive) { //Si desde el Calibrator se indicó que se debe trasmitir en vivo.
        transmitter.grabber = this;
        transmitter.startThread(true, false);
    }
}

//--------------------------------------------------------------
void Grabber::update() {

}

//--------------------------------------------------------------
void Grabber::draw() {

}

void Grabber::exit() {
    //Stop the thread
    int i       = 0;
    for(i; i<gdata->total2D; i++) {
        t2D[i].stopThread();
    }

    i = 0;
    for(i; i<gdata->total3D; i++) {
        t3D[i].stopThread();
    }

    if(goLive) {
        transmitter.stopThread();
    }

    if(t2D != NULL) {
        delete t2D;
    }

    if(t3D != NULL) {
        delete t3D;
    }

    if(tData != NULL) {
        delete tData;
    }

    if(gdata != NULL) {
        delete gdata;
    }
}

//Operación invocada por Transmitter para refrezcar la información a trasmitir.
void Grabber::updateThreadData() {
    return;
    int di      = 0;
    int i       = 0;

    /**
    * ACTUALIZO LA INFO DE LAS CÁMARAS 2D
    */
    for(i; i<gdata->total2D; i++) {
        t2D[i].lock();
        tData[di].inited    = 0;
        if(t2D[i].isDeviceInitted() && t2D[i].isDataAllocated()) { //Si la cámara está inicializada.
            tData[di].type   = DEVICE_2D; // 2D
            tData[di].inited = 1; // true
            if(t2D[i].context->colorRGB == 1) {
                tData[di].img.setFromPixels(t2D[i].img.getPixels(), t2D[i].img.width, t2D[i].img.height, OF_IMAGE_COLOR, true); //Clono la imágen
            } else {
                tData[di].img.setFromPixels(t2D[i].img.getPixels(), t2D[i].img.width, t2D[i].img.height, OF_IMAGE_GRAYSCALE, true); //Clono la imágen
            }
        }
        t2D[i].unlock();
        di++;
    }

    /**
    * ACTUALIZO LA INFO DE LAS CÁMARAS 3D
    */
    i = 0;
    for(i; i<gdata->total3D; i++) {
        t3D[i].lock();
        tData[di].inited    = 0;

        if(t3D[i].isDeviceInitted()) { //Si la cámara está inicializada.
            tData[di].type   = DEVICE_3D; // 2D
            tData[di].inited = 1; // true
            t3D[i].updateData();

            if(t2D[i].context->use2D == 1) {
                //Clono la imágen
                if(t2D[i].context->colorRGB == 1) {
                    tData[di].img.setFromPixels(t3D[i].img.getPixels(), t3D[i].img.width, t3D[i].img.height, OF_IMAGE_COLOR, true);
                } else {
                    tData[di].img.setFromPixels(t3D[i].img.getPixels(), t3D[i].img.width, t3D[i].img.height, OF_IMAGE_GRAYSCALE, true);
                }
            }

            if(t2D[i].context->use3D == 1) {
                //Hacer que esta nube de puntos, cuando tela de, ya te la de transformada.
                tData[di].spix   = t3D[i].spix;
            }
        }
        t3D[i].unlock();
        di++;
    }
}

//--------------------------------------------------------------
void Grabber::keyPressed(int key) {
    exit();
}

//--------------------------------------------------------------
void Grabber::keyReleased(int key) {

}

//--------------------------------------------------------------
void Grabber::mouseMoved(int x, int y ) {

}

//--------------------------------------------------------------
void Grabber::mouseDragged(int x, int y, int button) {

}

//--------------------------------------------------------------
void Grabber::mousePressed(int x, int y, int button) {

}

//--------------------------------------------------------------
void Grabber::mouseReleased(int x, int y, int button) {

}

//--------------------------------------------------------------
void Grabber::windowResized(int w, int h) {

}

//--------------------------------------------------------------
void Grabber::gotMessage(ofMessage msg) {

}

//--------------------------------------------------------------
void Grabber::dragEvent(ofDragInfo dragInfo) {

}
