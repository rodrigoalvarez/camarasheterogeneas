#include "Grabber.h"
#include "Thread2D.h"

#include "ofxSimpleGuiToo.h"

//--------------------------------------------------------------
void Grabber::setup() {

    gdata       = new GlobalData();
    gdata->loadCalibData("settings.xml");
    ofLogLevel(OF_LOG_VERBOSE);
    //ofLogToFile("client_log.txt", false);

    setupGui();
    downsampling = 4;
    switch(gdata->sys_data->logLevel) {
        case 0: ofSetLogLevel(OF_LOG_VERBOSE); break;
        case 1: ofSetLogLevel(OF_LOG_NOTICE); break;
        case 2: ofSetLogLevel(OF_LOG_WARNING); break;
        case 3: ofSetLogLevel(OF_LOG_ERROR); break;
        case 4: ofSetLogLevel(OF_LOG_FATAL_ERROR); break;
        case 5: ofSetLogLevel(OF_LOG_SILENT); break;
        default:ofSetLogLevel(OF_LOG_VERBOSE); break;
    }

    FrameUtils::init();

    ofSetFrameRate(gdata->sys_data->fps);

    total2D     = gdata->total2D;    //Hacer que se cargue dinámico.
    total3D     = gdata->total3D;    //Hacer que se cargue dinámico.

    //ofSetFrameRate(gdata->sys_data->fps);

    if((gdata->total2D + gdata->total3D) > 0) {
        tData = new ThreadData[gdata->total2D + gdata->total3D];
        for(int w = 0; w < (gdata->total2D + gdata->total3D); w++) {
            tData[w].cliId = gdata->sys_data->cliId;
        }
    } else {
        tData = NULL;
    }

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
            t3D[i3D].sys_data                   = gdata->sys_data;
            t3D[i3D].context                    = camera;
            t3D[i3D].startThread(true, false);
            i3D ++;
        } else {
            t2D[i2D].sys_data                   = gdata->sys_data;
            t2D[i2D].context                    = camera;
            t2D[i2D].startThread(true, true);
            i2D ++;
        }
        camera = camera->sig;
    }
    ofLogVerbose() << "[Grabber::setup] " << gdata->sys_data->goLive;
    if(gdata->sys_data->goLive == 1) { //Si desde el Calibrator se indicó que se debe trasmitir en vivo.
        transmitter.grabber     = this;
        transmitter.sys_data    = gdata->sys_data;
        transmitter.startThread(true, true);
    }
    ofVideoGrabber vid;
    vid.listDevices();
}

void Grabber::setupGui() {
    gui.addFPSCounter();
    gui.show();
}

//--------------------------------------------------------------
void Grabber::update() {

}

//--------------------------------------------------------------
void Grabber::draw() {
    if(gui.isOn()) {
        gui.draw();
    }
}

void Grabber::exit() {
    //Stop the thread
    int i       = 0;
    for(i; i<gdata->total2D; i++) {
        //t2D[i].stopThread();
        if(t2D[i].isThreadRunning()) {
            t2D[i].exit();
            t2D[i].waitForThread();
        }
    }

    i = 0;
    for(i; i<gdata->total3D; i++) {
        //t3D[i].stopThread();
        if(t3D[i].isThreadRunning()) {
            t3D[i].exit();
            t3D[i].waitForThread();
        }
    }

    if(gdata->sys_data->goLive == 1) {
        if(transmitter.isThreadRunning()) {
            transmitter.exit();
            transmitter.waitForThread();
            //transmitter.stopThread();
        }
    }

    ofSleepMillis(1500);

    if(t2D != NULL) {
        delete t2D;
    }

    if(t3D != NULL) {
        delete t3D;
    }

    if(tData != NULL) {
        i = 0;
        for(i; i<gdata->total2D + gdata->total3D; i++) {
            if(tData[i].xpix != NULL) {
                delete tData[i].xpix;
                delete tData[i].ypix;
                delete tData[i].zpix;
            }
        }
        delete tData;
    }

    if(gdata != NULL) {
        delete gdata;
    }
}


//xn::DepthGenerator& Xn_depth;

//Operación invocada por Transmitter para refrezcar la información a trasmitir.
void Grabber::updateThreadData() {
    ofLogVerbose() << "[Grabber::updateThreadData] " << " entrando. " << gdata->total2D;
    int di      = 0;
    int i       = 0;

    /**
    * ACTUALIZO LA INFO DE LAS CÁMARAS 2D
    */
    for(i; i<gdata->total2D; i++) {
        t2D[i].lock();
        tData[di].camId         = t2D[i].context->id;
        tData[di].state         = 0;
        tData[di].cameraType    = 1;
        ofLogVerbose() << "[Grabber::updateThreadData] " << " if " << t2D[i].isDeviceInitted() << " " << t2D[i].isDataAllocated() << " " << t2D[i].context->id;
        if(t2D[i].isDeviceInitted() && t2D[i].isDataAllocated()) { //Si la cámara está inicializada.
            tData[di].state  = DEVICE_2D; // 2D
            tData[di].img.clear();
            tData[di].compressed    = t2D[i].context->useCompression;
            tData[di].img.setFromPixels(t2D[i].img.getPixels(), t2D[i].img.getWidth(), t2D[i].img.getHeight(), OF_IMAGE_COLOR, true);
            tData[di].qfactor       = t2D[i].context->rgbCompressionQuality;

            int imgW                = (int)t2D[i].img.getWidth()*t2D[i].context->resolutionDownSample;
            int imgH                = (int)t2D[i].img.getHeight()*t2D[i].context->resolutionDownSample;
            tData[di].imgWidth      = imgW;
            tData[di].imgHeight     = imgH;
            tData[di].img.resize(imgW, imgH);

            tData[di].imgrow1.set(t2D[i].context->imgrow1.x, t2D[i].context->imgrow1.y, t2D[i].context->imgrow1.z, t2D[i].context->imgrow1.w);
            tData[di].imgrow2.set(t2D[i].context->imgrow2.x, t2D[i].context->imgrow2.y, t2D[i].context->imgrow2.z, t2D[i].context->imgrow2.w);
            tData[di].imgrow3.set(t2D[i].context->imgrow3.x, t2D[i].context->imgrow3.y, t2D[i].context->imgrow3.z, t2D[i].context->imgrow3.w);
            tData[di].imgrow4.set(t2D[i].context->imgrow4.x, t2D[i].context->imgrow4.y, t2D[i].context->imgrow4.z, t2D[i].context->imgrow4.w);

            ofLogVerbose() << "[Grabber::updateThreadData] 2D - imgrow1.x: " << tData[di].imgrow1.x << ", imgrow1.y: " << tData[di].imgrow1.y << ", imgrow1.z: " << tData[di].imgrow1.z << ", imgrow1.w: " << tData[di].imgrow1.w;
            ofLogVerbose() << "[Grabber::updateThreadData] 2D - imgrow2.x: " << tData[di].imgrow2.x << ", imgrow2.y: " << tData[di].imgrow2.y << ", imgrow2.z: " << tData[di].imgrow2.z << ", imgrow2.w: " << tData[di].imgrow2.w;
            ofLogVerbose() << "[Grabber::updateThreadData] 2D - imgrow3.x: " << tData[di].imgrow3.x << ", imgrow3.y: " << tData[di].imgrow3.y << ", imgrow3.z: " << tData[di].imgrow3.z << ", imgrow3.w: " << tData[di].imgrow3.w;
            ofLogVerbose() << "[Grabber::updateThreadData] 2D - imgrow4.x: " << tData[di].imgrow4.x << ", imgrow4.y: " << tData[di].imgrow4.y << ", imgrow4.z: " << tData[di].imgrow4.z << ", imgrow4.w: " << tData[di].imgrow4.w;
        }
        tData[di].nubeW = tData[di].nubeH = 0;
        gettimeofday(&tData[di].curTime, NULL);
        t2D[i].unlock();
        di++;
    }

    /**
    * ACTUALIZO LA INFO DE LAS CÁMARAS 3D
    */
    ofLogVerbose() << "ACTUALIZO LA INFO DE LAS CÁMARAS 3D" << endl;
    i = 0;
    for(i; i<gdata->total3D; i++) {
        t3D[i].lock();
        tData[di].state         = 0;
        tData[di].camId         = t3D[i].context->id;
        tData[di].cameraType    = 2;

        if(t3D[i].isDeviceInitted()) { //Si la cámara está inicializada.

            if(t3D[i].context->use2D == 1) {
                tData[di].state    = DEVICE_2D;
                //Clono la imágen
                tData[di].compressed    = t3D[i].context->useCompression;
                tData[di].img.setFromPixels(t3D[i].img.getPixels(), t3D[i].img.getWidth(), t3D[i].img.getHeight(), OF_IMAGE_COLOR, true);

                int imgW                = (int)t3D[i].img.getWidth()*t3D[i].context->resolutionDownSample;
                int imgH                = (int)t3D[i].img.getHeight()*t3D[i].context->resolutionDownSample;
                tData[di].imgWidth      = imgW;
                tData[di].imgHeight     = imgH;

                tData[di].img.resize(imgW, imgH);
                tData[di].qfactor       = t3D[i].context->rgbCompressionQuality;

                tData[di].nubeW = tData[di].nubeH = 0;
            }

            if(t3D[i].context->use3D == 1) {
                ((t3D[i].context->use2D == 1) ? tData[di].state = DEVICE_2D_3D : tData[di].state = DEVICE_3D);
                //Hacer que esta nube de puntos, cuando tela de, ya te la de transformada.

                tData[di].nubeW = t3D[i].spix.getWidth();
                tData[di].nubeH = t3D[i].spix.getHeight();

                rawPix          = t3D[i].spix.getPixels();
                Xn_depth        = &t3D[i].openNIRecorder->getDepthGenerator();

                y   = 0;
                x   = 0;
                d   = 0;
                tData[di].nubeLength    = 0;

                tmpX = NULL;
                tmpY = NULL;
                tmpZ = NULL;

                if((tData[di].nubeW > 0) && (tData[di].nubeH > 0)) {
                    tmpX = new float[tData[di].nubeW * tData[di].nubeH];
                    tmpY = new float[tData[di].nubeW * tData[di].nubeH];
                    tmpZ = new float[tData[di].nubeW * tData[di].nubeH];
                }

                tData[di].row1.set(t3D[i].context->row1.x, t3D[i].context->row1.y, t3D[i].context->row1.z, t3D[i].context->row1.w);
                tData[di].row2.set(t3D[i].context->row2.x, t3D[i].context->row2.y, t3D[i].context->row2.z, t3D[i].context->row2.w);
                tData[di].row3.set(t3D[i].context->row3.x, t3D[i].context->row3.y, t3D[i].context->row3.z, t3D[i].context->row3.w);
                tData[di].row4.set(t3D[i].context->row4.x, t3D[i].context->row4.y, t3D[i].context->row4.z, t3D[i].context->row4.w);

                matrix.set( t3D[i].context->row1.x, t3D[i].context->row1.y, t3D[i].context->row1.z, t3D[i].context->row1.w,
                            t3D[i].context->row2.x, t3D[i].context->row2.y, t3D[i].context->row2.z, t3D[i].context->row2.w,
                            t3D[i].context->row3.x, t3D[i].context->row3.y, t3D[i].context->row3.z, t3D[i].context->row3.w,
                            t3D[i].context->row4.x, t3D[i].context->row4.y, t3D[i].context->row4.z, t3D[i].context->row4.w);

                ofLogVerbose() << matrix << endl;

                for(y=0; y < tData[di].nubeH; y += t3D[i].context->pcDownSample) {
                    for(x=0; x < tData[di].nubeW; x += t3D[i].context->pcDownSample) {
                        d = rawPix[y * tData[di].nubeW + x];
                        if(d != 0) {
                            Point2D.X   = x;
                            Point2D.Y   = y;
                            Point2D.Z   = (float)d;
                            try {

                                Xn_depth->ConvertProjectiveToRealWorld(1, &Point2D, &Point3D);
                                v1.set(Point3D.X, Point3D.Y, Point3D.Z);

                                vt = transformPoint(v1, matrix);
                                if(((int)vt->x == 0) && ((int)vt->x == 0) && ((int)vt->x == 0)) {

                                } else {
                                    tmpX[tData[di].nubeLength] = vt->x;
                                    tmpY[tData[di].nubeLength] = vt->y;
                                    tmpZ[tData[di].nubeLength] = vt->z;
                                    tData[di].nubeLength ++;
                                }
                                delete vt;

                            } catch(...) {
                                ofLogVerbose() << "[Grabber::updateThreadData] " << "Excepción al transformar los puntos.";
                            }
                        }
                    }
                }

                if(tData[di].xpix != NULL) {
                    delete tData[di].xpix;
                    delete tData[di].ypix;
                    delete tData[di].zpix;
                    tData[di].xpix = NULL;
                    tData[di].ypix = NULL;
                    tData[di].zpix = NULL;
                }

                if(tData[di].nubeLength > 0) {
                    tData[di].xpix = new float[tData[di].nubeLength];
                    tData[di].ypix = new float[tData[di].nubeLength];
                    tData[di].zpix = new float[tData[di].nubeLength];

                    memcpy(tData[di].xpix,     tmpX,     sizeof(float) * tData[di].nubeLength);
                    memcpy(tData[di].ypix,     tmpY,     sizeof(float) * tData[di].nubeLength);
                    memcpy(tData[di].zpix,     tmpZ,     sizeof(float) * tData[di].nubeLength);
                }

                if(tmpX != NULL) {
                    delete tmpX;
                    delete tmpY;
                    delete tmpZ;
                }

                //ofLogVerbose() << "[Grabber::updateThreadData] " << " saliendo del for for.";

            }
        }
        gettimeofday(&tData[di].curTime, NULL);
        t3D[i].unlock();
        di++;
    }

}

ofVec3f * Grabber::transformPoint(ofVec3f point, ofMatrix4x4 transform) {
    ofVec4f v;
    v.set(point.x, point.y, point.z, 1);
    v            = transform * v;
    ofVec3f * v3 = new ofVec3f(v.x, v.y, v.z);
    return v3;
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
